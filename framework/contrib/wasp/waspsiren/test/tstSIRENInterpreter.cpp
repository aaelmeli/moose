#include "waspsiren/SIRENInterpreter.h"
#include "waspcore/Interpreter.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace wasp;

TEST(SIREN, selection_on_keyed_value)
{
    DummyInterp<TreeNodePool<>> interp;
    // decl = value
    // key = 3.14
    // document
    // |_keyedvalue
    //   |_ decl (key)
    //   |_ =  (=)
    //   |_ value (3.14)
    //

    auto token_i = interp.token_count();
    interp.push_token("key", wasp::STRING, 0);
    interp.push_leaf(wasp::DECL, "decl", token_i);  // node 0
    token_i = interp.token_count();
    interp.push_token("=", wasp::ASSIGN, 5);
    interp.push_leaf(wasp::ASSIGN, "=", token_i);  // node 1
    token_i = interp.token_count();
    interp.push_token("3.14", wasp::REAL, 7);
    interp.push_leaf(wasp::VALUE, "value", token_i);           // node 2
    interp.push_parent(wasp::KEYED_VALUE, "key", {0, 1, 2});   // node 3
    interp.push_parent(wasp::DOCUMENT_ROOT, "document", {3});  // node 4
    NodeView document(4, interp);
    ASSERT_EQ(1, document.child_count());
    NodeView key = document.child_at(0);
    ASSERT_TRUE(key.has_parent());
    ASSERT_EQ(3, key.child_count());
    {  // select only the root
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string document = "document";
            ASSERT_EQ(document, set.adapted(0).name());
            ASSERT_EQ(DOCUMENT_ROOT, set.adapted(0).type());
        }
    }
    {  // select the key child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/key"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string key = "key";
            ASSERT_EQ(key, set.adapted(0).name());
            ASSERT_EQ(KEYED_VALUE, set.adapted(0).type());
        }
    }
    {  // select the key's value child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/key/value"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string value = "value";
            ASSERT_EQ(value, set.adapted(0).name());
            ASSERT_EQ(VALUE, set.adapted(0).type());
        }
    }
    {  // select the any first level's value child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/*/value"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string value = "value";
            ASSERT_EQ(value, set.adapted(0).name());
            ASSERT_EQ(VALUE, set.adapted(0).type());
        }
    }
    {  // select the key's decl child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // select the key's decl child relative from document
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("  key / decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // select the key's decl child relative from key
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(key, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // test root-based selection with expected no results
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" /nothing/decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(key, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // test relative-based selection with expected no results
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" nothing/decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(key, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // test relative-based selection of parent
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" .. "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(key, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string document = "document";
            ASSERT_EQ(document, set.adapted(0).name());
            ASSERT_EQ(DOCUMENT_ROOT, set.adapted(0).type());
        }
    }
    {  // test relative-based selection of parent with expected no results
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" ../.. "));  // document has no parent
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(key, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's decl where key's value=3.14
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ value = 3.14 ]/ decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // select the key's *l where ke?'s va?ue=3.14
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / ke? [ va?ue = 3.14 ]/ *l "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // select the key's dec where key's value=3.14 (no results)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ value = 3.14 ]/ dec "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's dec where key's val=3.14 (no results)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ val = 3.14 ]/ dec "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's decl where key's value=3.1 (no results)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ value = 3.1 ]/ decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's first decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl[1]"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // select the key's second decl (no second - empty selection)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl[2]"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
}

/**
 * @brief TEST with multiple children under root
 */
