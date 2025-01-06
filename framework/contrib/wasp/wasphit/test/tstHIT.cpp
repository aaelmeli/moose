#include "wasphit/HITInterpreter.h"
#include "wasphit/HITNodeView.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
using namespace wasp;

TEST(HITInterpreter, bad)

{
    std::stringstream input;
    input << R"INPUT([GlobalParams]
disp_x = disp_x
disp_y = disp_y
disp_z = disp_z
[../])INPUT";
    std::stringstream        error;
    DefaultHITInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream tree_print;
    auto              root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    root_view.paths(tree_print);
    std::stringstream expected_tree;
    expected_tree <<
        R"INPUT(/
/GlobalParams
/GlobalParams/[ ([)
/GlobalParams/decl (GlobalParams)
/GlobalParams/] (])
/GlobalParams/disp_x
/GlobalParams/disp_x/decl (disp_x)
/GlobalParams/disp_x/= (=)
/GlobalParams/disp_x/value (disp_x)
/GlobalParams/disp_y
/GlobalParams/disp_y/decl (disp_y)
/GlobalParams/disp_y/= (=)
/GlobalParams/disp_y/value (disp_y)
/GlobalParams/disp_z
/GlobalParams/disp_z/decl (disp_z)
/GlobalParams/disp_z/= (=)
/GlobalParams/disp_z/value (disp_z)
/GlobalParams/term ([../])
)INPUT";
    ASSERT_EQ(expected_tree.str(), tree_print.str());
}

TEST(HITInterpreter, simple)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_key
    //    |_ decl 'key'
    //    |_ = '='
    //    |_ value '3.421'
    input << "key =  3.421";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());

    std::vector<std::string> token_data      = {"key", "=", "3.421"};
    std::vector<std::string> leaf_node_names = {"decl", "=", "value"};
    std::vector<wasp::NODE>  leaf_node_types = {wasp::DECL, wasp::ASSIGN,
                                               wasp::VALUE};
    for (std::size_t i = 0; i < 3; ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(token_data[i], interpreter.data(i));
            ASSERT_EQ(leaf_node_types[i], interpreter.type(i));
            ASSERT_EQ(leaf_node_names[i], interpreter.name(i));
        }
    }
    std::vector<std::size_t> parent_indices = {3, 3, 3, 4, 5};
    std::vector<std::size_t> child_count    = {0, 0, 0, 3, 1};
    std::vector<std::size_t> column         = {1, 5, 8, 1, 1};
    std::vector<wasp::NODE> node_types = {wasp::DECL, wasp::ASSIGN, wasp::VALUE,
                                          wasp::KEYED_VALUE,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"decl", "=", "value", "key",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    ASSERT_EQ(node_types.size(), parent_indices.size());
    ASSERT_EQ(node_types.size(), child_count.size());
    ASSERT_EQ(node_types.size(), column.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
            ASSERT_EQ(parent_indices[i], interpreter.parent_node_index(i));
            ASSERT_EQ(child_count[i], interpreter.child_count(i));
            ASSERT_EQ(child_count[i], interpreter.child_count(i));
            ASSERT_EQ(column[i], interpreter.column(i));
            ASSERT_EQ(1, interpreter.line(i));
        }
    }
}

TEST(HITInterpreter, empty_object)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_ object_term '[]'

    input << "[ted][]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(6, interpreter.node_count());

    std::vector<std::string> token_data      = {"[", "ted", "]"};
    std::vector<std::string> leaf_node_names = {"[", "decl", "]"};
    std::vector<wasp::NODE>  leaf_node_types = {wasp::LBRACKET, wasp::DECL,
                                               wasp::RBRACKET};
    for (std::size_t i = 0; i < 3; ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(token_data[i], interpreter.data(i));
            ASSERT_EQ(leaf_node_types[i], interpreter.type(i));
            ASSERT_EQ(leaf_node_names[i], interpreter.name(i));
        }
    }
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET, wasp::DECL,
                                          wasp::RBRACKET, wasp::OBJECT_TERM,
                                          wasp::OBJECT,   wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",  "decl", "]",
                                           "term", "ted",  "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, simple_object)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_key
    //      |_ decl 'key'
    //      |_ = '='
    //      |_ value '3.421'
    //    |_ object_term '[]'

    input << "[ted]key = 3.421[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(10, interpreter.node_count());

    // object decl
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // object term
                                          ,
                                          wasp::OBJECT_TERM
                                          // object
                                          ,
                                          wasp::OBJECT
                                          // document root
                                          ,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {
        "[", "decl", "]", "decl", "=", "value", "key", "term", "ted", "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, less_simple_object)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_boo
    //      |_ decl 'boo'
    //      |_ = '='
    //      |_ value 'foo'
    //    |_comment
    //    |_fred
    //      |_ decl 'fred'
    //      |_ = '='
    //      |_ value '1'
    //    |_key
    //      |_ decl 'key'
    //      |_ = '='
    //      |_ value '3.421'
    //    |_ object_term '[]'

    input << R"INPUT([ted]
     boo = foo # halloween
     fred = 1
     key = 3.421
 [])INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(19, interpreter.node_count());

    // object decl
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value boo
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // comment
                                          ,
                                          wasp::COMMENT
                                          // keyed value fred
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // keyed value key
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // object term
                                          ,
                                          wasp::OBJECT_TERM
                                          // object
                                          ,
                                          wasp::OBJECT
                                          // document root
                                          ,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "decl",
                                           "=",
                                           "value"  // boo
                                           ,
                                           "boo",
                                           "comment"  // halloween
                                           ,
                                           "decl",
                                           "=",
                                           "value"  // fred
                                           ,
                                           "fred",
                                           "decl",
                                           "=",
                                           "value"  // key
                                           ,
                                           "key",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    NodeView fred;
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
            if (std::strcmp("fred", interpreter.name(i)) == 0) fred = interpreter.node_at(i);
        }
    }
    NodeView fred_value = fred.first_child_by_name("value");
    ASSERT_EQ("1", fred_value.data());
    fred_value.set_data("1\n2\n3\n1\n2\n3\n1\n2\n3\n1\n2\n3");
    ASSERT_EQ("fred = 1\n2\n3\n1\n2\n3\n1\n2\n3\n1\n2\n3", fred.data());
    // Notice that the additional byte offset introduced by the longer token data
    // absorbes the whitespace prefix on the following line 'key'
    std::string expected_value = R"INPUT([ted]
     boo = foo # halloween
     fred = 1
2
3
1
2
3
1
2
3
1
2
3 key = 3.421
 [])INPUT";
    ASSERT_EQ(expected_value.data(), interpreter.root().data());
}

TEST(HITInterpreter, object_array)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_array 'data'
    //      |_ decl 'data'
    //      |_ = '='
    //      |_ ' '''
    //      |_ value 'basic'
    //      |_ value '201'
    //      |_ value 'lu'
    //      |_ ' '''
    //    |_ object_term '[]'

    input << "[ted]data='basic 201 lu'[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(14, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value array data
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::QUOTE,
                                          wasp::VALUE,
                                          wasp::VALUE,
                                          wasp::VALUE,
                                          wasp::QUOTE,
                                          wasp::ARRAY,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "decl",
                                           "="  // data array
                                           ,
                                           "'"  // start
                                           ,
                                           "value"  // basic
                                           ,
                                           "value"  // 201
                                           ,
                                           "value"  // lu
                                           ,
                                           "'"  // terminator
                                           ,
                                           "data",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, double_quoted_value)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_key
    //      |_ decl 'key'
    //      |_ = '='
    //      |_ value '"quotedvalue"'
    //    |_ object_term '[]'

    input << "[ted]key = \"quotedvalue\"[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(10, interpreter.node_count());

    // object decl
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // object term
                                          ,
                                          wasp::OBJECT_TERM
                                          // object
                                          ,
                                          wasp::OBJECT
                                          // document root
                                          ,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {
        "[", "decl", "]", "decl", "=", "value", "key", "term", "ted", "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, object_array_semicolons)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_array 'data'
    //      |_ decl 'data'
    //      |_ = '='
    //      |_ ' '''
    //      |_ value basic
    //      |_ , semicolon
    //      |_ value 201
    //      |_ , semicolon
    //      |_ value again
    //      |_ , semicolon
    //      |_ value here
    //      |_ , semicolon
    //      |_ value close
    //      |_ , semicolon
    //      |_ value far
    //      |_ value lu
    //      |_ ' '''
    //    |_ object_term '[]'

    input << "[ted]data='basic;201 ; again;here;close ; far lu'[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(23, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value array data
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::QUOTE,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::VALUE,
                                          wasp::QUOTE,
                                          wasp::ARRAY,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "decl",
                                           "="  // data array
                                           ,
                                           "'"  // start
                                           ,
                                           "value"  // basic
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // 201
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // again
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // here
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // close
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // far
                                           ,
                                           "value"  // lu
                                           ,
                                           "'"  // terminator
                                           ,
                                           "data",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, object_empty_subobject)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //      |_ sub_object 'fred'
    //        |_ sub_object_decl 'fred'
    //          |_ [ '['
    //          |_ ./ './'
    //          |_ string 'fred'
    //          |_ ] ']'
    //        |_ object_term '[../]'
    //    |_ object_term '[]'

    input << "[ted][./fred][../][]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(12, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // sub object
                                          ,
                                          wasp::LBRACKET,
                                          wasp::DOT_SLASH,
                                          wasp::DECL,
                                          wasp::RBRACKET,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",    "decl", "]",   "[",
                                           "./",   "decl", "]",   "term",
                                           "fred", "term",   "ted", "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, object_subobject)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //      |_ sub_object 'fred'
    //        |_ sub_object_decl 'fred'
    //          |_ [ '['
    //          |_ ./ './'
    //          |_ string 'fred'
    //          |_ ] ']'
    //          |_key
    //            |_ decl 'key'
    //            |_ = '='
    //            |_ value '3.421'
    //              |_ object_term '[../]'
    //    |_ object_term '[]'

    input << "[ted][./fred]key=3.4321[../][]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // sub object
                                          ,
                                          wasp::LBRACKET,
                                          wasp::DOT_SLASH,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value key
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "[",
                                           "./",
                                           "decl",
                                           "]",
                                           "decl",
                                           "=",
                                           "value"  // key
                                           ,
                                           "key",
                                           "term",
                                           "fred",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, simple_view)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_key
    //    |_ decl 'key'
    //    |_ = '='
    //    |_ value '3.421'
    input << "key =  3.421";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::DECL, wasp::ASSIGN, wasp::VALUE,
                                          wasp::KEYED_VALUE,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"decl", "=", "value", "key",
                                           "document"};
    std::vector<std::string> node_paths = {"/key/decl", "/key/=", "/key/value",
                                           "/key", "/"};
    std::vector<std::size_t> node_columns = {1, 5, 8, 1, 1};
    ASSERT_EQ(node_types.size(), node_names.size());
    ASSERT_EQ(node_types.size(), node_columns.size());
    ASSERT_EQ(node_types.size(), node_paths.size());
    auto document = interpreter.root();
    // the root of the document
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(node_names.back(), document.name());
    ASSERT_EQ(wasp::DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.line());
    ASSERT_EQ(1, document.column());
    ASSERT_TRUE(document.equal(document));
    ASSERT_TRUE(false == document.has_parent());
    ASSERT_EQ(node_paths.back(), document.path());
    auto key = document.child_at(0);
    ASSERT_EQ(3, key.child_count());
    ASSERT_TRUE(key.has_parent());
    ASSERT_TRUE(document.equal(key.parent()));
    ASSERT_EQ(wasp::KEYED_VALUE, key.type());
    ASSERT_EQ(node_names[3], key.name());
    ASSERT_EQ(1, key.line());
    ASSERT_EQ(1, key.column());
    ASSERT_FALSE(document.equal(key));
    ASSERT_EQ(node_paths[3], key.path());
    // TODO add test on data (type, line, col, etc).
    for (std::size_t i = 0, child_count = key.child_count(); i < child_count;
         ++i)
    {
        {
            SCOPED_TRACE(i);
            auto child = key.child_at(i);
            ASSERT_EQ(node_types[i], child.type());
            ASSERT_EQ(0, child.child_count());
            ASSERT_EQ(node_types[i], child.type());
            ASSERT_EQ(node_names[i], child.name());
            ASSERT_EQ(1, child.line());
            ASSERT_EQ(node_columns[i], child.column());
            ASSERT_TRUE(child.has_parent());
            ASSERT_TRUE(key.equal(child.parent()));
            ASSERT_EQ(node_paths[i], child.path());
        }
    }
    std::string expected_paths =
        "/\n/key\n/key/decl (key)\n/key/= (=)\n/key/value (3.421)\n";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}

