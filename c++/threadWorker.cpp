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
#include <threadWorker.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>

using namespace utils;

void CThreadWorker::waitUntilFinished() noexcept
{
    if(m_thread.joinable()) {
        m_thread.join();
    }
}

void CThreadWorker::stop() noexcept
{
    if(m_thread.joinable()) {
        m_running = false;
        m_thread.join();
    }
}

std::string CThreadWorker::getName() const noexcept
{
    return m_name;
}

EScheduling CThreadWorker::getScheduler() const noexcept
{
    return m_sched;
}

int CThreadWorker::getPrio() const noexcept
{
    return m_prio;
}

bool CThreadWorker::setThreadParam(EScheduling sched, int prio) noexcept
{
    if (!m_thread.joinable()) {
        m_sched = sched;
        m_prio = prio;
        m_threadState = EThreadParam::CACHED;
        return true;
    }

    if (setScheduling(sched, prio)) {
        m_sched = sched;
        m_prio = prio;
        return true;
    }
    return false;
}

bool CThreadWorker::setScheduling(EScheduling sched, int prio) noexcept
{
    sched_param schedPrio { prio };
    int schedPolicy;

    switch(sched)
    {
       	case EScheduling::OTHER : schedPolicy = SCHED_OTHER; break;
		case EScheduling::FIFO  : schedPolicy = SCHED_FIFO;  break;
		case EScheduling::RR    : schedPolicy = SCHED_RR;    break;
		case EScheduling::BATCH : schedPolicy = SCHED_BATCH; break;
		case EScheduling::IDLE  : schedPolicy = SCHED_IDLE;  break;
        default                 : schedPolicy = SCHED_OTHER; break;
    }

    if (pthread_setschedparam(m_thread.native_handle(),schedPolicy,&schedPrio) != 0) {
        std::cout << "Failed to setschedparam: " << strerror(errno) << std::endl; 
        return false;
    }
    return true;
}

std::tuple<EScheduling,int> CThreadWorker::getScheduling() const noexcept
{
    sched_param prio;
    int policy;
    if (pthread_getschedparam(m_thread.native_handle(), &policy, &prio) == -1) {
        return std::tuple(EScheduling::FAIL, -1);
    }

    EScheduling sched;
    switch(policy)
    {
       	case SCHED_OTHER: sched = EScheduling::OTHER; break;
		case SCHED_FIFO:  sched = EScheduling::FIFO;  break;
		case SCHED_RR:    sched = EScheduling::RR;    break;
		case SCHED_BATCH: sched = EScheduling::BATCH; break;
		case SCHED_IDLE:  sched = EScheduling::IDLE;  break;
        default:          sched = EScheduling::FAIL;  break;
    }
    return std::tuple(sched, prio.sched_priority);
}

void CThreadWorker::setThreadName(const std::string& name) noexcept
{
    try
    {
        pthread_setname_np(m_thread.native_handle(), name.c_str());
    }
    catch(std::exception& e)
    {
        std::cerr <<  "ERR Set pthread_setname_np: " << e.what() << std::endl;
    }
}
