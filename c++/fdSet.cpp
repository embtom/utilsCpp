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

#include <fdSet.h>
#include <sstream> //std::stringstream
#include <string.h> //strerror
#include <errno.h> //errno
#include <unistd.h> //write, close
#include <sys/epoll.h> //epoll
#include <error_msg.hpp> //buildErrorMessage
#include <list>

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
   explicit CFdSetPrivate();
   ~CFdSetPrivate();
   void AddFd(int fd, CFdSet::Callback cb);
   void RemoveFd(int fd);
   CFdSetRetval Select();
   void UnBlock();

private:
   std::list<event_data>   m_eventData;
   int                     m_epollFd {0};
   int                     m_unBlockFd[2] {0};
};
} //utils

using namespace utils;
CZU_DEFINE_OPAQUE_DELETER(CFdSetPrivate)

//*****************************************************************************
// Method definitions "CFdSetPrivate"

CFdSetPrivate::CFdSetPrivate()
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
   AddFd(m_unBlockFd[0], nullptr);
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
   m_eventData.emplace_back(event_data{.fd = fd, .cb = cb});

   epoll_event epev = {0};
   epev.events = EPOLLIN;
   epev.data.ptr = &m_eventData.back();

   if(epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &epev) == -1)
   {
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Failed to add fd to epoll: ", strerror(errno)));
   }
}

void CFdSetPrivate::RemoveFd(int fd)
{
   auto it = std::find_if(m_eventData.begin(), m_eventData.end(), [&fd] (event_data& elm)
   {
      if(elm.fd == fd) {
         return true;
      }
      else {
         return false;
      }
   });

   if (it == m_eventData.end()) {
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Fd to remove not available"));
   }

   if(epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
   {
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Failed to remove fd to epoll: ", strerror(errno)));
   }

   m_eventData.erase(it);
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
         if(pEventData->fd == m_unBlockFd[0]) {
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
   if(write(m_unBlockFd[1],&dummy, sizeof(dummy)) == -1)
   {
      throw std::runtime_error(utils::buildErrorMessage("CFdSetPrivate::",__func__," Failed to write to unblock pipe: ", strerror(errno)));
   }
}

//*****************************************************************************
// Method definitions "CFdSet"
CFdSet::CFdSet() :
   m_pPrivate(utils::make_unique_opaque<CFdSetPrivate>())
{ }

CFdSet::~CFdSet()
{ }

void CFdSet::AddFd(int fd, Callback cb)
{
   if (!cb) {
      throw std::runtime_error(utils::buildErrorMessage("CFdSet::",__func__," No callback passed "));
   }
   m_pPrivate->AddFd(fd, cb);
}

void CFdSet::RemoveFd(int fd)
{
   m_pPrivate->RemoveFd(fd);
}

CFdSetRetval CFdSet::Select()
{
   return m_pPrivate->Select();
}

void CFdSet::UnBlock()
{
   m_pPrivate->UnBlock();
}
