#include "waspcore/Interpreter.h"
#include "waspcore/TokenPool.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace wasp;

// fake type enumerations
char decl    = 0;
char term    = 1;
char integer = 2;
char real    = 3;
char word    = 4;
char paren   = 5;
char value   = 6;
char id      = 7;
char array   = 8;
char assign  = 9;
char root    = 10;
/**
 * @brief TEST getters
 * 'root'
 *  |_ 'data'
 *    |_ 'array'
 *    |_ '('
 *    |_ 'foo'
 *    |_ ')'
 *    |_ '234'
 *    |_ '1.2343'
 *    |_ 'end'
 */

TEST(NodeView, child)
{
    //'array (foo) 234 1.2343 end\n' // data
    // 0     67  1011  15     22     // offsets
    DummyInterp<TreeNodePool<
        char, char, TokenPool<char, char, default_file_offset_type_size>>>
                             interp;
    std::vector<size_t>      root_child_indices;
    size_t                   data_root_index;
    std::vector<std::string> token_data = {"array", "(",      "foo", ")",
                                           "234",   "1.2343", "end"};
    std::vector<std::string> node_name = {"decl",  "op",    "id",  "cp",
                                          "value", "value", "term"};
    std::vector<char> node_type  = {decl, paren, id, paren, value, value, term};
    std::vector<char> token_type = {word,    paren, word, paren,
                                    integer, real,  term};
    std::vector<default_file_offset_type_size> token_offset = {0,  6,  7, 10,
                                                               11, 15, 22};
    {  // push first line's contents

        ASSERT_EQ(token_data.size(), node_name.size());
        ASSERT_EQ(token_data.size(), node_type.size());
        ASSERT_EQ(token_data.size(), token_type.size());
        ASSERT_EQ(token_data.size(), token_offset.size());
        std::vector<size_t> child_indices;
        for (std::size_t i = 0; i < token_data.size(); ++i)
        {
            {
                SCOPED_TRACE(i);
                // capture index of new leaf node
                child_indices.push_back(static_cast<char>(i));
                size_t token_index = interp.token_count();
                interp.push_token(token_data[i].c_str(), token_type[i],
                                  token_offset[i]);
                interp.push_leaf(node_type[i], node_name[i].c_str(),
                                 token_index);
                ASSERT_EQ(i,
                          interp.leaf_index(i));  // already leaf, so same index
                ASSERT_EQ(1, interp.line(i));
                ASSERT_EQ(node_type[i], interp.type(i));
                ASSERT_EQ(node_name[i], interp.name(i));
                ASSERT_EQ(token_offset[i] + 1, interp.column(i));
                ASSERT_EQ(i + 1, interp.size());
                ASSERT_EQ(token_type[i], interp.node_token_type(i));
            }
        }
        // add parent node
        std::string parent_name = "data";
        char        parent_type = array;
        // create parent node with type, name, and children
        root_child_indices.push_back(interp.size());  // capture
        data_root_index = interp.size();
        interp.push_parent(parent_type, parent_name.c_str(), child_indices);
        ASSERT_EQ(7 + 1, interp.size());
        ASSERT_EQ(
            0,
            interp.leaf_index(interp.size() - 1));  // parent of first leaf node
        ASSERT_EQ(1, interp.line(interp.size() - 1));
        ASSERT_EQ(1, interp.column(interp.size() - 1));
        default_file_offset_type_size line_offset =
            token_offset.back() + token_data.back().size();
        ASSERT_EQ(25, line_offset);
        interp.push_line_offset(line_offset);
    }  // done with first line

    std::string root_name = "root";
    interp.push_parent(root, root_name.c_str(), root_child_indices);
    ASSERT_EQ(root_name, interp.name(interp.size() - 1));
    ASSERT_EQ(root, interp.type(interp.size() - 1));

    {  // test data child_by_name(value)
        NodeView data_view(data_root_index, interp);
        ASSERT_FALSE(data_view.is_null());
        ASSERT_EQ(&interp, data_view.node_pool());
        std::string name          = data_view.name();
        std::string expected_name = "data";
        ASSERT_EQ(expected_name, name);
        {  // test empty scenario
            const std::string    expected_child_name = "ted";
            NodeView::Collection children =
                data_view.child_by_name(expected_child_name);
            ASSERT_TRUE(children.empty());
            ASSERT_EQ(0, data_view.child_count_by_name(expected_child_name));
            ASSERT_EQ(0, data_view.child_count_by_name(expected_child_name, 1));
        }
        {  // test multiple returned
            const std::string    expected_child_name = "value";
            NodeView::Collection children =
                data_view.child_by_name(expected_child_name);
            ASSERT_EQ(2, children.size());
            ASSERT_EQ(2, data_view.child_count_by_name(expected_child_name));
            ASSERT_EQ(1, data_view.child_count_by_name(expected_child_name, 1));
            ASSERT_EQ(2, data_view.child_count_by_name(expected_child_name, 3));
            // t iterates the token_data value information
            for (size_t t = 4, i = 0; i < children.size(); ++i, ++t)
            {
                const auto& child_view = children[i];
                std::string child_name = child_view.name();
                ASSERT_EQ(expected_child_name, "value");
                ASSERT_EQ(node_name[t], child_name);
                ASSERT_EQ(node_type[t], child_view.type());
            }
        }
        {  // first child
            const std::string child_name = "value";
            auto child_view = data_view.first_child_by_name(child_name);
            ASSERT_FALSE(child_view.is_null());
            std::string child_data = child_view.data();
            ASSERT_EQ("234", child_data);
            child_view.set_data("5678");
            child_data = child_view.data();
            ASSERT_EQ("5678", child_data);
        }
    }
    NodeView root_view(interp.size() - 1, interp);

    {// Test child at and iterator (using DefaultPush, equivalent to child_at(index))
        auto itr = root_view.begin();
        // Root has children so iterator should be valid
        EXPECT_TRUE(itr);
        size_t child_index = 0;
        while(itr)
        {
            SCOPED_TRACE(child_index);
            // Same node
            ASSERT_EQ(root_view.child_at(child_index), itr.get());
            itr.next(); child_index++;
            bool is_valid = (child_index < root_view.child_count());
            // if the stack is empty the child count has been exhausted
            ASSERT_EQ(itr.data().empty(), !is_valid);
            ASSERT_EQ(itr, is_valid);
        }
    }
    std::cout << root_view.data() << std::endl;
    std::stringstream xml;
    wasp::to_xml(root_view, xml);
    std::string expected_xml = R"INPUT(<root>
  <data>
    <decl loc="1.1">array</decl>
    <op loc="1.7">(</op>
    <id loc="1.8">foo</id>
    <cp loc="1.11">)</cp>
    <value loc="1.12">5678</value>
    <value loc="1.16">1.2343</value>
    <term loc="1.23">end</term>
  </data>
</root>
)INPUT";
    ASSERT_EQ(expected_xml, xml.str());
}
TEST(TreeNodePool, is_null_test)
{
    NodeView null_view;
    ASSERT_TRUE(null_view.is_null());
    DummyInterp<TreeNodePool<
        char, char, TokenPool<char, char, default_file_offset_type_size>>>
             interp;
    NodeView view(1, interp);
    ASSERT_FALSE(view.is_null());
}
TEST(TreeNodePool, operator_equal_test)
{
    NodeView null_view;
    ASSERT_TRUE(null_view.is_null());
    DummyInterp<TreeNodePool<
        char, char, TokenPool<char, char, default_file_offset_type_size>>>
             interp;
    NodeView view1(0, interp);
    NodeView view2(0, interp);
    NodeView view3(1, interp);
    ASSERT_EQ(view1, view2);
    ASSERT_NE(view1, view3);
}
TEST(NodeView, to_type_test)
{
    //'"QString1" \'QString2\' string 14 14e6 15.0 15.0e6 3.145' // data
    DummyInterp<TreeNodePool<
        char, char, TokenPool<char, char, default_file_offset_type_size>>>
        interp;

    {  // push the data with needed metadata for testing purposes
        std::vector<std::string> token_data = {
            "\"String0\"", "'String1'", "String2", "14",
            "14e6",        "15.0",      "3.145"};
        char type_test = -1;  // dont care - not testing

        for (std::size_t i = 0; i < token_data.size(); ++i)
        {
            char index = static_cast<char>(i);
            // only attribute we care about in this test is the data
            // dummy everything else
            interp.push_token(token_data[i].c_str(), type_test, 0);
            interp.push_leaf(type_test, "test", index);
        }
        // test strings
        ASSERT_LE(3, token_data.size());
        for (std::size_t i = 0; i < 3 /*strings*/; ++i)
        {
            std::string expected_string =
                std::string("String") + std::to_string(i);
            NodeView view(i, interp);
            EXPECT_EQ(expected_string, view.to_string());
            bool ok = false;
            EXPECT_EQ(expected_string, view.to_string(&ok));
            EXPECT_TRUE(ok);
        }
        // test all others as strings
        for (std::size_t i = 3; i < token_data.size(); ++i)
        {
            std::string expected_string = token_data[i];
            NodeView    view(i, interp);
            EXPECT_EQ(expected_string, view.to_string());
            bool ok = false;
            EXPECT_EQ(expected_string, view.to_string(&ok));
            EXPECT_TRUE(ok);
        }
        // test strings as doubles - failure
        for (std::size_t i = 0; i < 3 /*strings*/; ++i)
        {
            NodeView view(i, interp);
            EXPECT_EQ(0.0, view.to_double());
            bool ok = true;
            view.to_double(&ok);
            EXPECT_FALSE(ok);
        }
        // test strings as int - failure
        for (std::size_t i = 0; i < 3 /*strings*/; ++i)
        {
            NodeView view(i, interp);
            EXPECT_EQ(0, view.to_int());
            bool ok = true;
            view.to_int(&ok);
            EXPECT_FALSE(ok);
        }
    }
}
