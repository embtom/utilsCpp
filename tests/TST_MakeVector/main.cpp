#include <gtest/gtest.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <enum_reflect.hpp>
#include <memory>
#include <make_vector.hpp>
#include <instanceCounter.hpp>

TEST(makeFunctions, makeVector)
{
    auto vec = utils::make_vector<counted<std::string>>("foo", "bar", "baz");

    auto contentCheck = [&vec] (const std::string& toCheck)
    {
        auto it = std::find_if(vec.begin(),vec.end(), [&toCheck] (const auto& elm )
        {
            if (elm.compare(toCheck) == 0) {
                return true;
            }
            return false;
        });

        EXPECT_EQ((*it),toCheck);
    };

    contentCheck(std::string("foo"));
    contentCheck(std::string("bar"));
    contentCheck(std::string("baz"));
    vec.clear();

    EXPECT_EQ(s_lastCounter.num_construct, 3);
    EXPECT_EQ(s_lastCounter.num_copy, 0);
    EXPECT_EQ(s_lastCounter.num_destruct, 3);
    EXPECT_EQ(s_lastCounter.num_move, 0);

    instance_counter<std::string>::clear();
}

enum class elements {elem1, elem2, elem3};

class elementBox
{
public:
    elementBox(elements elem) :
        m_element(elem)
    { }
private:
   elements m_element;
};


TEST(makeFunctions, makeVector2)
{
    auto vec = utils::make_vector<counted<elementBox>>(elements::elem1, elements::elem2, elements::elem3);
    vec.clear();

    EXPECT_EQ(s_lastCounter.num_construct, 3);
    EXPECT_EQ(s_lastCounter.num_copy, 0);
    EXPECT_EQ(s_lastCounter.num_destruct, 3);
    EXPECT_EQ(s_lastCounter.num_move, 0);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}