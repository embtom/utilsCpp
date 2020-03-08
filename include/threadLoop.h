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

#ifndef THREADLOOP_H
#define THREADLOOP_H

//******************************************************************************
// Header
#include <thread>
#include <atomic>
#include <utility>
#include <functional>
#include <memory>
#include <threadWorker.h>


namespace utils
{

//*****************************************************************************
//! \brief CThreadLoop
//!
class CThreadLoop
{
public:
    CThreadLoop() noexcept  = default;
    CThreadLoop(const CThreadLoop&) = delete;
    CThreadLoop& operator=(const CThreadLoop&) = delete;
    ~CThreadLoop() noexcept = default;

    template<typename Callable, typename Name, typename... Args, std::enable_if_t<is_string<Name>::value, int> = 0>
    CThreadLoop(Callable&& f, Name&& name, Args&&... args) noexcept :
        m_worker(std::make_unique<CThreadWorker>(std::forward<Callable>(f),
                                                 std::forward<Name>(name),
                                                 std::forward<Args>(args)...))
    { }

    CThreadLoop(CThreadLoop&& rhs) noexcept :
        m_worker (std::move(rhs.m_worker))
    { }

    CThreadLoop& operator=(CThreadLoop&& rhs) noexcept {
        m_worker = std::move(rhs.m_worker);
        return *this;
    }

    template<typename Rep, typename Period>
    void start(const std::chrono::duration<Rep, Period>& interval) noexcept {
        m_worker->start(interval);
    }

    void start() noexcept {
        m_worker->start(std::chrono::milliseconds::zero());
    }

    void stop() noexcept {
        m_worker->stop();
    }

    void waitUntilFinished() noexcept {
        m_worker->waitUntilFinished();
    }

    std::string getName() const noexcept {
        return m_worker->getName();
    }

    EScheduling getScheduler() const noexcept {
        return m_worker->getScheduler();
    }

    int getPrio() const noexcept {
        return m_worker->getPrio();
    }

    bool setThreadParam(EScheduling sched, int prio) noexcept {
        return m_worker->setThreadParam(sched, prio);
    }

private:
    std::unique_ptr<CThreadWorker>  m_worker;
};

}

#endif  // THREADLOOP_H