TEST(HITInterpreter, paths)
{
    std::stringstream input;
    input << R"INPUT([Problem]
  coord_type = RZ
  [./child][../]
[])INPUT";

    std::string expected_paths =
        R"INPUT(/
/Problem
/Problem/[ ([)
/Problem/decl (Problem)
/Problem/] (])
/Problem/coord_type
/Problem/coord_type/decl (coord_type)
/Problem/coord_type/= (=)
/Problem/coord_type/value (RZ)
/Problem/child
/Problem/child/[ ([)
/Problem/child/./ (./)
/Problem/child/decl (child)
/Problem/child/] (])
/Problem/child/term ([../])
/Problem/term ([])
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}

/**
 * @brief TEST hit doesn't support expressions natively
 * It just grabs them as strings
 */
TEST(HITInterpreter, expression)
{
    std::stringstream input;
    input << R"INPUT(
function = 'A*c^2*(1-c)^2+B*(c^2+6*(1-c),(gr0^2+gr1^2+gr2^2+gr3^2)
             -4*(2-c)*(gr0^3+gr1^3+gr2^3+gr3^3)
             +3*(gr0^2+gr1^2,gr2^2+gr3^2)^2)'
)INPUT";
    std::vector<std::string> data = {
        "function",
        "=",
        "'",
        "A*c^2*(1-c)^2+B*(c^2+6*(1-c),(gr0^2+gr1^2+gr2^2+gr3^2)",
        "-4*(2-c)*(gr0^3+gr1^3+gr2^3+gr3^3)",
        "+3*(gr0^2+gr1^2,gr2^2+gr3^2)^2)",
        "'"};

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(9, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto fnode = document.child_at(0);
    ASSERT_EQ(data.size(), fnode.child_count());
    for (std::size_t i = 0; i < fnode.child_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(data[i], fnode.child_at(i).data());
        }
    }
}
/**
 *  @brief TEST comments that are empty and with content
 */
TEST(HITInterpreter, comments)
{
    std::stringstream input;
    input << R"INPUT(#
# comment with content
#)INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::vector<std::string> data = {"#", "# comment with content", "#"};
    ASSERT_EQ(data.size(), document.child_count());
    for (std::size_t i = 0; i < document.child_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(data[i], document.child_at(i).data());
        }
    }
}

/**
 * @brief TEST nested subblocks (subblocks within subblocks)
 */
TEST(HITInterpreter, nested_subblocks)
{
    std::stringstream input;
    input << R"INPUT([block]
  [./subblock]
    [./nested_subblock]
    [../]
  [../]
[])INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(18, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto block = document.child_at(0);
    ASSERT_EQ("block", std::string(block.name()));
    ASSERT_EQ(wasp::OBJECT, block.type());
    ASSERT_EQ(5, block.child_count());
    auto subblock = block.child_at(3);
    ASSERT_EQ("subblock", std::string(subblock.name()));
    ASSERT_EQ(wasp::OBJECT, subblock.type());
    ASSERT_EQ(6, subblock.child_count());
    auto nestedsubblock = subblock.child_at(4);
    ASSERT_EQ("nested_subblock", std::string(nestedsubblock.name()));
    ASSERT_EQ(wasp::OBJECT, nestedsubblock.type());
    ASSERT_EQ(3, nestedsubblock.line());
    ASSERT_EQ(5, nestedsubblock.column());
}

TEST(HITInterpreter, multiple_objects)
{
    std::stringstream input;
    input << R"INPUT([Problem]
    # Specify coordinate system type
    coord_type = RZ
[]

[Mesh]
    file = pelletcladmergedfine10_rz.e
    displacements = 'disp_x disp_y'
    patch_size = 1000 # For contact algorithm
[]
[1]
    some = thing
    [./2]
        another=value
        sliceheight = ${/ ${fuelheight} ${numslices}}
    [../]
[])INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(55, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());  // problem and mesh
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));
    std::string       expected_paths = R"INPUT(/
/Problem
/Problem/[ ([)
/Problem/decl (Problem)
/Problem/] (])
/Problem/comment (# Specify coordinate system type)
/Problem/coord_type
/Problem/coord_type/decl (coord_type)
/Problem/coord_type/= (=)
/Problem/coord_type/value (RZ)
/Problem/term ([])
/Mesh
/Mesh/[ ([)
/Mesh/decl (Mesh)
/Mesh/] (])
/Mesh/file
/Mesh/file/decl (file)
/Mesh/file/= (=)
/Mesh/file/value (pelletcladmergedfine10_rz.e)
/Mesh/displacements
/Mesh/displacements/decl (displacements)
/Mesh/displacements/= (=)
/Mesh/displacements/' (')
/Mesh/displacements/value (disp_x)
/Mesh/displacements/value (disp_y)
/Mesh/displacements/' (')
/Mesh/patch_size
/Mesh/patch_size/decl (patch_size)
/Mesh/patch_size/= (=)
/Mesh/patch_size/value (1000)
/Mesh/comment (# For contact algorithm)
/Mesh/term ([])
/1
/1/[ ([)
/1/decl (1)
/1/] (])
/1/some
/1/some/decl (some)
/1/some/= (=)
/1/some/value (thing)
/1/2
/1/2/[ ([)
/1/2/./ (./)
/1/2/decl (2)
/1/2/] (])
/1/2/another
/1/2/another/decl (another)
/1/2/another/= (=)
/1/2/another/value (value)
/1/2/sliceheight
/1/2/sliceheight/decl (sliceheight)
/1/2/sliceheight/= (=)
/1/2/sliceheight/value (${/ ${fuelheight} ${numslices}})
/1/2/term ([../])
/1/term ([])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    auto mesh_view = document.first_child_by_name("Mesh");
    ASSERT_FALSE(mesh_view.is_null());
    ASSERT_FALSE(mesh_view.is_decorative());
    ASSERT_EQ(wasp::OBJECT, mesh_view.type());
    ASSERT_EQ(8, mesh_view.child_count());
    ASSERT_EQ(3, mesh_view.non_decorative_children_count());
    auto disp_view = mesh_view.first_child_by_name("displacements");
    ASSERT_FALSE(disp_view.is_null());
    ASSERT_EQ(wasp::ARRAY, disp_view.type());
    ASSERT_EQ(2, disp_view.non_decorative_children_count());
    auto values = disp_view.non_decorative_children();
    ASSERT_EQ(2, values.size());
    for (auto v : values)
    {
        ASSERT_EQ("value", std::string(v.name()));
        ASSERT_EQ("disp", v.to_string().substr(0, 4));
    }
}

/**
 * @brief TEST hit type promotion
 * When a block/subblock has a type field,
 * the block/subblock name becomes the type field's value
 * with "_type" appended.
 *
 * Test each scenario for block and subblock
 * 1. block with only type = value
 * 2. block with type = value and other stuff
 * 3. subblock with only type = value
 * 4. subblock with type = value and other stuff
 * 5. subblock with type = value
 * 6. block with type = value
 */
