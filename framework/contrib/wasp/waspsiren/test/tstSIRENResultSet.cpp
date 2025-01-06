#include "waspsiren/SIRENResultSet.h"
#include "waspcore/TreeNodePool.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace wasp;

class FakeAdapter
{
  public:
    FakeAdapter(const std::string& name = "fake",
                const std::string& data = "adapter")
        : name(name), data(data)
    {
    }
    FakeAdapter(const FakeAdapter& orig)
        : name(orig.name), data(orig.data), children(orig.children)
    {
    }
    ~FakeAdapter() {}
    std::string              name;
    std::string              data;
    std::vector<FakeAdapter> children;
};

TEST(SIREN, result)
{
    SIRENResultSet<FakeAdapter> set;
    ASSERT_EQ(0, set.result_count());

    set.push(FakeAdapter("fake", "adapter"));
    ASSERT_EQ(1, set.result_count());
    ASSERT_TRUE(set.is_adapted(0));
    ASSERT_FALSE(set.is_scalar(0));
    ASSERT_EQ("fake", set.adapted(0).name);
    ASSERT_EQ("adapter", set.adapted(0).data);
    set.push("count", "3");
    ASSERT_EQ(2, set.result_count());
    ASSERT_EQ(2, set.size());
    ASSERT_FALSE(set.is_adapted(1));
    ASSERT_TRUE(set.is_scalar(1));

    for (std::size_t i = 0, index = 2; i < 10; ++i, ++index)
    {
        set.push(FakeAdapter("fake" + std::to_string(i),
                             "adapter" + std::to_string(i)));
        ASSERT_EQ(index + 1, set.result_count());
        ASSERT_TRUE(set.is_adapted(index));
        ASSERT_FALSE(set.is_scalar(index));
        ASSERT_EQ("fake" + std::to_string(i), set.adapted(index).name);
        ASSERT_EQ("adapter" + std::to_string(i), set.adapted(index).data);
    }
    // check initial pushs to ensure no mischief has occurred
    ASSERT_TRUE(set.is_adapted(0));
    ASSERT_FALSE(set.is_scalar(0));
    ASSERT_EQ("fake", set.adapted(0).name);
    ASSERT_EQ("adapter", set.adapted(0).data);
}
