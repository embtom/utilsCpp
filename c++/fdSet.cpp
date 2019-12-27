/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/embtom)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//******************************************************************************
// Header
#include "fdSet.h"
#include <sstream> //std::stringstream
#include <string.h> //strerror
#include <errno.h> //errno
#include <unistd.h> //write, close
#include <sys/select.h> //select


using namespace utils;

//*****************************************************************************
// Method definitions "CFdSet"
CFdSet::CFdSet()
{
   if(pipe(m_unBlockFd) == -1)
   {
      std::stringstream ss;
      ss << "Failed to create pipe with error: " << strerror(errno);
      throw std::runtime_error(ss.str());
   }
}

CFdSet::~CFdSet()
{
   UnBlock();
   close(m_unBlockFd[0]);
   close(m_unBlockFd[1]);
}

void CFdSet::AddFd(int fd) noexcept
{
   m_fds.push_back(fd);
}

CFdSetRetval CFdSet::Select(Callback cb)
{
   int maxFd{0};
   fd_set fdSet;
   FD_ZERO(&fdSet);

   auto setFd = [&maxFd, &fdSet] (int fd ) 
   {
      maxFd = std::max(maxFd, fd);
      FD_SET(fd, &fdSet);
   };

   setFd(m_unBlockFd[1]);

   for(auto& fd : m_fds)
   {
      setFd(fd);
   }
   
   const int ret = select(maxFd+1, &fdSet, NULL, NULL, NULL);
   switch (ret)
   {
      case  0:
      {
         std::stringstream ss;
         ss << "Timeout occured: " << strerror(errno);
         throw std::runtime_error(ss.str());
      }
      case -1:
      {
         std::stringstream ss;
         ss << "Select failed with error: " << strerror(errno);
         throw std::runtime_error(ss.str());
      }
      default: 
      {
         break;
      }
   }

   if(FD_ISSET(m_unBlockFd[1],&fdSet))
   {
      return CFdSetRetval::UNBLOCK;
   }

   for(auto& fd : m_fds)
   {
      if(FD_ISSET(fd,&fdSet))
      {
         cb(fd);
         CFdSetRetval::OK;
      }
   }
}

void CFdSet::UnBlock()
{
   int dummy = 1;

   if(write(m_unBlockFd[0],&dummy, sizeof(dummy)) == -1)
   {
      std::stringstream ss;
      ss << "Failed to write to unblock pipe: " << strerror(errno);
      throw std::runtime_error(ss.str());
   }
}
