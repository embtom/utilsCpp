#include <gtest/gtest.h>
#include <iostream>
#include <cstring>
#include <enum_reflect.hpp>

DECLARE_ENUM(EnumTest,int, val1=4, val2=2, val3=0x20, val4=0b101, val5=052);

TEST(EnumReflect, value)
{
    EnumTest a = EnumTest::val1;
    EXPECT_EQ(static_cast<int>(a), 4);

    a = EnumTest::val2;
    EXPECT_EQ(static_cast<int>(a), 2);

    a = EnumTest::val3;
    EXPECT_EQ(static_cast<int>(a), 32);

    a = EnumTest::val4;
    EXPECT_EQ(static_cast<int>(a), 5);

    a = EnumTest::val5;
    EXPECT_EQ(static_cast<int>(a), 42);
}

TEST(EnumReflect, toString)
{
    EnumTest a = EnumTest::val1;
    EXPECT_EQ(EnumToString(a), "val1");

    a = EnumTest::val2;
    EXPECT_EQ(EnumToString(a), "val2");

    a = EnumTest::val3;
    EXPECT_EQ(EnumToString(a), "val3");

    a = EnumTest::val4;
    EXPECT_EQ(EnumToString(a), "val4");

    a = EnumTest::val5;
    EXPECT_EQ(EnumToString(a), "val5");
}


#define CLEAR_BUFFER buffer.str("");

class StdoutEnumReflect : public ::testing::Test {
protected:
    StdoutEnumReflect()
    { }
    ~StdoutEnumReflect() override = default;
    void SetUp() override
    {
        sbuf = std::cout.rdbuf();
        // Redirect cout to our stringstream buffer or any other ostream
        std::cout.rdbuf(buffer.rdbuf());
    }
    // Called after each unit test
    void TearDown() override {
        // When done redirect cout to its old self
        std::cout.rdbuf(sbuf);
        sbuf = nullptr;
    }
    // The following objects can be reused in each unit test
    // This can be an ofstream as well or any other ostream
    std::stringstream buffer{};
    // backup of std cout streambuf
    std::streambuf *sbuf {nullptr};
};

TEST_F(StdoutEnumReflect, StackOverflowTest)
{
    EnumTest a = EnumTest::val1;
    {
        std::cout << a << std::endl;
        std::string actual{buffer.str()};
        EXPECT_EQ(actual,"val1\n");
        CLEAR_BUFFER;
    }
    {
        a = EnumTest::val2;
        std::cout << a << std::endl;
        std::string actual{buffer.str()};
        EXPECT_EQ(actual,"val2\n");
        CLEAR_BUFFER;
    }
    {
        a = EnumTest::val3;
        std::cout << a << std::endl;
        std::string actual{buffer.str()};
        EXPECT_EQ(actual,"val3\n");
        CLEAR_BUFFER;
    }
    {
        a = EnumTest::val4;
        std::cout << a << std::endl;
        std::string actual{buffer.str()};
        EXPECT_EQ(actual,"val4\n");
        CLEAR_BUFFER;
    }
    {
        a = EnumTest::val5;
        std::cout << a << std::endl;
        std::string actual{buffer.str()};
        EXPECT_EQ(actual,"val5\n");
        CLEAR_BUFFER;
    }
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}