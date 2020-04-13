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

#ifndef THREADWORKER_H
#define THREADWORKER_H

//******************************************************************************
// Header
#include <iostream>
#include <atomic>
#include <utility>
#include <functional>
#include <thread>
#include <chrono>
#include <templateHelpers.h>

namespace utils
{
    enum class EScheduling
    {
        OTHER,
        FIFO,
        RR,
        BATCH,
	    IDLE,
        FAILED
    };

//*****************************************************************************
//! \brief CThreadWorker
//!
class CThreadWorker
{
public:
    CThreadWorker() = delete;
    CThreadWorker(const CThreadWorker&) = delete;
    CThreadWorker& operator=(const CThreadWorker&) = delete;

    template<typename Callable, typename Name, typename... Args, std::enable_if_t<is_string<Name>::value, int> = 0>
    CThreadWorker(Callable&& f, Name&& name, Args&&... args) noexcept :
        m_name(std::forward<Name>(name))
    {
        try
        {
            m_threadFunc = [this, f = std::forward<Callable>(f), args = std::make_tuple(std::forward<Args>(args) ...)]()
            {
                m_running = true;
                while (m_running)
                {
                    auto start = std::chrono::steady_clock::now();
                    if(std::apply(f,args))
                    {   /* Thread loop should exit */
                        m_running = false;
                        continue;
                    }
                    auto end = std::chrono::steady_clock::now();
                    auto elapsed = end - start;
                    auto timeToWait = m_interval - elapsed;
                    if (timeToWait > std::chrono::milliseconds::zero()) {
                        std::this_thread::sleep_for(timeToWait);
                    }
                }
            };
        }
        catch(std::exception& e)
        {
            std::cerr <<  "ERR CThreadLoop Error: " << e.what() << std::endl;
        }
    }

    ~CThreadWorker() noexcept {
        stop();
    }

    template<typename Rep, typename Period>
    void start(const std::chrono::duration<Rep, Period> interval) noexcept
    {
        m_interval = std::chrono::duration_cast<decltype(m_interval)>(interval);
        m_thread = std::thread(m_threadFunc);
        setThreadName(m_name);

        switch(m_threadState)
        {
            case EThreadParam::NOTHING: break;
            case EThreadParam::CACHED:
            {
                setScheduling(m_sched, m_prio);
                m_threadState = EThreadParam::SET;
                break;
            }
            case EThreadParam::SET:
            {
                setScheduling(m_sched, m_prio);
                break;
            }
        }

        EScheduling sched;
        int prio;
        std::tie (sched, prio) = getScheduling();
        m_sched = sched;
        m_prio = prio;
    }

    void stop() noexcept;

    void waitUntilFinished() noexcept;

    std::string getName() const noexcept;

    EScheduling getScheduler() const noexcept;
    int getPrio() const noexcept;
    bool setThreadParam(EScheduling sched, int prio) noexcept;

private:
    enum class EThreadParam {
        NOTHING,
        CACHED,
        SET
    };

    void setThreadName(const std::string& name) noexcept;
    std::tuple<EScheduling,int> getScheduling() const noexcept;
    bool setScheduling(EScheduling sched, int prio) noexcept;

    std::atomic<bool>           m_running {false};
    mutable std::thread         m_thread;
    std::function<void(void)>   m_threadFunc;

    EScheduling                 m_sched {EScheduling::FAILED};
    int                         m_prio {0};
    EThreadParam                m_threadState {EThreadParam::NOTHING};
    std::string                 m_name;
    std::chrono::nanoseconds    m_interval {std::chrono::nanoseconds::zero()};
};

}

#endif //THREADWORKER_H
