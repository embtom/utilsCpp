#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <threadLoop.h>


#define GTEST_BOX                   "[     cout ] "

using Clock = std::chrono::steady_clock;


TEST(ThreadLoop__Test, basicTest)
{
    std::string threadName ("intervalTestThread");
    std::chrono::milliseconds threadInterval (10);
    int cycles = 200;

    auto last (Clock::now());
    auto threadFunc = [&threadInterval, &last, &cycles] (const std::string& funcName)
    {
        if (cycles%50 == 0) {
            std::cout << funcName << " with cycle: " << cycles << std::endl;
        }
        auto now = Clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

        if (diff > threadInterval + std::chrono::milliseconds(1)) {
            ADD_FAILURE() << "Interval Exceeds with: " << diff.count() << std::endl;
        }
        last = now;
        cycles--;
        if( cycles > 0) {
            return false;
        }
        return true;
    };

    utils::CThreadLoop threadTest (threadFunc, threadName, threadName);
    threadTest.setThreadParam(EScheduling::OTHER, 0);
    threadTest.start(threadInterval);
    threadTest.waitUntilFinished();

    last = Clock::now();
    threadTest.start(threadInterval);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(threadTest.getScheduler(), EScheduling::OTHER);
    EXPECT_EQ(threadTest.getPrio(), 0);
    EXPECT_EQ(threadTest.getName(), threadName);
    threadTest.waitUntilFinished();

    cycles = 200;

    last = Clock::now();
    threadTest.start(threadInterval);

    utils::CThreadLoop moveObj (std::move(threadTest));
    EXPECT_EQ(moveObj.getScheduler(), EScheduling::OTHER);
    EXPECT_EQ(moveObj.getPrio(), 0);
    moveObj.waitUntilFinished();
}


TEST(ThreadLoop__Test, cyclicTest)
{

    utils::CThreadLoop b;
    {
        int passIntThread = 5;
        std::string passStringThread ("testString");

        auto func = [passIntThread, passStringThread](const int& passInt, const std::string& passString)
        {
            EXPECT_EQ(passIntThread, passInt);
            EXPECT_EQ(passStringThread, passString);
            return false;
        };
       utils::CThreadLoop a (func, "testWorker", passIntThread, passStringThread);
       a.start(std::chrono::milliseconds(1000));
       a.setThreadParam(EScheduling::OTHER, 0);
       b = std::move(a);
    }

    EXPECT_EQ(b.getScheduler(), EScheduling::OTHER);
    EXPECT_EQ(b.getPrio(), 0);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