TEST(SIREN, selection_on_keyed_values)
{
    DummyInterp<TreeNodePool<>> interp;
    // decl = value
    // 'key = 3.14 key = 3.149 key = 20'
    // document
    // |_keyedvalue
    //   |_ decl (key)
    //   |_ =  (=)
    //   |_ value (3.14)
    //
    // |_keyedvalue
    //   |_ decl (key)
    //   |_ =  (=)
    //   |_ value (3.149)
    //
    // |_keyedvalue
    //   |_ decl (key)
    //   |_ =  (=)
    //   |_ value (20)
    //
    auto token_i = interp.token_count();
    interp.push_token("key", wasp::STRING, 0);
    interp.push_leaf(wasp::DECL, "decl", token_i);  // node 0
    token_i = interp.token_count();
    interp.push_token("=", wasp::ASSIGN, 5);
    interp.push_leaf(wasp::ASSIGN, "=", token_i);  // node 1
    token_i = interp.token_count();
    interp.push_token("3.14", wasp::REAL, 7);
    interp.push_leaf(wasp::VALUE, "value", token_i);          // node 2
    interp.push_parent(wasp::KEYED_VALUE, "key", {0, 1, 2});  // node 3
    // second keyedvalue
    token_i = interp.token_count();
    interp.push_token("key", wasp::STRING, 11);
    interp.push_leaf(wasp::DECL, "decl", token_i);  // node 4
    token_i = interp.token_count();
    interp.push_token("=", wasp::ASSIGN, 16);
    interp.push_leaf(wasp::ASSIGN, "=", token_i);  // node 5
    token_i = interp.token_count();
    interp.push_token("3.149", wasp::REAL, 18);
    interp.push_leaf(wasp::VALUE, "value", token_i);          // node 6
    interp.push_parent(wasp::KEYED_VALUE, "key", {4, 5, 6});  // node 7
    // third keyedvalue
    token_i = interp.token_count();
    interp.push_token("key", wasp::STRING, 22);
    interp.push_leaf(wasp::DECL, "decl", token_i);  // node 8
    token_i = interp.token_count();
    interp.push_token("=", wasp::ASSIGN, 27);
    interp.push_leaf(wasp::ASSIGN, "=", token_i);  // node 9
    token_i = interp.token_count();
    interp.push_token("20", wasp::REAL, 29);
    interp.push_leaf(wasp::VALUE, "value", token_i);  // node 10
    ASSERT_EQ(11, interp.size());
    interp.push_parent(wasp::KEYED_VALUE, "key", {8, 9, 10});         // node 11
    interp.push_parent(wasp::DOCUMENT_ROOT, "document", {3, 7, 11});  // node 12
    NodeView document(12, interp);
    ASSERT_EQ(13, interp.size());
    ASSERT_EQ(3, document.child_count());
    NodeView key = document.child_at(2);
    ASSERT_TRUE(key.has_parent());
    ASSERT_EQ(3, key.child_count());
    ASSERT_EQ(0, strcmp("key", key.name()));
    {  // select only the root
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string document = "document";
            ASSERT_EQ(document, set.adapted(0).name());
            ASSERT_EQ(DOCUMENT_ROOT, set.adapted(0).type());
        }
    }
    {  // select the key child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/key"));
        {
            SIRENResultSet<decltype(document)> set;

            ASSERT_EQ(3, siren.evaluate(document, set));
            ASSERT_EQ(3, set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string key = "key";
                ASSERT_EQ(key, set.adapted(i).name());
                ASSERT_EQ(KEYED_VALUE, set.adapted(i).type());
            }
        }
    }
    {  // select the key's value child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("/key/value"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(3, siren.evaluate(document, set));
            ASSERT_EQ(3, set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string value = "value";
                ASSERT_EQ(value, set.adapted(i).name());
                ASSERT_EQ(VALUE, set.adapted(i).type());
            }
        }
    }
    {  // select the key's decl child
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(3, siren.evaluate(document, set));
            ASSERT_EQ(3, set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
            }
        }
    }
    {  // select the key's decl child relative from document
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString("  key / decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(3, siren.evaluate(document, set));
            ASSERT_EQ(3, set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
            }
        }
    }
    {  // select the key's decl child relative from key
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(key, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // test root-based selection with expected no results
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" /nothing/decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(key, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // test relative-based selection with expected no results
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" nothing/decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(key, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // test relative-based selection of parent
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" .. "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(key, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string document = "document";
            ASSERT_EQ(document, set.adapted(0).name());
            ASSERT_EQ(DOCUMENT_ROOT, set.adapted(0).type());
        }
    }
    {  // test relative-based selection of parent with expected no results
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" ../.. "));  // document has no parent
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(key, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's decl where key's value=3.149
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ value = 3.149 ]/ decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
        }
    }
    {  // select the key's dec where key's value=3.14 (no results)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ value = 3.14 ]/ dec "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's dec where key's val=3.14 (no results)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ val = 3.14 ]/ dec "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's decl where key's value=3.1 (no results)
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key [ value = 3.1 ]/ decl "));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(0, siren.evaluate(document, set));
            ASSERT_EQ(0, set.result_count());
        }
    }
    {  // select the key's first decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl[1]"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
            ASSERT_EQ(0, set.adapted(0).node_index());
        }
    }
    {  // select the key's second decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl[2]"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
            ASSERT_EQ(4, set.adapted(0).node_index());
        }
    }
    {  // select the key's third decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key / decl[3]"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            ASSERT_TRUE(set.is_adapted(0));
            std::string decl = "decl";
            ASSERT_EQ(decl, set.adapted(0).name());
            ASSERT_EQ(DECL, set.adapted(0).type());
            ASSERT_EQ(8, set.adapted(0).node_index());
        }
    }
    {  // select the first and second key's decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key[1:2] / decl"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(2, siren.evaluate(document, set));
            ASSERT_EQ(2, set.result_count());
            std::vector<int> tree_node = {0, 4};
            ASSERT_EQ(tree_node.size(), set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
                EXPECT_EQ(tree_node[i], set.adapted(i).node_index());
            }
        }
    }
    {  // select the second and third key's decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key[2:3] / decl"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(2, siren.evaluate(document, set));
            ASSERT_EQ(2, set.result_count());
            std::vector<int> tree_node = {4, 8};
            ASSERT_EQ(tree_node.size(), set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
                EXPECT_EQ(tree_node[i], set.adapted(i).node_index());
            }
        }
    }
    {  // select the all 3 key's decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key[0:5] / decl"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(3, siren.evaluate(document, set));
            ASSERT_EQ(3, set.result_count());
            std::vector<int> tree_node = {0, 4, 8};
            ASSERT_EQ(tree_node.size(), set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
                EXPECT_EQ(tree_node[i], set.adapted(i).node_index());
            }
        }
    }
    {  // select the first and third key's decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key[1:3:2] / decl"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(2, siren.evaluate(document, set));
            ASSERT_EQ(2, set.result_count());
            std::vector<int> tree_node = {0, 8};
            ASSERT_EQ(tree_node.size(), set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
                EXPECT_EQ(tree_node[i], set.adapted(i).node_index());
            }
        }
    }
    {  // select the first and third key's decl
        DefaultSIRENInterpreter siren;
        ASSERT_TRUE(siren.parseString(" / key[3:6:3] / decl"));
        {
            SIRENResultSet<decltype(document)> set;
            ASSERT_EQ(1, siren.evaluate(document, set));
            ASSERT_EQ(1, set.result_count());
            std::vector<int> tree_node = {8};
            ASSERT_EQ(tree_node.size(), set.result_count());
            for (size_t i = 0; i < set.result_count(); ++i)
            {
                SCOPED_TRACE(i);
                ASSERT_TRUE(set.is_adapted(i));
                std::string decl = "decl";
                ASSERT_EQ(decl, set.adapted(i).name());
                ASSERT_EQ(DECL, set.adapted(i).type());
                EXPECT_EQ(tree_node[i], set.adapted(i).node_index());
            }
        }
    }
}
