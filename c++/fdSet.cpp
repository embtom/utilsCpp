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
#include <sys/epoll.h> //epoll
#include <error_msg.hpp> //buildErrorMessage


namespace utils
{
struct event_data 
{
   int fd;
   CFdSet::Callback cb;
};

//*****************************************************************************
//! \brief CFdSet
//!
class CFdSetPrivate
{

public:
   CFdSetPrivate(unsigned int maxEvents);
   ~CFdSetPrivate();
   void AddFd(int fd, CFdSet::Callback cb);
   CFdSetRetval Select();
   void UnBlock();

private:
   const int               m_maxEvents;
   std::vector<event_data> m_eventData;
   int                     m_epollFd {0};
   int                     m_unBlockFd[2];
};
} //utils

using namespace utils;
CZU_DEFINE_OPAQUE_DELETER(CFdSetPrivate)

//*****************************************************************************
// Method definitions "CFdSetPrivate"

CFdSetPrivate::CFdSetPrivate(unsigned int maxEvents) :
   m_maxEvents(m_maxEvents)
{
   if(pipe(m_unBlockFd) == -1)
   {
      throw std::runtime_error(utils::buildErrorMessage("CFdSet::", __func__, " Failed to create pipe with error: ", strerror(errno)));
   }

   int ret = epoll_create1(EPOLL_CLOEXEC);
   if (ret == -1)
   {
      close(m_unBlockFd[0]);
      close(m_unBlockFd[1]);
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::", __func__, " Failed to open epoll with error: ", strerror(errno)));
   }
   m_epollFd = ret;
   m_eventData.reserve(maxEvents);

   AddFd(m_unBlockFd[1], nullptr);
}

CFdSetPrivate::~CFdSetPrivate()
{
   UnBlock();
   close(m_epollFd);
   close(m_unBlockFd[0]);
   close(m_unBlockFd[1]);
}

void CFdSetPrivate::AddFd(int fd, CFdSet::Callback cb)
{
   if (m_eventData.size() >= m_maxEvents)
   {
        throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::", __func__, " max_events reached "));
   }
   m_eventData.emplace_back(event_data{.fd = fd, .cb = cb});
   
   epoll_event epev = {0};
   epev.events = EPOLLIN;
   epev.data.ptr = &m_eventData.back();

   if(epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &epev) == -1)
   {
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Failed to add fd to epoll: ", strerror(errno)));
   }
}

CFdSetRetval CFdSetPrivate::Select()
{
   int ret;
   epoll_event epev[5] = {0};
   CFdSetRetval ERet = CFdSetRetval::OK;

   do {
	   ret = epoll_wait(m_epollFd, &epev[0], 5, -1);
		if ((ret == -1) && (errno != EINTR))
      {
         throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Failed to epoll_wait: ", strerror(errno)));   
      }

      for(int i = 0; i < ret; i++) 
      {
         event_data *pEventData = static_cast<event_data*>(epev[i].data.ptr);
         if(pEventData->fd == m_unBlockFd[1]) {
            ERet = CFdSetRetval::UNBLOCK;
         }
         else {
            pEventData->cb(pEventData->fd);   
         }
      }
	} while ((ret == -1) && (errno == EINTR));

   return ERet;
}

void CFdSetPrivate::UnBlock()
{
   int dummy = 1;
   if(write(m_unBlockFd[0],&dummy, sizeof(dummy)) == -1)
   {
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Failed to write to unblock pipe: ", strerror(errno)));
   }
}

//*****************************************************************************
// Method definitions "CFdSet"
CFdSet::CFdSet(unsigned int maxEvents) :
   m_pPrivate(utils::make_unique_opaque<CFdSetPrivate>(maxEvents))
{ }

CFdSet::~CFdSet()
{ }

void CFdSet::AddFd(int fd, Callback cb)
{
   m_pPrivate->AddFd(fd, cb);
}

CFdSetRetval CFdSet::Select()
{
   return m_pPrivate->Select();
}

void CFdSet::UnBlock()
{
   m_pPrivate->UnBlock();
}
