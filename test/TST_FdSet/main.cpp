#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <unistd.h>
#include <fdSet.h>

#define ANSI_TXT_GRN                "\033[0;32m"
#define ANSI_TXT_MGT                "\033[0;35m" //Magenta
#define ANSI_TXT_DFT                "\033[0;0m" //Console default
#define GTEST_BOX                   "[     cout ] "
#define COUT_GTEST                  ANSI_TXT_GRN << GTEST_BOX  
#define COUT_GTEST_MGT              COUT_GTEST << ANSI_TXT_DFT


TEST(EnumReflect, value)
{
    utils::CFdSet fdSet;

    fdSet.UnBlock();
} 

class CFdSetTest : public ::testing::Test {
protected:
    CFdSetTest() :
        m_threadRunning(false)
    {  }
    ~CFdSetTest() override
    {


    }
    // Called in front of each unit test
    void SetUp() override 
    {   
        m_threadRunning = true;
        m_testThd = std::thread(&CFdSetTest::testWorker,this);
    }
    // Called after each unit test
    void TearDown() override 
    {
        if(m_testThd.joinable()) {
            m_testThd.join();
        }        
    }

    void testWorker()
    {
        while (m_threadRunning) 
        {
            std::cout << COUT_GTEST_MGT << "Start of the testWorker" << std::endl;
            m_SelectRet = m_fdSet.Select();
            if (m_SelectRet == utils::CFdSetRetval::UNBLOCK) {
                m_threadRunning = false;
            }
        }
        std::cout << COUT_GTEST_MGT << "End of the testWorker" << std::endl;
    
    }

    std::thread m_testThd;
    std::atomic<bool> m_threadRunning;
    utils::CFdSet m_fdSet;
    utils::CFdSetRetval m_SelectRet;
    
};

TEST_F(CFdSetTest, CheckUnblock)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_fdSet.UnBlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(m_SelectRet, utils::CFdSetRetval::UNBLOCK);
}

TEST_F(CFdSetTest, CheckFdHandling)
{
    int senddummy = 255;
    int testFd[2] {0};
    ASSERT_EQ(pipe(testFd), 0);
    m_fdSet.AddFd(testFd[0],[&testFd, &senddummy](int fd)
    {
        int readDummy = 0;
        read(testFd[0],&readDummy,sizeof(readDummy));
        EXPECT_EQ(senddummy, readDummy);
        EXPECT_EQ(testFd[0], fd);
        std::cout << COUT_GTEST_MGT << "Unblock by Fd " << fd << " with " << readDummy << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
    ASSERT_EQ(write(testFd[1],&senddummy, sizeof(senddummy)), sizeof(senddummy));


    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    m_fdSet.RemoveFd(testFd[0]);
    m_fdSet.UnBlock();

    close(testFd[0]);
    close(testFd[1]);
}

TEST_F(CFdSetTest, CheckMultipleFdHandling)
{
    std::mutex m;
    std::condition_variable cond_var;
    std::atomic<int> fdSetCount {0};
    assert(fdSetCount.is_lock_free());

    int senddummy1 = 10;
    int testFd1[2] {0};
    ASSERT_EQ(pipe(testFd1), 0);
    m_fdSet.AddFd(testFd1[0],[&testFd1, &senddummy1, &fdSetCount, &m, &cond_var](int fd)
    {
        int readDummy = 0;
        read(testFd1[0],&readDummy,sizeof(readDummy));
        EXPECT_EQ(senddummy1, readDummy);
        EXPECT_EQ(testFd1[0], fd);
        std::cout << COUT_GTEST_MGT << "Unblock by Fd " << fd << " with " << readDummy << std::endl;
        std::unique_lock<std::mutex> lock(m);
        fdSetCount++;
        if (fdSetCount == 3) {
            cond_var.notify_one();
        }
    });

    int senddummy2 = 50;
    int testFd2[2] {0};
    ASSERT_EQ(pipe(testFd2), 0);
    m_fdSet.AddFd(testFd2[0],[&testFd2, &senddummy2, &fdSetCount, &m, &cond_var](int fd)
    {
        int readDummy = 0;
        read(testFd2[0],&readDummy,sizeof(readDummy));
        EXPECT_EQ(senddummy2, readDummy);
        EXPECT_EQ(testFd2[0], fd);
        std::cout << COUT_GTEST_MGT << "Unblock by Fd " << fd << " with " << readDummy << std::endl;
        std::unique_lock<std::mutex> lock(m);
        fdSetCount++;
        if (fdSetCount == 3) {
            cond_var.notify_one();
        }
    });

    int senddummy3 = 100;
    int testFd3[2] {0};
    ASSERT_EQ(pipe(testFd3), 0);
    m_fdSet.AddFd(testFd3[0],[&testFd3, &senddummy3, &fdSetCount, &m, &cond_var](int fd)
    {
        int readDummy = 0;
        read(testFd3[0],&readDummy,sizeof(readDummy));
        EXPECT_EQ(senddummy3, readDummy);
        EXPECT_EQ(testFd3[0], fd);
        std::cout << COUT_GTEST_MGT << "Unblock by Fd " << fd << " with " << readDummy << std::endl;
        std::unique_lock<std::mutex> lock(m);
        fdSetCount++;
        if (fdSetCount == 3) {
            cond_var.notify_one();
        }
    });

    ASSERT_EQ(write(testFd1[1],&senddummy1, sizeof(senddummy1)), sizeof(senddummy1));
    ASSERT_EQ(write(testFd2[1],&senddummy2, sizeof(senddummy2)), sizeof(senddummy2));
    ASSERT_EQ(write(testFd3[1],&senddummy3, sizeof(senddummy3)), sizeof(senddummy3));

    std::cv_status status = std::cv_status::no_timeout;
    
    {
        std::unique_lock<std::mutex> lock(m);
        do   
        {
            if (fdSetCount == 3)
                break;
            status = cond_var.wait_for(lock,std::chrono::seconds(2));
        } while((fdSetCount == 3) || (status != std::cv_status::timeout));
    }
    EXPECT_EQ(status, std::cv_status::no_timeout);
    
    m_fdSet.UnBlock();
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}