TEST(HITInterpreter, type_promotion)
{
    std::stringstream input;
    input << R"INPUT([Problem]
    # Specify coordinate system type
    type = ted
    [./s1] type = fred [../]
    [./s2] type = x  y=1 [../]
[]
[p1]
    type = t1
[]
[p2]
   type = t2
    [./s3]
        type = us3
    [../]
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(63, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());  // problem and mesh
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    std::string       expected_paths = R"INPUT(/
/ted_type
/ted_type/[ ([)
/ted_type/decl (Problem)
/ted_type/] (])
/ted_type/comment (# Specify coordinate system type)
/ted_type/type
/ted_type/type/decl (type)
/ted_type/type/= (=)
/ted_type/type/value (ted)
/ted_type/fred_type
/ted_type/fred_type/[ ([)
/ted_type/fred_type/./ (./)
/ted_type/fred_type/decl (s1)
/ted_type/fred_type/] (])
/ted_type/fred_type/type
/ted_type/fred_type/type/decl (type)
/ted_type/fred_type/type/= (=)
/ted_type/fred_type/type/value (fred)
/ted_type/fred_type/term ([../])
/ted_type/x_type
/ted_type/x_type/[ ([)
/ted_type/x_type/./ (./)
/ted_type/x_type/decl (s2)
/ted_type/x_type/] (])
/ted_type/x_type/type
/ted_type/x_type/type/decl (type)
/ted_type/x_type/type/= (=)
/ted_type/x_type/type/value (x)
/ted_type/x_type/y
/ted_type/x_type/y/decl (y)
/ted_type/x_type/y/= (=)
/ted_type/x_type/y/value (1)
/ted_type/x_type/term ([../])
/ted_type/term ([])
/t1_type
/t1_type/[ ([)
/t1_type/decl (p1)
/t1_type/] (])
/t1_type/type
/t1_type/type/decl (type)
/t1_type/type/= (=)
/t1_type/type/value (t1)
/t1_type/term ([])
/t2_type
/t2_type/[ ([)
/t2_type/decl (p2)
/t2_type/] (])
/t2_type/type
/t2_type/type/decl (type)
/t2_type/type/= (=)
/t2_type/type/value (t2)
/t2_type/us3_type
/t2_type/us3_type/[ ([)
/t2_type/us3_type/./ (./)
/t2_type/us3_type/decl (s3)
/t2_type/us3_type/] (])
/t2_type/us3_type/type
/t2_type/us3_type/type/decl (type)
/t2_type/us3_type/type/= (=)
/t2_type/us3_type/type/value (us3)
/t2_type/us3_type/term ([../])
/t2_type/term ([])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    auto ted_view = document.first_child_by_name("ted_type");
    ASSERT_FALSE(ted_view.is_null());
    ASSERT_FALSE(ted_view.is_decorative());
    ASSERT_EQ(wasp::OBJECT, ted_view.type());
    ASSERT_EQ(8, ted_view.child_count());
    // ted/type and ted/fred
    ASSERT_EQ(3, ted_view.non_decorative_children_count());
}

/**
 * @brief Test HIT syntax
 *
 * MOOSE's new HIT parser relaxes the declarator and terminator syntax
 * for blocks and subblocks and all blocks are considered the same type
 *
 * Blocks and subblocks may be started with:
 *  - Block declarator syntax    ( i.e. "[blockname]")
 *  - Subblock declarator syntax ( i.e. "[./blockname]")
 *
 * Blocks and subblocks may be terminated with:
 *  - Block terminator syntax    ( i.e. "[]")
 *  - Subblock terminator syntax ( i.e. "[../]")
 */
TEST(HITInterpreter, hit_syntax)
{
    std::stringstream input;
    input << R"INPUT(
[TopLevelBlock]

  [./sub-decl-sub-term-no-type]
    flag01  = 1.1
    array01 = 'name11 name12 name13'
  [../]

  [./sub-decl-top-term-no-type]
    flag02  = 2.2
    array02 = 'name21 name22 name23'
  []

  [top-decl-sub-term-no-type]
    flag03  = 3.3
    array03 = 'name31 name32 name33'
  [../]

  [top-decl-top-term-no-type]
    flag04  = 4.4
    array04 = 'name41 name42 name43'
  []

  [./sub-decl-sub-term-wt-type]
    type    = SubDeclSubTerm
    flag05  = 5.5
    array05 = 'name51 name42 name53'
  [../]

  [./sub-decl-top-term-wt-type]
    type    = SubDeclTopTerm
    flag06  = 6.6
    array06 = 'name61 name62 name63'
  []

  [top-decl-sub-term-wt-type]
    type    = TopDeclSubTerm
    flag07  = 7.7
    array07 = 'name71 name72 name73'
  [../]

  [top-decl-top-term-wt-type]
    type    = TopDeclTopTerm
    flag08  = 8.8
    array08 = 'name81 name82 name83'
  []

[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(162, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/TopLevelBlock
/TopLevelBlock/[ ([)
/TopLevelBlock/decl (TopLevelBlock)
/TopLevelBlock/] (])
/TopLevelBlock/sub-decl-sub-term-no-type
/TopLevelBlock/sub-decl-sub-term-no-type/[ ([)
/TopLevelBlock/sub-decl-sub-term-no-type/./ (./)
/TopLevelBlock/sub-decl-sub-term-no-type/decl (sub-decl-sub-term-no-type)
/TopLevelBlock/sub-decl-sub-term-no-type/] (])
/TopLevelBlock/sub-decl-sub-term-no-type/flag01
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/decl (flag01)
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/= (=)
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/value (1.1)
/TopLevelBlock/sub-decl-sub-term-no-type/array01
/TopLevelBlock/sub-decl-sub-term-no-type/array01/decl (array01)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/= (=)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/' (')
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name11)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name12)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name13)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/' (')
/TopLevelBlock/sub-decl-sub-term-no-type/term ([../])
/TopLevelBlock/sub-decl-top-term-no-type
/TopLevelBlock/sub-decl-top-term-no-type/[ ([)
/TopLevelBlock/sub-decl-top-term-no-type/./ (./)
/TopLevelBlock/sub-decl-top-term-no-type/decl (sub-decl-top-term-no-type)
/TopLevelBlock/sub-decl-top-term-no-type/] (])
/TopLevelBlock/sub-decl-top-term-no-type/flag02
/TopLevelBlock/sub-decl-top-term-no-type/flag02/decl (flag02)
/TopLevelBlock/sub-decl-top-term-no-type/flag02/= (=)
/TopLevelBlock/sub-decl-top-term-no-type/flag02/value (2.2)
/TopLevelBlock/sub-decl-top-term-no-type/array02
/TopLevelBlock/sub-decl-top-term-no-type/array02/decl (array02)
/TopLevelBlock/sub-decl-top-term-no-type/array02/= (=)
/TopLevelBlock/sub-decl-top-term-no-type/array02/' (')
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name21)
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name22)
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name23)
/TopLevelBlock/sub-decl-top-term-no-type/array02/' (')
/TopLevelBlock/sub-decl-top-term-no-type/term ([])
/TopLevelBlock/top-decl-sub-term-no-type
/TopLevelBlock/top-decl-sub-term-no-type/[ ([)
/TopLevelBlock/top-decl-sub-term-no-type/decl (top-decl-sub-term-no-type)
/TopLevelBlock/top-decl-sub-term-no-type/] (])
/TopLevelBlock/top-decl-sub-term-no-type/flag03
/TopLevelBlock/top-decl-sub-term-no-type/flag03/decl (flag03)
/TopLevelBlock/top-decl-sub-term-no-type/flag03/= (=)
/TopLevelBlock/top-decl-sub-term-no-type/flag03/value (3.3)
/TopLevelBlock/top-decl-sub-term-no-type/array03
/TopLevelBlock/top-decl-sub-term-no-type/array03/decl (array03)
/TopLevelBlock/top-decl-sub-term-no-type/array03/= (=)
/TopLevelBlock/top-decl-sub-term-no-type/array03/' (')
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name31)
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name32)
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name33)
/TopLevelBlock/top-decl-sub-term-no-type/array03/' (')
/TopLevelBlock/top-decl-sub-term-no-type/term ([../])
/TopLevelBlock/top-decl-top-term-no-type
/TopLevelBlock/top-decl-top-term-no-type/[ ([)
/TopLevelBlock/top-decl-top-term-no-type/decl (top-decl-top-term-no-type)
/TopLevelBlock/top-decl-top-term-no-type/] (])
/TopLevelBlock/top-decl-top-term-no-type/flag04
/TopLevelBlock/top-decl-top-term-no-type/flag04/decl (flag04)
/TopLevelBlock/top-decl-top-term-no-type/flag04/= (=)
/TopLevelBlock/top-decl-top-term-no-type/flag04/value (4.4)
/TopLevelBlock/top-decl-top-term-no-type/array04
/TopLevelBlock/top-decl-top-term-no-type/array04/decl (array04)
/TopLevelBlock/top-decl-top-term-no-type/array04/= (=)
/TopLevelBlock/top-decl-top-term-no-type/array04/' (')
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name41)
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name42)
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name43)
/TopLevelBlock/top-decl-top-term-no-type/array04/' (')
/TopLevelBlock/top-decl-top-term-no-type/term ([])
/TopLevelBlock/SubDeclSubTerm_type
/TopLevelBlock/SubDeclSubTerm_type/[ ([)
/TopLevelBlock/SubDeclSubTerm_type/./ (./)
/TopLevelBlock/SubDeclSubTerm_type/decl (sub-decl-sub-term-wt-type)
/TopLevelBlock/SubDeclSubTerm_type/] (])
/TopLevelBlock/SubDeclSubTerm_type/type
/TopLevelBlock/SubDeclSubTerm_type/type/decl (type)
/TopLevelBlock/SubDeclSubTerm_type/type/= (=)
/TopLevelBlock/SubDeclSubTerm_type/type/value (SubDeclSubTerm)
/TopLevelBlock/SubDeclSubTerm_type/flag05
/TopLevelBlock/SubDeclSubTerm_type/flag05/decl (flag05)
/TopLevelBlock/SubDeclSubTerm_type/flag05/= (=)
/TopLevelBlock/SubDeclSubTerm_type/flag05/value (5.5)
/TopLevelBlock/SubDeclSubTerm_type/array05
/TopLevelBlock/SubDeclSubTerm_type/array05/decl (array05)
/TopLevelBlock/SubDeclSubTerm_type/array05/= (=)
/TopLevelBlock/SubDeclSubTerm_type/array05/' (')
/TopLevelBlock/SubDeclSubTerm_type/array05/value (name51)
/TopLevelBlock/SubDeclSubTerm_type/array05/value (name42)
/TopLevelBlock/SubDeclSubTerm_type/array05/value (name53)
/TopLevelBlock/SubDeclSubTerm_type/array05/' (')
/TopLevelBlock/SubDeclSubTerm_type/term ([../])
/TopLevelBlock/SubDeclTopTerm_type
/TopLevelBlock/SubDeclTopTerm_type/[ ([)
/TopLevelBlock/SubDeclTopTerm_type/./ (./)
/TopLevelBlock/SubDeclTopTerm_type/decl (sub-decl-top-term-wt-type)
/TopLevelBlock/SubDeclTopTerm_type/] (])
/TopLevelBlock/SubDeclTopTerm_type/type
/TopLevelBlock/SubDeclTopTerm_type/type/decl (type)
/TopLevelBlock/SubDeclTopTerm_type/type/= (=)
/TopLevelBlock/SubDeclTopTerm_type/type/value (SubDeclTopTerm)
/TopLevelBlock/SubDeclTopTerm_type/flag06
/TopLevelBlock/SubDeclTopTerm_type/flag06/decl (flag06)
/TopLevelBlock/SubDeclTopTerm_type/flag06/= (=)
/TopLevelBlock/SubDeclTopTerm_type/flag06/value (6.6)
/TopLevelBlock/SubDeclTopTerm_type/array06
/TopLevelBlock/SubDeclTopTerm_type/array06/decl (array06)
/TopLevelBlock/SubDeclTopTerm_type/array06/= (=)
/TopLevelBlock/SubDeclTopTerm_type/array06/' (')
/TopLevelBlock/SubDeclTopTerm_type/array06/value (name61)
/TopLevelBlock/SubDeclTopTerm_type/array06/value (name62)
/TopLevelBlock/SubDeclTopTerm_type/array06/value (name63)
/TopLevelBlock/SubDeclTopTerm_type/array06/' (')
/TopLevelBlock/SubDeclTopTerm_type/term ([])
/TopLevelBlock/TopDeclSubTerm_type
/TopLevelBlock/TopDeclSubTerm_type/[ ([)
/TopLevelBlock/TopDeclSubTerm_type/decl (top-decl-sub-term-wt-type)
/TopLevelBlock/TopDeclSubTerm_type/] (])
/TopLevelBlock/TopDeclSubTerm_type/type
/TopLevelBlock/TopDeclSubTerm_type/type/decl (type)
/TopLevelBlock/TopDeclSubTerm_type/type/= (=)
/TopLevelBlock/TopDeclSubTerm_type/type/value (TopDeclSubTerm)
/TopLevelBlock/TopDeclSubTerm_type/flag07
/TopLevelBlock/TopDeclSubTerm_type/flag07/decl (flag07)
/TopLevelBlock/TopDeclSubTerm_type/flag07/= (=)
/TopLevelBlock/TopDeclSubTerm_type/flag07/value (7.7)
/TopLevelBlock/TopDeclSubTerm_type/array07
/TopLevelBlock/TopDeclSubTerm_type/array07/decl (array07)
/TopLevelBlock/TopDeclSubTerm_type/array07/= (=)
/TopLevelBlock/TopDeclSubTerm_type/array07/' (')
/TopLevelBlock/TopDeclSubTerm_type/array07/value (name71)
/TopLevelBlock/TopDeclSubTerm_type/array07/value (name72)
/TopLevelBlock/TopDeclSubTerm_type/array07/value (name73)
/TopLevelBlock/TopDeclSubTerm_type/array07/' (')
/TopLevelBlock/TopDeclSubTerm_type/term ([../])
/TopLevelBlock/TopDeclTopTerm_type
/TopLevelBlock/TopDeclTopTerm_type/[ ([)
/TopLevelBlock/TopDeclTopTerm_type/decl (top-decl-top-term-wt-type)
/TopLevelBlock/TopDeclTopTerm_type/] (])
/TopLevelBlock/TopDeclTopTerm_type/type
/TopLevelBlock/TopDeclTopTerm_type/type/decl (type)
/TopLevelBlock/TopDeclTopTerm_type/type/= (=)
/TopLevelBlock/TopDeclTopTerm_type/type/value (TopDeclTopTerm)
/TopLevelBlock/TopDeclTopTerm_type/flag08
/TopLevelBlock/TopDeclTopTerm_type/flag08/decl (flag08)
/TopLevelBlock/TopDeclTopTerm_type/flag08/= (=)
/TopLevelBlock/TopDeclTopTerm_type/flag08/value (8.8)
/TopLevelBlock/TopDeclTopTerm_type/array08
/TopLevelBlock/TopDeclTopTerm_type/array08/decl (array08)
/TopLevelBlock/TopDeclTopTerm_type/array08/= (=)
/TopLevelBlock/TopDeclTopTerm_type/array08/' (')
/TopLevelBlock/TopDeclTopTerm_type/array08/value (name81)
/TopLevelBlock/TopDeclTopTerm_type/array08/value (name82)
/TopLevelBlock/TopDeclTopTerm_type/array08/value (name83)
/TopLevelBlock/TopDeclTopTerm_type/array08/' (')
/TopLevelBlock/TopDeclTopTerm_type/term ([])
/TopLevelBlock/term ([])
)INPUT";

    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}

