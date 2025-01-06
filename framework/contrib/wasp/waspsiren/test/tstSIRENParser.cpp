#include "waspsiren/SIRENInterpreter.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace wasp;

TEST(SIREN, parse_only_root)
{
    std::stringstream input;
    input << R"INPUT(/)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(3, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    auto root_selection = document.child_at(0);
    // when there exists only the root selection
    // the node type is a separator
    ASSERT_EQ(DOCUMENT_ROOT, root_selection.type());
}
TEST(SIREN, parse_simple_root)
{
    std::stringstream input;
    input << R"INPUT(/child)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    auto root_selection = document.child_at(0);
    // when there exists a root selection
    // with a child selection the node type is a declaration
    ASSERT_EQ(DOCUMENT_ROOT, root_selection.type());
    // '/' 'child'
    ASSERT_EQ(2, root_selection.child_count());
    ASSERT_EQ(SEPARATOR, root_selection.child_at(0).type());
    ASSERT_EQ(DECL, root_selection.child_at(1).type());
    std::string decl = "child";
    ASSERT_EQ(decl, root_selection.child_at(1).name());
}

TEST(SIREN, parse_simple_root_w_conditionally_predicated_child)
{
    std::stringstream input;
    input << R"INPUT(/child/gchild[id=1]/ggchild)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(17, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL}

        ,
        {"/", SEPARATOR},
        {"gchild", DECL},
        {"[", LBRACKET}

        ,
        {"id", DECL},
        {"==", EQ},
        {"value", VALUE},
        {"id", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"/", SEPARATOR},
        {"ggchild", DECL},
        {"O", OBJECT}

        ,
        {"O", OBJECT},
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_simple_root_w_index_predicated_child)
{
    std::stringstream input;
    input << R"INPUT(/child/gchild[1]/ggchild)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(15, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL}

        ,
        {"/", SEPARATOR},
        {"gchild", DECL},
        {"[", LBRACKET}

        ,
        {"int", INTEGER},
        {"I", INDEX}

        ,
        {"]", RBRACKET},
        {"ipcs", PREDICATED_CHILD}

        ,
        {"/", SEPARATOR},
        {"ggchild", DECL},
        {"O", OBJECT}

        ,
        {"O", OBJECT},
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_simple_root_w_indices_predicated_child)
{
    std::stringstream input;
    input << R"INPUT(/child/gchild[1:10]/ggchild)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(17, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL}

        ,
        {"/", SEPARATOR},
        {"gchild", DECL},
        {"[", LBRACKET}

        ,
        {"int", INTEGER},
        {":", COLON},
        {"int", INTEGER},
        {"I", INDEX}

        ,
        {"]", RBRACKET},
        {"ipcs", PREDICATED_CHILD}

        ,
        {"/", SEPARATOR},
        {"ggchild", DECL},
        {"O", OBJECT}

        ,
        {"O", OBJECT},
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_simple_root_w_strided_indices_predicated_child)
{
    std::stringstream input;
    input << R"INPUT(/child/gchild[1:10:2]/ggchild)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(19, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL}

        ,
        {"/", SEPARATOR},
        {"gchild", DECL},
        {"[", LBRACKET}

        ,
        {"int", INTEGER}  // start
        ,
        {":", COLON},
        {"int", INTEGER}  // end
        ,
        {":", COLON},
        {"int", INTEGER}  // stride
        ,
        {"I", INDEX}

        ,
        {"]", RBRACKET},
        {"ipcs", PREDICATED_CHILD}

        ,
        {"/", SEPARATOR},
        {"ggchild", DECL},
        {"O", OBJECT}

        ,
        {"O", OBJECT},
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_eq_pred)
{
    std::stringstream input;
    input << R"INPUT(/ child [id = 'x'] )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL},
        {"[", LBRACKET}

        ,
        {"id", DECL},
        {"==", EQ},
        {"value", VALUE},
        {"id", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_gte_pred)
{
    std::stringstream input;
    input << R"INPUT(/ child [r >= 3] )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL},
        {"[", LBRACKET}

        ,
        {"r", DECL},
        {">=", GTE},
        {"value", VALUE},
        {"r", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_lte_pred)
{
    std::stringstream input;
    input << R"INPUT(/ child [r <= 3] )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL},
        {"[", LBRACKET}

        ,
        {"r", DECL},
        {"<=", LTE},
        {"value", VALUE},
        {"r", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_neq_pred)
{
    std::stringstream input;
    input << R"INPUT(/ child [r != 3] )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL},
        {"[", LBRACKET}

        ,
        {"r", DECL},
        {"!=", NEQ},
        {"value", VALUE},
        {"r", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_lt_pred)
{
    std::stringstream input;
    input << R"INPUT(/ child [r < 3] )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL},
        {"[", LBRACKET}

        ,
        {"r", DECL},
        {"<", LT},
        {"value", VALUE},
        {"r", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_gt_pred)
{
    std::stringstream input;
    input << R"INPUT(/ child [r > 3] )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"/", SEPARATOR},
        {"child", DECL},
        {"[", LBRACKET}

        ,
        {"r", DECL},
        {">", GT},
        {"value", VALUE},
        {"r", KEYED_VALUE}

        ,
        {"]", RBRACKET},
        {"cpcs", PREDICATED_CHILD}

        ,
        {"R", DOCUMENT_ROOT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}

TEST(SIREN, parse_single_relative)
{
    std::stringstream input;
    input << R"INPUT( child )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(2, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"child", DECL}, {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_relative)
{
    std::stringstream input;
    input << R"INPUT( child/grandchild )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"child", DECL},
        {"/", SEPARATOR},
        {"grandchild", DECL},
        {"O", OBJECT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_relative_w_parent)
{
    std::stringstream input;
    input << R"INPUT( child/../sibling )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(8, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"child", DECL},  {"/", SEPARATOR},           {"P", PARENT},
        {"/", SEPARATOR}, {"sibling", DECL},          {"O", OBJECT},
        {"O", OBJECT},    {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_parent)
{
    std::stringstream input;
    input << R"INPUT( ../uncle )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"P", PARENT},
        {"/", SEPARATOR},
        {"uncle", DECL},
        {"O", OBJECT},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}

TEST(SIREN, parse_root_based_any_selection)
{
    std::stringstream input;
    input << R"INPUT( // )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(2, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"A", ANY}, {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_root_based_any_selection_w_child)
{
    std::stringstream input;
    input << R"INPUT( //child_name )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"A", ANY},
        {"child_name", DECL},
        {"A", ANY},
        {"document", DOCUMENT_ROOT}};
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}

TEST(SIREN, parse_relative_based_any_selection)
{
    std::stringstream input;
    // selection from the current context
    input << R"INPUT( something*// )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {"something*", DECL}  // current node
        ,
        {"A", ANY}  // any child
        ,
        {"A", ANY}  // ANY selection context
        ,
        {"document", DOCUMENT_ROOT}  // selection expression root
    };
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}

TEST(SIREN, parse_relative_based_any_selection_w_child)
{
    std::stringstream input;
    // selection from the current context
    input << R"INPUT( .//child_name )INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    //    document.paths(std::cout);
    std::vector<std::pair<std::string, NODE>> expected = {
        {".", DECL}  // current node
        ,
        {"A", ANY}  // any child
        ,
        {"child_name", DECL}  // named 'child_name'
        ,
        {"A", ANY}  // ANY selection context
        ,
        {"document", DOCUMENT_ROOT}  // selection expression root
    };
    ASSERT_EQ(expected.size(), interpreter.node_count());
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(expected[i].first, interpreter.name(i));
            ASSERT_EQ(expected[i].second, interpreter.type(i));
        }
    }
}
TEST(SIREN, parse_numbered_child)
{
    std::stringstream input;
    input << R"INPUT(/1child)INPUT";
    DefaultSIRENInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.child_count());
    auto root_selection = document.child_at(0);
    // when there exists a root selection
    // with a child selection the node type is a declaration
    ASSERT_EQ(DOCUMENT_ROOT, root_selection.type());
    // '/' 'child'
    ASSERT_EQ(2, root_selection.child_count());
    ASSERT_EQ(SEPARATOR, root_selection.child_at(0).type());
    ASSERT_EQ(DECL, root_selection.child_at(1).type());
    std::string decl = "1child";
    ASSERT_EQ(decl, root_selection.child_at(1).name());
}
