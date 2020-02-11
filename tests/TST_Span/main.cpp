#include <gtest/gtest.h>
#include <span.h>
#include <vector>
#include <algorithm>
#include "make_vector.hpp"


#define GTEST_BOX                   "[     cout ] "


struct testObj
{
    bool operator==(const testObj& other) const
    {
        if ((other.idx == idx) &&
            (other.a32 == a32) &&
            (other.b16 == b16) &&
            (other.c16 == c16))
        {
            return true;
        }
        return false;
    }
    bool operator!=(const testObj& other) const
    {
        return (other==*this) ? false : true;
    }
    uint32_t idx;
    uint32_t a32;
    uint16_t b16;
    uint16_t c16;
};

struct testObjMirror : public testObj
{ };

TEST(SpanTest, arrays)
{
    testObj obj1 {0, 0xaabbccdd, 0xbbaa, 0xddcc};
    testObj obj2[6] {{0, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {1, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {2, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {3, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {4, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {5, 0xaabbccdd, 0xbbaa, 0xddcc}};

    std::array<std::remove_all_extents_t<decltype(obj2)>,sizeof(obj2)/sizeof(*obj2)> obj3;
    std::copy(std::begin(obj2), std::end(obj2), std::begin(obj3));

    {
        //Basic instanciation
        utils::span<testObj> spanObj1 (obj1);
        EXPECT_EQ(spanObj1.size(), 1);
        EXPECT_EQ(spanObj1.front()==obj1,true);
        EXPECT_EQ(spanObj1.front(), spanObj1.back());
        EXPECT_NE(spanObj1.data(), nullptr);
        EXPECT_EQ(*spanObj1.data(),obj1);
    }
    {
        utils::span<testObj> spanObj2 (obj2);
        EXPECT_EQ(spanObj2.size(), sizeof(obj2)/sizeof(*obj2));
        EXPECT_EQ(std::equal(spanObj2.begin(), spanObj2.end(), std::begin(obj2)),true);
        EXPECT_NE(spanObj2.front(), spanObj2.back());
        int i = 0;
        for(auto it = spanObj2.cbegin(); it != spanObj2.cend(); ++it, i++)
        {
            EXPECT_EQ((*it)==obj2[i],true);
        }

        for(i = 0; i < spanObj2.size(); i++)
        {
            EXPECT_EQ(*(spanObj2.data() + i), obj2[i]);
        }

        EXPECT_NE(spanObj2.data(), nullptr);
    }
    {
        utils::span<testObj> spanObj3 (obj3);
        EXPECT_EQ(std::equal(spanObj3.begin(), spanObj3.end(), obj3.begin()),true);
        EXPECT_EQ(spanObj3.size(), obj3.size());
        EXPECT_NE(spanObj3.data(), nullptr);
    }


    {
        //Test of deduction rules
        utils::span spanObj1 (obj1);
        EXPECT_EQ(spanObj1.size(), 1);

        #ifndef __clang__
            utils::span spanObj2 (obj2);
            EXPECT_EQ(spanObj2.size(), sizeof(obj2)/sizeof(*obj2));
        #endif

        utils::span spanObj3 (obj3);
    }
}

TEST(SpanTest, Container)
{
    std::vector<testObj> obj6 = utils::make_vector(testObj({0, 0xaabbccdd, 0xbbaa, 0xddcc}),
                                                   testObj({1, 0xaabbccdd, 0xbbaa, 0xddcc}));
    utils::span<testObj> spanObjVector (obj6);

    EXPECT_EQ(spanObjVector.size(), obj6.size());
    EXPECT_EQ(std::equal(spanObjVector.begin(), spanObjVector.end(), std::begin(obj6)),true);
    EXPECT_NE(spanObjVector.front(), spanObjVector.back());
    int i = 0;
    for(auto it = spanObjVector.cbegin(); it != spanObjVector.cend(); ++it, i++)
    {
        EXPECT_EQ((*it)==obj6[i],true);
    }
    for(i = 0; i < spanObjVector.size(); i++)
    {
        EXPECT_EQ(*(spanObjVector.data() + i), obj6[i]);
    }

    EXPECT_NE(spanObjVector.data(), nullptr);
}

TEST(SpanTest, SpanCopy)
{
    testObj obj2[] {{0, 0xaabbccdd, 0xbbaa, 0xddcc},
                {1, 0xaabbccdd, 0xbbaa, 0xddcc},
                {2, 0xaabbccdd, 0xbbaa, 0xddcc},
                {3, 0xaabbccdd, 0xbbaa, 0xddcc},
                {4, 0xaabbccdd, 0xbbaa, 0xddcc},
                {5, 0xaabbccdd, 0xbbaa, 0xddcc}};

    utils::span<testObj> baseSpan (obj2);
    utils::span copySpan (baseSpan);

    EXPECT_EQ(baseSpan.size(), copySpan.size());
    EXPECT_EQ(std::equal(baseSpan.begin(), baseSpan.end(), copySpan.begin()),true);

    auto copySpan2 = std::move(copySpan);
    EXPECT_EQ(baseSpan.size(), copySpan.size());
    EXPECT_EQ(std::equal(baseSpan.begin(), baseSpan.end(), copySpan2.begin()),true);
}

TEST(SpanTest, makeSpan)
{
    testObj obj1 {0, 0xaabbccdd, 0xbbaa, 0xddcc};
    testObj obj2[] {{0, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {1, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {2, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {3, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {4, 0xaabbccdd, 0xbbaa, 0xddcc},
                    {5, 0xaabbccdd, 0xbbaa, 0xddcc}};

    std::array<std::remove_all_extents_t<decltype(obj2)>,sizeof(obj2)/sizeof(*obj2)> obj3;
    std::copy(std::begin(obj2), std::end(obj2), std::begin(obj3));

    {
        utils::span spanObj1 = utils::make_span<testObj>(obj1);
        EXPECT_EQ(spanObj1.size(), 1);
        EXPECT_EQ(spanObj1.front()==obj1,true);
        EXPECT_EQ(spanObj1.front(), spanObj1.back());
        EXPECT_NE(spanObj1.data(), nullptr);
        EXPECT_EQ(*spanObj1.data(),obj1);
    }
    {
        utils::span spanObj2 = utils::make_span<testObj>(obj2);
        EXPECT_EQ(spanObj2.size(), sizeof(obj2)/sizeof(*obj2));
        EXPECT_EQ(std::equal(spanObj2.begin(), spanObj2.end(), std::begin(obj2)),true);
        EXPECT_NE(spanObj2.front(), spanObj2.back());
        int i = 0;
        for(auto it = spanObj2.cbegin(); it != spanObj2.cend(); ++it, i++)
        {
            EXPECT_EQ((*it)==obj2[i],true);
        }

        for(i = 0; i < spanObj2.size(); i++)
        {
            EXPECT_EQ(*(spanObj2.data() + i), obj2[i]);
        }

        EXPECT_NE(spanObj2.data(), nullptr);
    }
    {
        utils::span spanObj3 = utils::make_span<testObj>(obj3);
        EXPECT_EQ(spanObj3.size(), obj3.size());
        EXPECT_EQ(std::equal(spanObj3.begin(), spanObj3.end(), obj3.begin()),true);
        EXPECT_NE(spanObj3.data(), nullptr);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