/**
 * @brief Test HIT syntax - period in block declarators
 */
TEST(HITInterpreter, period_in_block_declarators)
{
    std::stringstream input;
    input << R"INPUT(
[TopBlock]
  [child.01.period.wt.type]
    type = Type01
    var01 = val01
  []
  [./child.02.period.wt.type]
    type = Type02
    var02 = val02
  [../]
  [child.03.period.no.type]
    var03 = val03
  [../]
  [./child.04.period.no.type]
    var04 = val04
  []
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(52, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/TopBlock
/TopBlock/[ ([)
/TopBlock/decl (TopBlock)
/TopBlock/] (])
/TopBlock/Type01_type
/TopBlock/Type01_type/[ ([)
/TopBlock/Type01_type/decl (child.01.period.wt.type)
/TopBlock/Type01_type/] (])
/TopBlock/Type01_type/type
/TopBlock/Type01_type/type/decl (type)
/TopBlock/Type01_type/type/= (=)
/TopBlock/Type01_type/type/value (Type01)
/TopBlock/Type01_type/var01
/TopBlock/Type01_type/var01/decl (var01)
/TopBlock/Type01_type/var01/= (=)
/TopBlock/Type01_type/var01/value (val01)
/TopBlock/Type01_type/term ([])
/TopBlock/Type02_type
/TopBlock/Type02_type/[ ([)
/TopBlock/Type02_type/./ (./)
/TopBlock/Type02_type/decl (child.02.period.wt.type)
/TopBlock/Type02_type/] (])
/TopBlock/Type02_type/type
/TopBlock/Type02_type/type/decl (type)
/TopBlock/Type02_type/type/= (=)
/TopBlock/Type02_type/type/value (Type02)
/TopBlock/Type02_type/var02
/TopBlock/Type02_type/var02/decl (var02)
/TopBlock/Type02_type/var02/= (=)
/TopBlock/Type02_type/var02/value (val02)
/TopBlock/Type02_type/term ([../])
/TopBlock/child.03.period.no.type
/TopBlock/child.03.period.no.type/[ ([)
/TopBlock/child.03.period.no.type/decl (child.03.period.no.type)
/TopBlock/child.03.period.no.type/] (])
/TopBlock/child.03.period.no.type/var03
/TopBlock/child.03.period.no.type/var03/decl (var03)
/TopBlock/child.03.period.no.type/var03/= (=)
/TopBlock/child.03.period.no.type/var03/value (val03)
/TopBlock/child.03.period.no.type/term ([../])
/TopBlock/child.04.period.no.type
/TopBlock/child.04.period.no.type/[ ([)
/TopBlock/child.04.period.no.type/./ (./)
/TopBlock/child.04.period.no.type/decl (child.04.period.no.type)
/TopBlock/child.04.period.no.type/] (])
/TopBlock/child.04.period.no.type/var04
/TopBlock/child.04.period.no.type/var04/decl (var04)
/TopBlock/child.04.period.no.type/var04/= (=)
/TopBlock/child.04.period.no.type/var04/value (val04)
/TopBlock/child.04.period.no.type/term ([])
/TopBlock/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - equals, brackets, and double quotes in arrays
 */
TEST(HITInterpreter, equals_brackets_quotes_in_arrays)
{
    std::stringstream input;
    input << R"INPUT(
array01 = 'd2F1:=D[F1,eta1,eta1] d2F2:=D[F2,eta1,eta1]'
[BlockContainer]
  array02 = ' 08.4;; ; -5;;123;abc:=t[e_x]t;;-02.85;  '
  array03 = 'path/to/real=1.01;path/to/int=10;path/to/string=one;path/to/blank=""
             path/to/real=2.02;path/to/int=20;path/to/string=two;path/to/blank=""'
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(51, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/array01
/array01/decl (array01)
/array01/= (=)
/array01/' (')
/array01/value (d2F1:=D[F1,eta1,eta1])
/array01/value (d2F2:=D[F2,eta1,eta1])
/array01/' (')
/BlockContainer
/BlockContainer/[ ([)
/BlockContainer/decl (BlockContainer)
/BlockContainer/] (])
/BlockContainer/array02
/BlockContainer/array02/decl (array02)
/BlockContainer/array02/= (=)
/BlockContainer/array02/' (')
/BlockContainer/array02/value (08.4)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (-5)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (123)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (abc:=t[e_x]t)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (-02.85)
/BlockContainer/array02/; (;)
/BlockContainer/array02/' (')
/BlockContainer/array03
/BlockContainer/array03/decl (array03)
/BlockContainer/array03/= (=)
/BlockContainer/array03/' (')
/BlockContainer/array03/value (path/to/real=1.01)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/int=10)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/string=one)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/blank="")
/BlockContainer/array03/value (path/to/real=2.02)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/int=20)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/string=two)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/blank="")
/BlockContainer/array03/' (')
/BlockContainer/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - non-ascii unicode in comments
 */
