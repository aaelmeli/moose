#include "gtest/gtest.h"

#include "waspcore/Definition.h"
using namespace wasp;
/**
 * @brief TEST definition match at existing section
 *
 */
TEST(Definition, current_section)
{
    Definition  definition;
    std::string aname;
    ASSERT_FALSE(definition.has("child"));
    ASSERT_EQ(-1, definition.delta("child", aname));
    ASSERT_EQ("", aname);  // no name found, unchanged

    AbstractDefinition* child = definition.create("child");
    ASSERT_EQ("child", child->name());
    ASSERT_TRUE(definition.has("child"));
    ASSERT_EQ(0, definition.delta("child", aname));
    ASSERT_EQ("child", aname);
    {
        AbstractDefinition* alias_of_child =
            definition.create_aliased("achild", child);
        ASSERT_EQ("achild", alias_of_child->name());
        ASSERT_EQ("child", alias_of_child->actual_name());
        ASSERT_TRUE(definition.has("achild"));
        ASSERT_EQ(0, definition.delta("child", aname));
        ASSERT_EQ("child", aname);
        ASSERT_EQ(0, definition.delta("achild", aname));
        ASSERT_EQ("child", aname);
    }
    {
        AbstractDefinition* alias_of_child =
            definition.create_aliased("bchild", child);
        ASSERT_EQ("bchild", alias_of_child->name());
        ASSERT_EQ("child", alias_of_child->actual_name());
        ASSERT_TRUE(definition.has("bchild"));
        ASSERT_TRUE(definition.has("achild"));
        ASSERT_EQ(0, definition.delta("child", aname));
        ASSERT_EQ("child", aname);
        ASSERT_EQ(0, definition.delta("achild", aname));
        ASSERT_EQ("child", aname);
        ASSERT_EQ(0, definition.delta("bchild", aname));
        ASSERT_EQ("child", aname);
        ASSERT_EQ(1, alias_of_child->delta("child", aname));
        ASSERT_EQ("child", aname);
        ASSERT_EQ(1, alias_of_child->delta("bchild", aname));
        ASSERT_EQ("child", aname);
    }
}

TEST(Definition, parent_section)
{
    Definition          definition;
    AbstractDefinition* child = definition.create("child");
    definition.create_aliased("achild", child);
    AbstractDefinition* grandchild1 = child->create("grandchild1");
    child->create_aliased("agrandchild1", grandchild1);
    AbstractDefinition* grandchild2 = child->create("grandchild2");
    std::string         aname;
    ASSERT_TRUE(child->has("grandchild1"));
    ASSERT_TRUE(child->has("agrandchild1"));
    ASSERT_EQ(0, child->delta("grandchild1", aname));
    ASSERT_EQ("grandchild1", aname);
    ASSERT_EQ(0, child->delta("agrandchild1", aname));
    ASSERT_EQ("grandchild1", aname);

    ASSERT_TRUE(child->has("grandchild2"));
    ASSERT_EQ(0, child->delta("grandchild2", aname));
    ASSERT_EQ("grandchild2", aname);

    ASSERT_EQ(1, grandchild1->delta("grandchild1", aname));
    ASSERT_EQ("grandchild1", aname);
    ASSERT_EQ(1, grandchild1->delta("grandchild2", aname));
    ASSERT_EQ("grandchild2", aname);
    aname = "this will not change because 'agrandchild2' doesn't exist - isn't "
            "found";
    ASSERT_EQ(-1, grandchild1->delta("agrandchild2", aname));
    ASSERT_EQ("this will not change because 'agrandchild2' doesn't exist - "
              "isn't found",
              aname);

    ASSERT_EQ(1, grandchild2->delta("grandchild1", aname));
    ASSERT_EQ("grandchild1", aname);
    ASSERT_EQ(1, grandchild2->delta("grandchild2", aname));
    ASSERT_EQ("grandchild2", aname);

    ASSERT_EQ(2, grandchild1->delta("child", aname));
    ASSERT_EQ("child", aname);
    ASSERT_EQ(2, grandchild2->delta("child", aname));
    ASSERT_EQ("child", aname);
    ASSERT_EQ(2, grandchild2->delta("achild", aname));
    ASSERT_EQ("child", aname);
}