TEST(HITInterpreter, non_ascii_unicode_in_comments)
{
    std::stringstream input;
    input << R"INPUT(
######################################################################
##                                                                  ##
## Lead off comment with non-ascii unicode degree symbol 600°C      ##
## Lead off comment with non-ascii unicode “L&R double quotes”      ##
## Lead off comment with non-ascii unicode ‘L&R single quotes’      ##
## Lead off comment with non-ascii unicode en dash – character      ##
##                                                                  ##
######################################################################

[ObjName]
  Param01 = val01 # trailing comment with non-ascii unicode degree symbol 600°C
  Param02 = val02 # trailing comment with non-ascii unicode “L&R double quotes”
  Param03 = val03 # trailing comment with non-ascii unicode ‘L&R single quotes’
  Param04 = val04 # trailing comment with non-ascii unicode en dash – character
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(34, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(9, document.child_count());
    ASSERT_EQ(9, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/comment (######################################################################)
/comment (##                                                                  ##)
/comment (## Lead off comment with non-ascii unicode degree symbol 600°C      ##)
/comment (## Lead off comment with non-ascii unicode “L&R double quotes”      ##)
/comment (## Lead off comment with non-ascii unicode ‘L&R single quotes’      ##)
/comment (## Lead off comment with non-ascii unicode en dash – character      ##)
/comment (##                                                                  ##)
/comment (######################################################################)
/ObjName
/ObjName/[ ([)
/ObjName/decl (ObjName)
/ObjName/] (])
/ObjName/Param01
/ObjName/Param01/decl (Param01)
/ObjName/Param01/= (=)
/ObjName/Param01/value (val01)
/ObjName/comment (# trailing comment with non-ascii unicode degree symbol 600°C)
/ObjName/Param02
/ObjName/Param02/decl (Param02)
/ObjName/Param02/= (=)
/ObjName/Param02/value (val02)
/ObjName/comment (# trailing comment with non-ascii unicode “L&R double quotes”)
/ObjName/Param03
/ObjName/Param03/decl (Param03)
/ObjName/Param03/= (=)
/ObjName/Param03/value (val03)
/ObjName/comment (# trailing comment with non-ascii unicode ‘L&R single quotes’)
/ObjName/Param04
/ObjName/Param04/decl (Param04)
/ObjName/Param04/= (=)
/ObjName/Param04/value (val04)
/ObjName/comment (# trailing comment with non-ascii unicode en dash – character)
/ObjName/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - multi-line and multi-group arrays
 */
TEST(HITInterpreter, multi_line_multi_group_arrays)
{
    std::stringstream input;
    input << R"INPUT(
[Params]

  array01 = '-flag_01 -flag_02 -flag_03   '
             '  -flag_04'
             ' -flag_05
               -flag_06' '-flag_07'

  array02 = '01 02.4 15.6 strain_xx strain_yy strain_zz '
                           '  19.8 max_principal_stress  96 ' ' more 8.3 vals'
            ' group_04 line_03 values '

  array03 = '   vonmises_stress hydrostatic_stress stress_xx stress_yy
       stress_zz '

                           'strain_xx strain_yy strain_zz'
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(65, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/Params
/Params/[ ([)
/Params/decl (Params)
/Params/] (])
/Params/array01
/Params/array01/decl (array01)
/Params/array01/= (=)
/Params/array01/' (')
/Params/array01/value (-flag_01)
/Params/array01/value (-flag_02)
/Params/array01/value (-flag_03)
/Params/array01/' (')
/Params/array01/' (')
/Params/array01/value (-flag_04)
/Params/array01/' (')
/Params/array01/' (')
/Params/array01/value (-flag_05)
/Params/array01/value (-flag_06)
/Params/array01/' (')
/Params/array01/' (')
/Params/array01/value (-flag_07)
/Params/array01/' (')
/Params/array02
/Params/array02/decl (array02)
/Params/array02/= (=)
/Params/array02/' (')
/Params/array02/value (01)
/Params/array02/value (02.4)
/Params/array02/value (15.6)
/Params/array02/value (strain_xx)
/Params/array02/value (strain_yy)
/Params/array02/value (strain_zz)
/Params/array02/' (')
/Params/array02/' (')
/Params/array02/value (19.8)
/Params/array02/value (max_principal_stress)
/Params/array02/value (96)
/Params/array02/' (')
/Params/array02/' (')
/Params/array02/value (more)
/Params/array02/value (8.3)
/Params/array02/value (vals)
/Params/array02/' (')
/Params/array02/' (')
/Params/array02/value (group_04)
/Params/array02/value (line_03)
/Params/array02/value (values)
/Params/array02/' (')
/Params/array03
/Params/array03/decl (array03)
/Params/array03/= (=)
/Params/array03/' (')
/Params/array03/value (vonmises_stress)
/Params/array03/value (hydrostatic_stress)
/Params/array03/value (stress_xx)
/Params/array03/value (stress_yy)
/Params/array03/value (stress_zz)
/Params/array03/' (')
/Params/array03/' (')
/Params/array03/value (strain_xx)
/Params/array03/value (strain_yy)
/Params/array03/value (strain_zz)
/Params/array03/' (')
/Params/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - equals and double quotes after slash in value
 */
TEST(HITInterpreter, equals_and_quotes_after_slash)
{
    std::stringstream input;
    input << R"INPUT(
[SlashesInValues]
  [input01]
    type = DirAndFile
    directory = some/directory/path01
    inputfile = file01.i
    parameter = path/to/a/param01=one
  []
  [input02]
    type = DirAndFile
    directory = some/directory/path02/
    inputfile = file02.i
    parameter = path/to/a/param02/="two"
  []
  [input03]
    type = DirAndFile
    directory = /some/directory/path03
    inputfile = file03.i
    parameter = /path/to/a/param03=3.3
  []
  [input04]
    type = DirAndFile
    directory = /some/directory/path04/
    inputfile = file04.i
    parameter = /path/to/a/param04/=444
  []
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(90, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/SlashesInValues
/SlashesInValues/[ ([)
/SlashesInValues/decl (SlashesInValues)
/SlashesInValues/] (])
/SlashesInValues/DirAndFile_type
/SlashesInValues/DirAndFile_type/[ ([)
/SlashesInValues/DirAndFile_type/decl (input01)
/SlashesInValues/DirAndFile_type/] (])
/SlashesInValues/DirAndFile_type/type
/SlashesInValues/DirAndFile_type/type/decl (type)
/SlashesInValues/DirAndFile_type/type/= (=)
/SlashesInValues/DirAndFile_type/type/value (DirAndFile)
/SlashesInValues/DirAndFile_type/directory
/SlashesInValues/DirAndFile_type/directory/decl (directory)
/SlashesInValues/DirAndFile_type/directory/= (=)
/SlashesInValues/DirAndFile_type/directory/value (some/directory/path01)
/SlashesInValues/DirAndFile_type/inputfile
/SlashesInValues/DirAndFile_type/inputfile/decl (inputfile)
/SlashesInValues/DirAndFile_type/inputfile/= (=)
/SlashesInValues/DirAndFile_type/inputfile/value (file01.i)
/SlashesInValues/DirAndFile_type/parameter
/SlashesInValues/DirAndFile_type/parameter/decl (parameter)
/SlashesInValues/DirAndFile_type/parameter/= (=)
/SlashesInValues/DirAndFile_type/parameter/value (path/to/a/param01=one)
/SlashesInValues/DirAndFile_type/term ([])
/SlashesInValues/DirAndFile_type
/SlashesInValues/DirAndFile_type/[ ([)
/SlashesInValues/DirAndFile_type/decl (input02)
/SlashesInValues/DirAndFile_type/] (])
/SlashesInValues/DirAndFile_type/type
/SlashesInValues/DirAndFile_type/type/decl (type)
/SlashesInValues/DirAndFile_type/type/= (=)
/SlashesInValues/DirAndFile_type/type/value (DirAndFile)
/SlashesInValues/DirAndFile_type/directory
/SlashesInValues/DirAndFile_type/directory/decl (directory)
/SlashesInValues/DirAndFile_type/directory/= (=)
/SlashesInValues/DirAndFile_type/directory/value (some/directory/path02/)
/SlashesInValues/DirAndFile_type/inputfile
/SlashesInValues/DirAndFile_type/inputfile/decl (inputfile)
/SlashesInValues/DirAndFile_type/inputfile/= (=)
/SlashesInValues/DirAndFile_type/inputfile/value (file02.i)
/SlashesInValues/DirAndFile_type/parameter
/SlashesInValues/DirAndFile_type/parameter/decl (parameter)
/SlashesInValues/DirAndFile_type/parameter/= (=)
/SlashesInValues/DirAndFile_type/parameter/value (path/to/a/param02/="two")
/SlashesInValues/DirAndFile_type/term ([])
/SlashesInValues/DirAndFile_type
/SlashesInValues/DirAndFile_type/[ ([)
/SlashesInValues/DirAndFile_type/decl (input03)
/SlashesInValues/DirAndFile_type/] (])
/SlashesInValues/DirAndFile_type/type
/SlashesInValues/DirAndFile_type/type/decl (type)
/SlashesInValues/DirAndFile_type/type/= (=)
/SlashesInValues/DirAndFile_type/type/value (DirAndFile)
/SlashesInValues/DirAndFile_type/directory
/SlashesInValues/DirAndFile_type/directory/decl (directory)
/SlashesInValues/DirAndFile_type/directory/= (=)
/SlashesInValues/DirAndFile_type/directory/value (/some/directory/path03)
/SlashesInValues/DirAndFile_type/inputfile
/SlashesInValues/DirAndFile_type/inputfile/decl (inputfile)
/SlashesInValues/DirAndFile_type/inputfile/= (=)
/SlashesInValues/DirAndFile_type/inputfile/value (file03.i)
/SlashesInValues/DirAndFile_type/parameter
/SlashesInValues/DirAndFile_type/parameter/decl (parameter)
/SlashesInValues/DirAndFile_type/parameter/= (=)
/SlashesInValues/DirAndFile_type/parameter/value (/path/to/a/param03=3.3)
/SlashesInValues/DirAndFile_type/term ([])
/SlashesInValues/DirAndFile_type
/SlashesInValues/DirAndFile_type/[ ([)
/SlashesInValues/DirAndFile_type/decl (input04)
/SlashesInValues/DirAndFile_type/] (])
/SlashesInValues/DirAndFile_type/type
/SlashesInValues/DirAndFile_type/type/decl (type)
/SlashesInValues/DirAndFile_type/type/= (=)
/SlashesInValues/DirAndFile_type/type/value (DirAndFile)
/SlashesInValues/DirAndFile_type/directory
/SlashesInValues/DirAndFile_type/directory/decl (directory)
/SlashesInValues/DirAndFile_type/directory/= (=)
/SlashesInValues/DirAndFile_type/directory/value (/some/directory/path04/)
/SlashesInValues/DirAndFile_type/inputfile
/SlashesInValues/DirAndFile_type/inputfile/decl (inputfile)
/SlashesInValues/DirAndFile_type/inputfile/= (=)
/SlashesInValues/DirAndFile_type/inputfile/value (file04.i)
/SlashesInValues/DirAndFile_type/parameter
/SlashesInValues/DirAndFile_type/parameter/decl (parameter)
/SlashesInValues/DirAndFile_type/parameter/= (=)
/SlashesInValues/DirAndFile_type/parameter/value (/path/to/a/param04/=444)
/SlashesInValues/DirAndFile_type/term ([])
/SlashesInValues/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming for regular and type blocks
 */
TEST(HITInterpreter, shorthand_path_naming_blocks)
{
    std::stringstream input;
    input << R"INPUT(

[a/b/c]
  param_outer = bar_outer
  [x/y/z]
    param_inner = bar_inner
  []
[]

[a/b/c]
  type        = foo_outer
  param_outer = bar_outer
  [x/y/z]
    type        = foo_inner
    param_inner = bar_inner
  []
[]

[/a/b///c//]
  param = bar
[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(64, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (a/b/c)
/a/b/c/] (])
/a/b/c/param_outer
/a/b/c/param_outer/decl (param_outer)
/a/b/c/param_outer/= (=)
/a/b/c/param_outer/value (bar_outer)
/a/b/c/x
/a/b/c/x/y
/a/b/c/x/y/z
/a/b/c/x/y/z/[ ([)
/a/b/c/x/y/z/decl (x/y/z)
/a/b/c/x/y/z/] (])
/a/b/c/x/y/z/param_inner
/a/b/c/x/y/z/param_inner/decl (param_inner)
/a/b/c/x/y/z/param_inner/= (=)
/a/b/c/x/y/z/param_inner/value (bar_inner)
/a/b/c/x/y/z/term ([])
/a/b/c/term ([])
/a
/a/b
/a/b/foo_outer_type
/a/b/foo_outer_type/[ ([)
/a/b/foo_outer_type/decl (a/b/c)
/a/b/foo_outer_type/] (])
/a/b/foo_outer_type/type
/a/b/foo_outer_type/type/decl (type)
/a/b/foo_outer_type/type/= (=)
/a/b/foo_outer_type/type/value (foo_outer)
/a/b/foo_outer_type/param_outer
/a/b/foo_outer_type/param_outer/decl (param_outer)
/a/b/foo_outer_type/param_outer/= (=)
/a/b/foo_outer_type/param_outer/value (bar_outer)
/a/b/foo_outer_type/x
/a/b/foo_outer_type/x/y
/a/b/foo_outer_type/x/y/foo_inner_type
/a/b/foo_outer_type/x/y/foo_inner_type/[ ([)
/a/b/foo_outer_type/x/y/foo_inner_type/decl (x/y/z)
/a/b/foo_outer_type/x/y/foo_inner_type/] (])
/a/b/foo_outer_type/x/y/foo_inner_type/type
/a/b/foo_outer_type/x/y/foo_inner_type/type/decl (type)
/a/b/foo_outer_type/x/y/foo_inner_type/type/= (=)
/a/b/foo_outer_type/x/y/foo_inner_type/type/value (foo_inner)
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner/decl (param_inner)
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner/= (=)
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner/value (bar_inner)
/a/b/foo_outer_type/x/y/foo_inner_type/term ([])
/a/b/foo_outer_type/term ([])
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (/a/b///c//)
/a/b/c/] (])
/a/b/c/param
/a/b/c/param/decl (param)
/a/b/c/param/= (=)
/a/b/c/param/value (bar)
/a/b/c/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed value parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

path/to/param/inner/01 = "one"

[//path/to///object/]

  ///path/to///param/inner/02// = 2.2

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/param
/path/to/param/inner
/path/to/param/inner/01
/path/to/param/inner/01/decl (path/to/param/inner/01)
/path/to/param/inner/01/= (=)
/path/to/param/inner/01/value ("one")
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/param
/path/to/object/path/to/param/inner
/path/to/object/path/to/param/inner/02
/path/to/object/path/to/param/inner/02/decl (///path/to///param/inner/02//)
/path/to/object/path/to/param/inner/02/= (=)
/path/to/object/path/to/param/inner/02/value (2.2)
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed array parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_arrays)
{
    std::stringstream input;
    input << R"INPUT(

path/to/array/inner/01 = '"eleven" "twelve" "thirteen"'

[//path/to///object/]

  ///path/to///array/inner/02// = '2.2 3.3 4.4'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(32, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/array
/path/to/array/inner
/path/to/array/inner/01
/path/to/array/inner/01/decl (path/to/array/inner/01)
/path/to/array/inner/01/= (=)
/path/to/array/inner/01/' (')
/path/to/array/inner/01/value ("eleven")
/path/to/array/inner/01/value ("twelve")
/path/to/array/inner/01/value ("thirteen")
/path/to/array/inner/01/' (')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/inner
/path/to/object/path/to/array/inner/02
/path/to/object/path/to/array/inner/02/decl (///path/to///array/inner/02//)
/path/to/object/path/to/array/inner/02/= (=)
/path/to/object/path/to/array/inner/02/' (')
/path/to/object/path/to/array/inner/02/value (2.2)
/path/to/object/path/to/array/inner/02/value (3.3)
/path/to/object/path/to/array/inner/02/value (4.4)
/path/to/object/path/to/array/inner/02/' (')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - keyed-keyed-value parameters
 */
TEST(HITInterpreter, keyed_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

param_outer_01 = path/to/param/inner/01="one"

[//path/to///object/]

  param_outer_02=///path/to///param/inner/02//=2.2

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/param_outer_01
/param_outer_01/decl (param_outer_01)
/param_outer_01/= (=)
/param_outer_01/value (path/to/param/inner/01="one")
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/param_outer_02
/path/to/object/param_outer_02/decl (param_outer_02)
/path/to/object/param_outer_02/= (=)
/path/to/object/param_outer_02/value (///path/to///param/inner/02//=2.2)
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - keyed-keyed-array parameters
 */
TEST(HITInterpreter, keyed_keyed_arrays)
{
    std::stringstream input;
    input << R"INPUT(

array_outer_01 = path/to/array/inner/01='"eleven";"twelve";"thirteen"'

[//path/to///object/]

  array_outer_02 = ///path/to///array/inner/02//='2.2;3.3;4.4'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/array_outer_01
/array_outer_01/decl (array_outer_01)
/array_outer_01/= (=)
/array_outer_01/value (path/to/array/inner/01='"eleven";"twelve";"thirteen"')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/array_outer_02
/path/to/object/array_outer_02/decl (array_outer_02)
/path/to/object/array_outer_02/= (=)
/path/to/object/array_outer_02/value (///path/to///array/inner/02//='2.2;3.3;4.4')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed-keyed-value parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

path/to/param/outer/01 = path/to/param/inner/01="one"

[//path/to///object/]

  ///path/to///param/outer/02// = ///path/to///param/inner/02//=2.2

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/param
/path/to/param/outer
/path/to/param/outer/01
/path/to/param/outer/01/decl (path/to/param/outer/01)
/path/to/param/outer/01/= (=)
/path/to/param/outer/01/value (path/to/param/inner/01="one")
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/param
/path/to/object/path/to/param/outer
/path/to/object/path/to/param/outer/02
/path/to/object/path/to/param/outer/02/decl (///path/to///param/outer/02//)
/path/to/object/path/to/param/outer/02/= (=)
/path/to/object/path/to/param/outer/02/value (///path/to///param/inner/02//=2.2)
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed-keyed-array parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_keyed_arrays)
{
    std::stringstream input;
    input << R"INPUT(

path/to/array/outer/01 = path/to/array/inner/01='"eleven";"twelve";"thirteen"'

[//path/to///object/]

  ///path/to///array/outer/02// = ///path/to///array/inner/02//='2.2;3.3;4.4'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/array
/path/to/array/outer
/path/to/array/outer/01
/path/to/array/outer/01/decl (path/to/array/outer/01)
/path/to/array/outer/01/= (=)
/path/to/array/outer/01/value (path/to/array/inner/01='"eleven";"twelve";"thirteen"')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/outer
/path/to/object/path/to/array/outer/02
/path/to/object/path/to/array/outer/02/decl (///path/to///array/outer/02//)
/path/to/object/path/to/array/outer/02/= (=)
/path/to/object/path/to/array/outer/02/value (///path/to///array/inner/02//='2.2;3.3;4.4')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - array values that are keyed values
 */
TEST(HITInterpreter, arrays_containing_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

array_outer_01 = 'param_inner_01 = 100 param_inner_02 = 200 param_inner_03 = 300'

[//path/to///object/]

  path/to/array//outer/02/ = 'param/inner/01 = 100 param/inner/02 = 200 param/inner/03 = 300'

[]

///path//to/array/outer/03 = array_outer_01='param_inner_01=100;param_inner_02=200;param_inner_03=300'

[//path/to///object/]

  path/to/array/outer//04 = path/to/array//outer/02/='param/inner/01=100;param/inner/02=200;param/inner/03=300'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(63, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(4, document.child_count());
    ASSERT_EQ(4, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/array_outer_01
/array_outer_01/decl (array_outer_01)
/array_outer_01/= (=)
/array_outer_01/' (')
/array_outer_01/value (param_inner_01)
/array_outer_01/value (=)
/array_outer_01/value (100)
/array_outer_01/value (param_inner_02)
/array_outer_01/value (=)
/array_outer_01/value (200)
/array_outer_01/value (param_inner_03)
/array_outer_01/value (=)
/array_outer_01/value (300)
/array_outer_01/' (')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/outer
/path/to/object/path/to/array/outer/02
/path/to/object/path/to/array/outer/02/decl (path/to/array//outer/02/)
/path/to/object/path/to/array/outer/02/= (=)
/path/to/object/path/to/array/outer/02/' (')
/path/to/object/path/to/array/outer/02/value (param/inner/01)
/path/to/object/path/to/array/outer/02/value (=)
/path/to/object/path/to/array/outer/02/value (100)
/path/to/object/path/to/array/outer/02/value (param/inner/02)
/path/to/object/path/to/array/outer/02/value (=)
/path/to/object/path/to/array/outer/02/value (200)
/path/to/object/path/to/array/outer/02/value (param/inner/03)
/path/to/object/path/to/array/outer/02/value (=)
/path/to/object/path/to/array/outer/02/value (300)
/path/to/object/path/to/array/outer/02/' (')
/path/to/object/term ([])
/path
/path/to
/path/to/array
/path/to/array/outer
/path/to/array/outer/03
/path/to/array/outer/03/decl (///path//to/array/outer/03)
/path/to/array/outer/03/= (=)
/path/to/array/outer/03/value (array_outer_01='param_inner_01=100;param_inner_02=200;param_inner_03=300')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/outer
/path/to/object/path/to/array/outer/04
/path/to/object/path/to/array/outer/04/decl (path/to/array/outer//04)
/path/to/object/path/to/array/outer/04/= (=)
/path/to/object/path/to/array/outer/04/value (path/to/array//outer/02/='param/inner/01=100;param/inner/02=200;param/inner/03=300')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief standalone file include not found error
 */
TEST(HITInterpreter, only_include_not_found)
{
    std::stringstream input;
    input << R"I(!include block_missing.i)I" << std::endl;
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input line:1 column:1 : could not find 'block_missing.i'" << std::endl;

    ASSERT_EQ(expected_errors.str(), errors.str());
}

/**
 * @brief standalone file include
 */
TEST(HITInterpreter, only_include)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("block.i");
    block_file << "[block] " <<std::endl
               << "  key = 3"<<std::endl
               << "[]"<<std::endl;
    block_file.close();
    }

    std::stringstream input;
    input << R"I(!include block.i)I" << std::endl;

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/incl
/incl/decl (!include)
/incl/path (block.i)
)INPUT";

    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief file include within a block
 */
TEST(HITInterpreter, include_block)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("nested_block.i");
    block_file << "[nested_block] " <<std::endl
               << "  key = 3"<<std::endl
               << "[]"<<std::endl;
    block_file.close();
    }

    std::stringstream input;
    input << R"I([block]!include nested_block.i[])I" << std::endl;

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/block
/block/[ ([)
/block/decl (block)
/block/] (])
/block/incl
/block/incl/decl (!include)
/block/incl/path (nested_block.i)
/block/term ([])
)INPUT";

    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());

    // Obtain the nested content
    HITNodeView document = interpreter.root();
    HITNodeView block = document.child_at(0);
    ASSERT_EQ(std::string("block"), block.name());
    HITNodeView::Collection children = block.non_decorative_children();
    ASSERT_EQ(1, children.size());
    ASSERT_EQ(std::string("nested_block"), children[0].name());

    // Ensure the path produces from a nested document's node includes
    // the context of the parent document's nodes
    auto key_node = children[0].first_child_by_name("key");
    ASSERT_FALSE(key_node.is_null());
    ASSERT_EQ("/block/nested_block/key", key_node.path());

    // Ensure the 'first_child_by_name' properly traverses file boundaries
    auto nested_block = block.first_child_by_name("nested_block");
    ASSERT_EQ(children[0], nested_block);
    ASSERT_EQ(children[0], block.child_by_name("nested_block")[0]);
}


/**
 * @brief Test FilePush iterator when iterating
 * Should iterate over variables a,b,c, where b
 * is actually obtained from an included file
 */
TEST(HITInterpreter, iterating_include)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("b.i");
    block_file << "b = 2" <<std::endl;
    block_file.close();
    }

    std::stringstream input;
    input << R"I(a = 1
!include b.i
c = 3)I" << std::endl;

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));

    // Obtain the nested content
    HITNodeView document = interpreter.root();

    std::vector<std::string> expected_names = {"a", "b", "c"};
    std::vector<std::string> expected_data = {"1", "2", "3"};
    std::vector<std::string> expected_paths = {"stream input", "./b.i", "stream input"};
    size_t index = 0;
    for(auto itr = document.begin(); itr != document.end(); itr.next())
    {
        SCOPED_TRACE(index);
        const HITNodeView& node = itr.get();
        ASSERT_EQ(expected_names[index], node.name());
        ASSERT_EQ(expected_data[index], node.to_string());
        ASSERT_EQ(expected_paths[index], node.node_pool()->stream_name());
        ++index;
    }

}

/**
 * @brief Test HIT syntax error - missing object terminator
 */
TEST(HITInterpreter, missing_object_terminator)
{
    std::stringstream input;
    input << R"INPUT([GlobalParams]
[one]
  [one_sub]
    one_sub_param=100
[]
[two]
  two_param=200
[]
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expect_errors;
    expect_errors << "stream input:9.1: syntax error, unexpected end of file"
                  << std::endl;

    ASSERT_EQ(expect_errors.str(), actual_errors.str());
}

/**
 * @brief Test HIT syntax error - object name with invalid character '#'
 */
TEST(HITInterpreter, object_name_with_invalid_pound)
{
    std::stringstream input;
    input << R"INPUT(
[#objectname] []
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expect_errors;
    expect_errors << "stream input:2.2: syntax error, unexpected invalid token, expecting subblock indicator ./ or integer or object name"
                  << std::endl;

    ASSERT_EQ(expect_errors.str(), actual_errors.str());
}

/**
 * @brief Test HIT syntax error - object name with invalid character '='
 */
TEST(HITInterpreter, object_name_with_invalid_equals)
{
    std::stringstream input;
    input << R"INPUT(
[object=name] []
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expect_errors;
    expect_errors << "stream input:2.8: syntax error, unexpected invalid token, expecting ]"
                  << std::endl;

    ASSERT_EQ(expect_errors.str(), actual_errors.str());
}

/**
 * @brief Test HIT syntax error - object name with invalid character '&'
 */
TEST(HITInterpreter, object_name_with_invalid_ampersand)
{
    std::stringstream input;
    input << R"INPUT(
[objectname&] []
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expect_errors;
    expect_errors << "stream input:2.12: syntax error, unexpected invalid token, expecting ]"
                  << std::endl;

    ASSERT_EQ(expect_errors.str(), actual_errors.str());
}

/**
 * @brief Test HIT syntax error - parameter name with invalid character '&'
 */
TEST(HITInterpreter, parameter_name_with_invalid_ampersand)
{
    std::stringstream input;
    input << R"INPUT(
param&name=10
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expect_errors;
    expect_errors << "stream input:2.6: syntax error, unexpected invalid token, expecting ="
                  << std::endl;

    ASSERT_EQ(expect_errors.str(), actual_errors.str());
}

/**
 * @brief Test HIT syntax error - invalid object terminator
 */
TEST(HITInterpreter, invalid_object_terminator)
{
    std::stringstream input;
    input << R"INPUT(
[object_name][./]
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expect_errors;
    expect_errors << "stream input:2.17: syntax error, unexpected ], expecting integer or object name"
                  << std::endl;

    ASSERT_EQ(expect_errors.str(), actual_errors.str());
}

/**
 * @brief Test HIT syntax - brace expression spans multiple lines
 */
TEST(HITInterpreter, brace_expression_spans_multiple_lines)
{
    std::stringstream input;
    input << R"INPUT(
param_name=${raw 4
                 2
            }
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/param_name
/param_name/decl (param_name)
/param_name/= (=)
/param_name/value (${raw 4
                 2
            })
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - quote chaining scenarios
 */
TEST(HITInterpreter, quote_chaining_scenarios)
{
    std::stringstream input;
    input << R"INPUT(
[quotes]
    [singles]
        field = ' "    mix1a mix1b "   "mix1c"    mix1d 1.2

                  "mix\"1e "  mix1f "    mix1g" 34  "mix\'1h "
                ' 'mix\'2a'
                '"mix\3a"' '    '
    []
    [doubles]
        field = " '    mix1a mix1b '   'mix1c'    mix1d 1.2

                  'mix\'1e '  mix1f '    mix1g' 34  'mix\"1h '
                " "mix\"2a"
                "'mix\3a'" "    "
    []
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(45, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/quotes
/quotes/[ ([)
/quotes/decl (quotes)
/quotes/] (])
/quotes/singles
/quotes/singles/[ ([)
/quotes/singles/decl (singles)
/quotes/singles/] (])
/quotes/singles/field
/quotes/singles/field/decl (field)
/quotes/singles/field/= (=)
/quotes/singles/field/' (')
/quotes/singles/field/value ("    mix1a mix1b ")
/quotes/singles/field/value ("mix1c")
/quotes/singles/field/value (mix1d)
/quotes/singles/field/value (1.2)
/quotes/singles/field/value ("mix\"1e ")
/quotes/singles/field/value (mix1f)
/quotes/singles/field/value ("    mix1g")
/quotes/singles/field/value (34)
/quotes/singles/field/value ("mix\'1h ")
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/field/value (mix\'2a)
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/field/value ("mix\3a")
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/term ([])
/quotes/doubles
/quotes/doubles/[ ([)
/quotes/doubles/decl (doubles)
/quotes/doubles/] (])
/quotes/doubles/field
/quotes/doubles/field/decl (field)
/quotes/doubles/field/= (=)
/quotes/doubles/field/value (" '    mix1a mix1b '   'mix1c'    mix1d 1.2

                  'mix\'1e '  mix1f '    mix1g' 34  'mix\"1h '
                ")
/quotes/doubles/field/value ("mix\"2a")
/quotes/doubles/field/value ("'mix\3a'")
/quotes/doubles/field/value ("    ")
/quotes/doubles/term ([])
/quotes/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - ampersands and semicolons in values
 */
TEST(HITInterpreter, ampersands_and_semicolons_in_values)
{
    std::stringstream input;
    input << R"INPUT(

value_wt_quotes_ampersand_beg = '&if((x<1)(y>0.5),1E7,-1E7)'
value_wt_quotes_ampersand_mid = 'if((x<1)&(y>0.5),1E7,-1E7)'
value_wt_quotes_ampersand_end = 'if((x<1)(y>0.5),1E7,-1E7)&'

value_no_quotes_ampersand_beg = &if((x<1)(y>0.5),1E7,-1E7)
value_no_quotes_ampersand_mid = if((x<1)&(y>0.5),1E7,-1E7)
value_no_quotes_ampersand_end = if((x<1)(y>0.5),1E7,-1E7)&

value_no_quotes_semicolon_beg = ;if((x<1)(y>0.5),1E7,-1E7)
value_no_quotes_semicolon_mid = if((x<1);(y>0.5),1E7,-1E7)
value_no_quotes_semicolon_end = if((x<1)(y>0.5),1E7,-1E7);

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(43, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(9, document.child_count());
    ASSERT_EQ(9, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/value_wt_quotes_ampersand_beg
/value_wt_quotes_ampersand_beg/decl (value_wt_quotes_ampersand_beg)
/value_wt_quotes_ampersand_beg/= (=)
/value_wt_quotes_ampersand_beg/' (')
/value_wt_quotes_ampersand_beg/value (&if((x<1)(y>0.5),1E7,-1E7))
/value_wt_quotes_ampersand_beg/' (')
/value_wt_quotes_ampersand_mid
/value_wt_quotes_ampersand_mid/decl (value_wt_quotes_ampersand_mid)
/value_wt_quotes_ampersand_mid/= (=)
/value_wt_quotes_ampersand_mid/' (')
/value_wt_quotes_ampersand_mid/value (if((x<1)&(y>0.5),1E7,-1E7))
/value_wt_quotes_ampersand_mid/' (')
/value_wt_quotes_ampersand_end
/value_wt_quotes_ampersand_end/decl (value_wt_quotes_ampersand_end)
/value_wt_quotes_ampersand_end/= (=)
/value_wt_quotes_ampersand_end/' (')
/value_wt_quotes_ampersand_end/value (if((x<1)(y>0.5),1E7,-1E7)&)
/value_wt_quotes_ampersand_end/' (')
/value_no_quotes_ampersand_beg
/value_no_quotes_ampersand_beg/decl (value_no_quotes_ampersand_beg)
/value_no_quotes_ampersand_beg/= (=)
/value_no_quotes_ampersand_beg/value (&if((x<1)(y>0.5),1E7,-1E7))
/value_no_quotes_ampersand_mid
/value_no_quotes_ampersand_mid/decl (value_no_quotes_ampersand_mid)
/value_no_quotes_ampersand_mid/= (=)
/value_no_quotes_ampersand_mid/value (if((x<1)&(y>0.5),1E7,-1E7))
/value_no_quotes_ampersand_end
/value_no_quotes_ampersand_end/decl (value_no_quotes_ampersand_end)
/value_no_quotes_ampersand_end/= (=)
/value_no_quotes_ampersand_end/value (if((x<1)(y>0.5),1E7,-1E7)&)
/value_no_quotes_semicolon_beg
/value_no_quotes_semicolon_beg/decl (value_no_quotes_semicolon_beg)
/value_no_quotes_semicolon_beg/= (=)
/value_no_quotes_semicolon_beg/value (;if((x<1)(y>0.5),1E7,-1E7))
/value_no_quotes_semicolon_mid
/value_no_quotes_semicolon_mid/decl (value_no_quotes_semicolon_mid)
/value_no_quotes_semicolon_mid/= (=)
/value_no_quotes_semicolon_mid/value (if((x<1);(y>0.5),1E7,-1E7))
/value_no_quotes_semicolon_end
/value_no_quotes_semicolon_end/decl (value_no_quotes_semicolon_end)
/value_no_quotes_semicolon_end/= (=)
/value_no_quotes_semicolon_end/value (if((x<1)(y>0.5),1E7,-1E7);)
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - period starts block names and paths
 */
TEST(HITInterpreter, period_starts_block_names_and_paths)
{
    std::stringstream input;
    input << R"INPUT(

[Block_Names_Begin_With_Letter]

  [child_1a_no_type]
    var_1a = val_1a
  []

  [child_1b_wt_type]
    type   = Type_1b
    var_1b = val_1b
  []

[]

[Block_Names_Begin_With_Period]

  [.child_2a_no_type]
    var_2a = val_2a
  []

  [.child_2b_wt_type]
    type   = Type_2b
    var_2b = val_2b
  []

[]

[Block_Paths_Begin_With_Period]

  [.child/3a/no/type]
    var_3a = val_3a
  []

  [.child/3b/wt/type]
    type   = Type_3b
    var_3b = val_3b
  []

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(88, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/Block_Names_Begin_With_Letter
/Block_Names_Begin_With_Letter/[ ([)
/Block_Names_Begin_With_Letter/decl (Block_Names_Begin_With_Letter)
/Block_Names_Begin_With_Letter/] (])
/Block_Names_Begin_With_Letter/child_1a_no_type
/Block_Names_Begin_With_Letter/child_1a_no_type/[ ([)
/Block_Names_Begin_With_Letter/child_1a_no_type/decl (child_1a_no_type)
/Block_Names_Begin_With_Letter/child_1a_no_type/] (])
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a/decl (var_1a)
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a/= (=)
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a/value (val_1a)
/Block_Names_Begin_With_Letter/child_1a_no_type/term ([])
/Block_Names_Begin_With_Letter/Type_1b_type
/Block_Names_Begin_With_Letter/Type_1b_type/[ ([)
/Block_Names_Begin_With_Letter/Type_1b_type/decl (child_1b_wt_type)
/Block_Names_Begin_With_Letter/Type_1b_type/] (])
/Block_Names_Begin_With_Letter/Type_1b_type/type
/Block_Names_Begin_With_Letter/Type_1b_type/type/decl (type)
/Block_Names_Begin_With_Letter/Type_1b_type/type/= (=)
/Block_Names_Begin_With_Letter/Type_1b_type/type/value (Type_1b)
/Block_Names_Begin_With_Letter/Type_1b_type/var_1b
/Block_Names_Begin_With_Letter/Type_1b_type/var_1b/decl (var_1b)
/Block_Names_Begin_With_Letter/Type_1b_type/var_1b/= (=)
/Block_Names_Begin_With_Letter/Type_1b_type/var_1b/value (val_1b)
/Block_Names_Begin_With_Letter/Type_1b_type/term ([])
/Block_Names_Begin_With_Letter/term ([])
/Block_Names_Begin_With_Period
/Block_Names_Begin_With_Period/[ ([)
/Block_Names_Begin_With_Period/decl (Block_Names_Begin_With_Period)
/Block_Names_Begin_With_Period/] (])
/Block_Names_Begin_With_Period/.child_2a_no_type
/Block_Names_Begin_With_Period/.child_2a_no_type/[ ([)
/Block_Names_Begin_With_Period/.child_2a_no_type/decl (.child_2a_no_type)
/Block_Names_Begin_With_Period/.child_2a_no_type/] (])
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a/decl (var_2a)
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a/= (=)
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a/value (val_2a)
/Block_Names_Begin_With_Period/.child_2a_no_type/term ([])
/Block_Names_Begin_With_Period/Type_2b_type
/Block_Names_Begin_With_Period/Type_2b_type/[ ([)
/Block_Names_Begin_With_Period/Type_2b_type/decl (.child_2b_wt_type)
/Block_Names_Begin_With_Period/Type_2b_type/] (])
/Block_Names_Begin_With_Period/Type_2b_type/type
/Block_Names_Begin_With_Period/Type_2b_type/type/decl (type)
/Block_Names_Begin_With_Period/Type_2b_type/type/= (=)
/Block_Names_Begin_With_Period/Type_2b_type/type/value (Type_2b)
/Block_Names_Begin_With_Period/Type_2b_type/var_2b
/Block_Names_Begin_With_Period/Type_2b_type/var_2b/decl (var_2b)
/Block_Names_Begin_With_Period/Type_2b_type/var_2b/= (=)
/Block_Names_Begin_With_Period/Type_2b_type/var_2b/value (val_2b)
/Block_Names_Begin_With_Period/Type_2b_type/term ([])
/Block_Names_Begin_With_Period/term ([])
/Block_Paths_Begin_With_Period
/Block_Paths_Begin_With_Period/[ ([)
/Block_Paths_Begin_With_Period/decl (Block_Paths_Begin_With_Period)
/Block_Paths_Begin_With_Period/] (])
/Block_Paths_Begin_With_Period/.child
/Block_Paths_Begin_With_Period/.child/3a
/Block_Paths_Begin_With_Period/.child/3a/no
/Block_Paths_Begin_With_Period/.child/3a/no/type
/Block_Paths_Begin_With_Period/.child/3a/no/type/[ ([)
/Block_Paths_Begin_With_Period/.child/3a/no/type/decl (.child/3a/no/type)
/Block_Paths_Begin_With_Period/.child/3a/no/type/] (])
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a/decl (var_3a)
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a/= (=)
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a/value (val_3a)
/Block_Paths_Begin_With_Period/.child/3a/no/type/term ([])
/Block_Paths_Begin_With_Period/.child
/Block_Paths_Begin_With_Period/.child/3b
/Block_Paths_Begin_With_Period/.child/3b/wt
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/[ ([)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/decl (.child/3b/wt/type)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/] (])
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/type
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/type/decl (type)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/type/= (=)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/type/value (Type_3b)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/var_3b
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/var_3b/decl (var_3b)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/var_3b/= (=)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/var_3b/value (val_3b)
/Block_Paths_Begin_With_Period/.child/3b/wt/Type_3b_type/term ([])
/Block_Paths_Begin_With_Period/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}
