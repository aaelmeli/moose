/*
 * File:   tstSON.cpp
 * Author: raq
 *
 * Created on September 26, 2013, 11:43 AM
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include <waspson/son_config.h>

#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(SON, keyed_value)
{
    std::stringstream input;
    input << R"INPUT(string = val
                    qstring="quoted string"
                    qstr='quoted string'
                    integer = 10323
                    real = 1.1232
                    "qkey" = str
                    'quoted key' = ' 1 12 '
                    tag ( "the id" ) = valu
                    tag ( an_id ) = vl
            )INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(43, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(9, document.child_count());
    std::string       expected_paths = R"INPUT(/
/string
/string/decl (string)
/string/= (=)
/string/value (val)
/qstring
/qstring/decl (qstring)
/qstring/= (=)
/qstring/value ("quoted string")
/qstr
/qstr/decl (qstr)
/qstr/= (=)
/qstr/value ('quoted string')
/integer
/integer/decl (integer)
/integer/= (=)
/integer/value (10323)
/real
/real/decl (real)
/real/= (=)
/real/value (1.1232)
/qkey
/qkey/decl ("qkey")
/qkey/= (=)
/qkey/value (str)
/quoted key
/quoted key/decl ('quoted key')
/quoted key/= (=)
/quoted key/value (' 1 12 ')
/tag
/tag/decl (tag)
/tag/( (()
/tag/id ("the id")
/tag/) ())
/tag/= (=)
/tag/value (valu)
/tag
/tag/decl (tag)
/tag/( (()
/tag/id (an_id)
/tag/) ())
/tag/= (=)
/tag/value (vl)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::ASSIGN,      wasp::VALUE,        wasp::KEYED_VALUE,
        wasp::DECL,   wasp::LPAREN,      wasp::IDENTIFIER,   wasp::RPAREN,
        wasp::ASSIGN, wasp::VALUE,       wasp::KEYED_VALUE,  wasp::DECL,
        wasp::LPAREN, wasp::IDENTIFIER,  wasp::RPAREN,       wasp::ASSIGN,
        wasp::VALUE,  wasp::KEYED_VALUE, wasp::DOCUMENT_ROOT};
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
    // Test iterator
    {
    size_t index = 0;
    for(auto itr = document.begin(); itr != document.end(); itr.next(), ++index)
    {
        SCOPED_TRACE(index);
        ASSERT_TRUE(index < document.child_count());
        ASSERT_EQ(document.child_at(index), itr.get());
    }
    }
}
TEST(SON, empty_object)
{
    std::stringstream input;
    input << R"INPUT(e1 {}
            e2{
            }
            e3 = { }
            'e4' : {}
            e5(id1) = { }
            'e6'("id 2") : {}
            )INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(35, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(6, document.child_count());
    std::string       expected_paths = R"INPUT(/
/e1
/e1/decl (e1)
/e1/{ ({)
/e1/} (})
/e2
/e2/decl (e2)
/e2/{ ({)
/e2/} (})
/e3
/e3/decl (e3)
/e3/= (=)
/e3/{ ({)
/e3/} (})
/e4
/e4/decl ('e4')
/e4/= (:)
/e4/{ ({)
/e4/} (})
/e5
/e5/decl (e5)
/e5/( (()
/e5/id (id1)
/e5/) ())
/e5/= (=)
/e5/{ ({)
/e5/} (})
/e6
/e6/decl ('e6')
/e6/( (()
/e6/id ("id 2")
/e6/) ())
/e6/= (:)
/e6/{ ({)
/e6/} (})
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::DECL,       wasp::LBRACE, wasp::RBRACE,       wasp::OBJECT,
        wasp::DECL,       wasp::LBRACE, wasp::RBRACE,       wasp::OBJECT,
        wasp::DECL,       wasp::ASSIGN, wasp::LBRACE,       wasp::RBRACE,
        wasp::OBJECT,     wasp::DECL,   wasp::ASSIGN,       wasp::LBRACE,
        wasp::RBRACE,     wasp::OBJECT, wasp::DECL,         wasp::LPAREN,
        wasp::IDENTIFIER, wasp::RPAREN, wasp::ASSIGN,       wasp::LBRACE,
        wasp::RBRACE,     wasp::OBJECT, wasp::DECL,         wasp::LPAREN,
        wasp::IDENTIFIER, wasp::RPAREN, wasp::ASSIGN,       wasp::LBRACE,
        wasp::RBRACE,     wasp::OBJECT, wasp::DOCUMENT_ROOT};
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
}

TEST(SON, empty_array)
{
    std::stringstream input;
    input << R"INPUT(e1 []
            e2[
            ]
            e3 = [ ]
            'e4' : []
            e5(id1) = [ ]
            'e6'("id 2") : []
            )INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(35, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(6, document.child_count());
    std::string       expected_paths = R"INPUT(/
/e1
/e1/decl (e1)
/e1/[ ([)
/e1/] (])
/e2
/e2/decl (e2)
/e2/[ ([)
/e2/] (])
/e3
/e3/decl (e3)
/e3/= (=)
/e3/[ ([)
/e3/] (])
/e4
/e4/decl ('e4')
/e4/= (:)
/e4/[ ([)
/e4/] (])
/e5
/e5/decl (e5)
/e5/( (()
/e5/id (id1)
/e5/) ())
/e5/= (=)
/e5/[ ([)
/e5/] (])
/e6
/e6/decl ('e6')
/e6/( (()
/e6/id ("id 2")
/e6/) ())
/e6/= (:)
/e6/[ ([)
/e6/] (])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::DECL,       wasp::LBRACKET, wasp::RBRACKET,     wasp::ARRAY,
        wasp::DECL,       wasp::LBRACKET, wasp::RBRACKET,     wasp::ARRAY,
        wasp::DECL,       wasp::ASSIGN,   wasp::LBRACKET,     wasp::RBRACKET,
        wasp::ARRAY,      wasp::DECL,     wasp::ASSIGN,       wasp::LBRACKET,
        wasp::RBRACKET,   wasp::ARRAY,    wasp::DECL,         wasp::LPAREN,
        wasp::IDENTIFIER, wasp::RPAREN,   wasp::ASSIGN,       wasp::LBRACKET,
        wasp::RBRACKET,   wasp::ARRAY,    wasp::DECL,         wasp::LPAREN,
        wasp::IDENTIFIER, wasp::RPAREN,   wasp::ASSIGN,       wasp::LBRACKET,
        wasp::RBRACKET,   wasp::ARRAY,    wasp::DOCUMENT_ROOT};
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        ASSERT_EQ(types[i], interpreter.type(i));
    }
}

TEST(SON, empty_execution_unit)
{
    std::stringstream input;
    input << R"INPUT(=code
end)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/code
/code/uoe_start (=)
/code/name (code)
/code/uoe_end (end)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {wasp::EXECUTION_UNIT_START, wasp::DECL,
                                     EXECUTION_UNIT_END, EXECUTION_UNIT,
                                     wasp::DOCUMENT_ROOT};
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
}
TEST(SON, execution_unit)
{
    std::stringstream input;
    input << R"INPUT(=code string = val
                    qstring="quoted string"
                    qstr='quoted string'
                    integer = 10323
                    real = 1.1232
                    "qkey" = str
                    'quoted key' = ' 1 12 '
                    tag ( "the id" ) = valu
                    tag ( an_id ) = vl
end)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(47, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/code
/code/uoe_start (=)
/code/name (code)
/code/string
/code/string/decl (string)
/code/string/= (=)
/code/string/value (val)
/code/qstring
/code/qstring/decl (qstring)
/code/qstring/= (=)
/code/qstring/value ("quoted string")
/code/qstr
/code/qstr/decl (qstr)
/code/qstr/= (=)
/code/qstr/value ('quoted string')
/code/integer
/code/integer/decl (integer)
/code/integer/= (=)
/code/integer/value (10323)
/code/real
/code/real/decl (real)
/code/real/= (=)
/code/real/value (1.1232)
/code/qkey
/code/qkey/decl ("qkey")
/code/qkey/= (=)
/code/qkey/value (str)
/code/quoted key
/code/quoted key/decl ('quoted key')
/code/quoted key/= (=)
/code/quoted key/value (' 1 12 ')
/code/tag
/code/tag/decl (tag)
/code/tag/( (()
/code/tag/id ("the id")
/code/tag/) ())
/code/tag/= (=)
/code/tag/value (valu)
/code/tag
/code/tag/decl (tag)
/code/tag/( (()
/code/tag/id (an_id)
/code/tag/) ())
/code/tag/= (=)
/code/tag/value (vl)
/code/uoe_end (end)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {wasp::EXECUTION_UNIT_START,
                                     wasp::DECL,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::LPAREN,
                                     wasp::IDENTIFIER,
                                     wasp::RPAREN,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     wasp::DECL,
                                     wasp::LPAREN,
                                     wasp::IDENTIFIER,
                                     wasp::RPAREN,
                                     wasp::ASSIGN,
                                     wasp::VALUE,
                                     wasp::KEYED_VALUE,
                                     EXECUTION_UNIT_END,
                                     EXECUTION_UNIT,
                                     wasp::DOCUMENT_ROOT};
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
}

TEST(SON, int_array)
{
    std::stringstream input;
    input << R"INPUT(
            ints=[1]
            integers =
                    [
                        4 5 6 7
                    ]
            "2darray"
                     [ 4 5
                       6 7 ]
            )INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::string       expected_paths = R"INPUT(/
/ints
/ints/decl (ints)
/ints/= (=)
/ints/[ ([)
/ints/value (1)
/ints/] (])
/integers
/integers/decl (integers)
/integers/= (=)
/integers/[ ([)
/integers/value (4)
/integers/value (5)
/integers/value (6)
/integers/value (7)
/integers/] (])
/2darray
/2darray/decl ("2darray")
/2darray/[ ([)
/2darray/value (4)
/2darray/value (5)
/2darray/value (6)
/2darray/value (7)
/2darray/] (])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::DECL,     wasp::ASSIGN,   wasp::LBRACKET, wasp::VALUE,
        wasp::RBRACKET, wasp::ARRAY,    wasp::DECL,     wasp::ASSIGN,
        wasp::LBRACKET, wasp::VALUE,    wasp::VALUE,    wasp::VALUE,
        wasp::VALUE,    wasp::RBRACKET, wasp::ARRAY,    wasp::DECL,
        wasp::LBRACKET, wasp::VALUE,    wasp::VALUE,    wasp::VALUE,
        wasp::VALUE,    wasp::RBRACKET, wasp::ARRAY,    wasp::DOCUMENT_ROOT};
    std::vector<wasp::NODE> token = {
        wasp::STRING,   wasp::ASSIGN,   wasp::LBRACKET, wasp::INTEGER,
        wasp::RBRACKET, wasp::UNKNOWN,  wasp::STRING,   wasp::ASSIGN,
        wasp::LBRACKET, wasp::INTEGER,      wasp::INTEGER,      wasp::INTEGER,
        wasp::INTEGER,      wasp::RBRACKET, wasp::UNKNOWN,  wasp::QUOTED_STRING,
        wasp::LBRACKET, wasp::INTEGER,      wasp::INTEGER,      wasp::INTEGER,
        wasp::INTEGER,      wasp::RBRACKET, wasp::UNKNOWN,  wasp::UNKNOWN};
    ASSERT_EQ(types.size(), interpreter.node_count());
    ASSERT_EQ(token.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
            ASSERT_EQ(token[i], interpreter.node_token_type(i));
        }
    }
}
TEST(SON, real_array)
{
    std::stringstream input;
    input << R"INPUT(
            1=[1.0]
            real =
                    [
                        4.0 5.2 1.0e6 1e-7
                    ]
            3.14
                     [ 4.3 5.4
                       6.1 7.0 ]
            )INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::string       expected_paths = R"INPUT(/
/1
/1/decl (1)
/1/= (=)
/1/[ ([)
/1/value (1.0)
/1/] (])
/real
/real/decl (real)
/real/= (=)
/real/[ ([)
/real/value (4.0)
/real/value (5.2)
/real/value (1.0e6)
/real/value (1e-7)
/real/] (])
/3.14
/3.14/decl (3.14)
/3.14/[ ([)
/3.14/value (4.3)
/3.14/value (5.4)
/3.14/value (6.1)
/3.14/value (7.0)
/3.14/] (])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::DECL,     wasp::ASSIGN,   wasp::LBRACKET, wasp::VALUE,
        wasp::RBRACKET, wasp::ARRAY,    wasp::DECL,     wasp::ASSIGN,
        wasp::LBRACKET, wasp::VALUE,    wasp::VALUE,    wasp::VALUE,
        wasp::VALUE,    wasp::RBRACKET, wasp::ARRAY,    wasp::DECL,
        wasp::LBRACKET, wasp::VALUE,    wasp::VALUE,    wasp::VALUE,
        wasp::VALUE,    wasp::RBRACKET, wasp::ARRAY,    wasp::DOCUMENT_ROOT};
    std::vector<wasp::NODE> token = {wasp::INTEGER  // decl
                                     ,
                                     wasp::ASSIGN,
                                     wasp::LBRACKET,
                                     wasp::REAL,
                                     wasp::RBRACKET,
                                     wasp::UNKNOWN,
                                     wasp::STRING,
                                     wasp::ASSIGN,
                                     wasp::LBRACKET,
                                     wasp::REAL,
                                     wasp::REAL,
                                     wasp::REAL,
                                     wasp::REAL,
                                     wasp::RBRACKET,
                                     wasp::UNKNOWN,
                                     wasp::REAL  // decl
                                     ,
                                     wasp::LBRACKET,
                                     wasp::REAL,
                                     wasp::REAL,
                                     wasp::REAL,
                                     wasp::REAL,
                                     wasp::RBRACKET,
                                     wasp::UNKNOWN,
                                     wasp::UNKNOWN};
    ASSERT_EQ(types.size(), interpreter.node_count());
    ASSERT_EQ(token.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
            ASSERT_EQ(token[i], interpreter.node_token_type(i));
        }
    }
}

TEST(SON, comments)
{
    std::stringstream input;
    input << R"INPUT(
% start of line
   k=1 % trailing line
            )INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(7, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::string       expected_paths = R"INPUT(/
/comment (% start of line)
/k
/k/decl (k)
/k/= (=)
/k/value (1)
/comment (% trailing line)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::COMMENT,     wasp::DECL,    wasp::ASSIGN,       wasp::VALUE,
        wasp::KEYED_VALUE, wasp::COMMENT, wasp::DOCUMENT_ROOT};
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
}

/**
 * @brief TEST the following permutations
 * Objects with comments, keys (w/expression), arrays, and objects
 * Arrays with comments, scalars, keys, arrays, and objects
 * Execution units with comments, keys, arrays and objects
 */
TEST(SON, SON)
{
    std::stringstream input;
    input << R"INPUT( var = 3.14 % global variable
 g_obj { k=v a[ 1 ] g{x=y} }
=eu
    obj { e=-5
            r[ 1.0 ] n { g=d } }
    arr [ r=5.3
            % A array has a 2 elements, 1) subtraction expression followed by
            % and 2) an unary minus expression
            A[ 1.0-5 -4 ] O { g=d } ]
end % conclusion of unit of execution
)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(80, interpreter.node_count());
    auto document = interpreter.root();
    // var, comment, global_obj, execution unit, comment
    ASSERT_EQ(5, document.child_count());
    std::string       expected_paths = R"INPUT(/
/var
/var/decl (var)
/var/= (=)
/var/value (3.14)
/comment (% global variable)
/g_obj
/g_obj/decl (g_obj)
/g_obj/{ ({)
/g_obj/k
/g_obj/k/decl (k)
/g_obj/k/= (=)
/g_obj/k/value (v)
/g_obj/a
/g_obj/a/decl (a)
/g_obj/a/[ ([)
/g_obj/a/value (1)
/g_obj/a/] (])
/g_obj/g
/g_obj/g/decl (g)
/g_obj/g/{ ({)
/g_obj/g/x
/g_obj/g/x/decl (x)
/g_obj/g/x/= (=)
/g_obj/g/x/value (y)
/g_obj/g/} (})
/g_obj/} (})
/eu
/eu/uoe_start (=)
/eu/name (eu)
/eu/obj
/eu/obj/decl (obj)
/eu/obj/{ ({)
/eu/obj/e
/eu/obj/e/decl (e)
/eu/obj/e/= (=)
/eu/obj/e/value (-5)
/eu/obj/r
/eu/obj/r/decl (r)
/eu/obj/r/[ ([)
/eu/obj/r/value (1.0)
/eu/obj/r/] (])
/eu/obj/n
/eu/obj/n/decl (n)
/eu/obj/n/{ ({)
/eu/obj/n/g
/eu/obj/n/g/decl (g)
/eu/obj/n/g/= (=)
/eu/obj/n/g/value (d)
/eu/obj/n/} (})
/eu/obj/} (})
/eu/arr
/eu/arr/decl (arr)
/eu/arr/[ ([)
/eu/arr/r
/eu/arr/r/decl (r)
/eu/arr/r/= (=)
/eu/arr/r/value (5.3)
/eu/arr/comment (% A array has a 2 elements, 1) subtraction expression followed by)
/eu/arr/comment (% and 2) an unary minus expression)
/eu/arr/A
/eu/arr/A/decl (A)
/eu/arr/A/[ ([)
/eu/arr/A/value
/eu/arr/A/value/value (1.0)
/eu/arr/A/value/- (-)
/eu/arr/A/value/value (5)
/eu/arr/A/value (-4)
/eu/arr/A/] (])
/eu/arr/O
/eu/arr/O/decl (O)
/eu/arr/O/{ ({)
/eu/arr/O/g
/eu/arr/O/g/decl (g)
/eu/arr/O/g/= (=)
/eu/arr/O/g/value (d)
/eu/arr/O/} (})
/eu/arr/] (])
/eu/uoe_end (end)
/comment (% conclusion of unit of execution)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    std::vector<wasp::NODE> types = {
        wasp::DECL  // /var/decl (var)
        ,
        wasp::ASSIGN  // /var/= (=)
        ,
        wasp::VALUE  // /var/value (3.14)
        ,
        wasp::KEYED_VALUE  // /var
        ,
        wasp::COMMENT  // /comment (% global variable)
        ,
        wasp::DECL  // /g_obj/decl (g_obj)
        ,
        wasp::LBRACE  // /g_obj/{ ({)
        ,
        wasp::DECL  // /g_obj/k/decl (k)
        ,
        wasp::ASSIGN  // /g_obj/k/= (=)
        ,
        wasp::VALUE  // /g_obj/k/value (v)
        ,
        wasp::KEYED_VALUE  // /g_obj/k
        ,
        wasp::DECL  // /g_obj/a/decl (a)
        ,
        wasp::LBRACKET  // /g_obj/a/[ ([)
        ,
        wasp::VALUE  // /g_obj/a/value (1)
        ,
        wasp::RBRACKET  // /g_obj/a/] (])
        ,
        wasp::ARRAY  // /g_obj/a
        ,
        wasp::DECL  // /g_obj/g/decl (g)
        ,
        wasp::LBRACE  // /g_obj/g/{ ({)
        ,
        wasp::DECL  // /g_obj/g/x/decl (x)
        ,
        wasp::ASSIGN  // /g_obj/g/x/= (=)
        ,
        wasp::VALUE  // /g_obj/g/x/value (y)
        ,
        wasp::KEYED_VALUE  // /g_obj/g/x
        ,
        wasp::RBRACE  // /g_obj/g/} (})
        ,
        wasp::OBJECT  // /g_obj/g
        ,
        wasp::RBRACE  // /g_obj/} (})
        ,
        wasp::OBJECT  // /g_obj
        ,
        wasp::EXECUTION_UNIT_START  // /eu/uoe_start (=)
        ,
        wasp::DECL  // /eu/name (eu)
        ,
        wasp::DECL  // /eu/obj/decl (obj)
        ,
        wasp::LBRACE  // /eu/obj/{ ({)
        ,
        wasp::DECL  // /eu/obj/e/decl (e)
        ,
        wasp::ASSIGN  // /eu/obj/e/= (=)
        ,
        wasp::VALUE  // /eu/obj/e/value (-5)
        ,
        wasp::KEYED_VALUE  // /eu/obj/e
        ,
        wasp::DECL  // /eu/obj/r/decl (r)
        ,
        wasp::LBRACKET  // /eu/obj/r/[ ([)
        ,
        wasp::VALUE  // /eu/obj/r/value (1.0)
        ,
        wasp::RBRACKET  // /eu/obj/r/] (])
        ,
        wasp::ARRAY  // /eu/obj/r
        ,
        wasp::DECL  // /eu/obj/n/decl (n)
        ,
        wasp::LBRACE  // /eu/obj/n/{ ({)
        ,
        wasp::DECL  // /eu/obj/n/g/decl (g)
        ,
        wasp::ASSIGN  // /eu/obj/n/g/= (=)
        ,
        wasp::VALUE  // /eu/obj/n/g/value (d)
        ,
        wasp::KEYED_VALUE  // /eu/obj/n/g
        ,
        wasp::RBRACE  // /eu/obj/n/} (})
        ,
        wasp::OBJECT  // /eu/obj/n
        ,
        wasp::RBRACE  // /eu/obj/} (})
        ,
        wasp::OBJECT  // /eu/obj
        ,
        wasp::DECL  // /eu/arr/decl (arr)
        ,
        wasp::LBRACKET  // /eu/arr/[ ([)
        ,
        wasp::DECL  // /eu/arr/r/decl (r)
        ,
        wasp::ASSIGN  // /eu/arr/r/= (=)
        ,
        wasp::VALUE  // /eu/arr/r/value (5.3)
        ,
        wasp::KEYED_VALUE  // /eu/arr/r
        ,
        wasp::COMMENT  // /eu/arr/comment (% A array has a 2 elements, 1)
                       // subtraction expression followed by)
        ,
        wasp::COMMENT  // /eu/arr/comment (% and 2) an unary minus expression)
        ,
        wasp::DECL  // /eu/arr/A/decl (A)
        ,
        wasp::LBRACKET  // /eu/arr/A/[ ([)
        ,
        wasp::VALUE  // /eu/arr/A/value/value (1.0)
        ,
        wasp::MINUS  // /eu/arr/A/value/- (-)
        ,
        wasp::VALUE  // /eu/arr/A/value/value (5)
        ,
        wasp::MINUS  // /eu/arr/A/value
        ,
        wasp::VALUE  // /eu/arr/A/value (-4)
        ,
        wasp::RBRACKET  // /eu/arr/A/] (])
        ,
        wasp::ARRAY  // /eu/arr/A
        ,
        wasp::DECL  // /eu/arr/O/decl (O)
        ,
        wasp::LBRACE  // /eu/arr/O/{ ({)
        ,
        wasp::DECL  // /eu/arr/O/g/decl (g)
        ,
        wasp::ASSIGN  // /eu/arr/O/g/= (=)
        ,
        wasp::VALUE  // /eu/arr/O/g/value (d)
        ,
        wasp::KEYED_VALUE  // /eu/arr/O/g
        ,
        wasp::RBRACE  // /eu/arr/O/} (})
        ,
        wasp::OBJECT  // /eu/arr/O
        ,
        wasp::RBRACKET  // /eu/arr/] (])
        ,
        wasp::ARRAY  // /eu/arr
        ,
        wasp::EXECUTION_UNIT_END  // /eu/uoe_end (end)
        ,
        wasp::EXECUTION_UNIT  // /eu
        ,
        wasp::COMMENT  // /comment (% conclusion of unit of execution)
        ,
        wasp::DOCUMENT_ROOT  // /
    };
    ASSERT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
}

/**
 * @brief TEST the supported expressions
 */
TEST(SON, expressions)
{
    std::stringstream input;
    input << R"INPUT( e[
            a+b  a+2  a+1.2  b+a  2+a  1.2+a
            a-b  a-2  a-1.2  b-a  2-a  1.2-a
            a*b  a*2  a*1.2  b*a  2*a  1.2*a
            a/b  a/2  a/1.2  b/a  2/a  1.2/a
            a^b  a^2  a^1.2  b^a  2^a  1.2^a
            a<b  a<2  a<1.2  b<a  2<a  1.2<a
            a>b  a>2  a>1.2  b>a  2>a  1.2>a
            a<=b a<=2 a<=1.2 b<=a 2<=a 1.2<=a
            a>=b a>=2 a>=1.2 b>=a 2>=a 1.2>=a
            a==b a==2 a==1.2 b==a 2==a 1.2==a
            a!=b a!=2 a!=1.2 b!=a 2!=a 1.2!=a
            a&&b a&&2 a&&1.2 b&&a 2&&a 1.2&&a
            a||b a||2 a||1.2 b||a 2||a 1.2||a
            ]
)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    EXPECT_EQ(312 + 5, interpreter.node_count());
    auto document = interpreter.root();
    // var, comment, global_obj, execution unit, comment
    ASSERT_EQ(1, document.child_count());

    std::vector<wasp::NODE> types = {
        DECL,
        LBRACKET,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS,
        VALUE,
        PLUS

        ,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS,
        VALUE,
        MINUS

        ,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY,
        VALUE,
        MULTIPLY

        ,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE,
        VALUE,
        DIVIDE

        ,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT,
        VALUE,
        EXPONENT

        ,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT,
        VALUE,
        LT

        ,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT,
        VALUE,
        GT

        ,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE,
        VALUE,
        LTE

        ,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE,
        VALUE,
        GTE

        ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ,
        VALUE,
        EQ

        ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ,
        VALUE,
        NEQ

        ,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND,
        VALUE,
        WASP_AND

        ,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        VALUE,
        WASP_OR,
        RBRACKET,
        ARRAY,
        wasp::DOCUMENT_ROOT  // /
    };
    EXPECT_EQ(types.size(), interpreter.node_count());
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(types[i], interpreter.type(i));
        }
    }
}

TEST(SONNodeView, HIVE_API)
{
    std::stringstream input;
    input << R"INPUT(
% comment 1
key = value1
key = "value2"
int_array [ 1 2 3 4 ]
real_array [ 1.1 2.2 3.3 4.4 ]
string_array [ 's' "t" " g " h ]
obj(foo){
    % comment 2
    key = value
    int_array [ 1 2 3 4 ]
    real_array [ 1.1 2.2 3.3 4.4 ]
    string_array [ s t g h ]
}
 % comment 3
)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(71, interpreter.node_count());
    SONNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());
    ASSERT_FALSE(document.has_parent());
    {  // test names
        std::vector<std::string> names = {
            "comment",    "key",          "key", "int_array",
            "real_array", "string_array", "obj", "comment"};
        std::vector<std::string> paths = {
            "/comment",    "/key",          "/key", "/int_array",
            "/real_array", "/string_array", "/obj", "/comment"};
        ASSERT_EQ(names.size(), document.child_count());
        for (size_t i = 0; i < names.size(); ++i)
        {
            SCOPED_TRACE(i);
            const auto& view = document.child_at(i);
            std::string name = view.name();
            ASSERT_EQ(names[i], name);
            std::string path = view.path();
            ASSERT_EQ(paths[i], path);
            ASSERT_EQ(name == "comment", view.is_decorative());
        }
        // should be 2 comments -- account accordingly
        ASSERT_EQ(names.size() - 2, document.non_decorative_children_count());
    }
    {  // test comments
        auto                     comments = document.child_by_name("comment");
        std::vector<std::string> data     = {"% comment 1", "% comment 3"};
        std::vector<size_t>      line     = {2, 15};
        std::vector<size_t>      column   = {1, 2};
        ASSERT_EQ(data.size(), comments.size());
        ASSERT_EQ(data.size(), line.size());
        ASSERT_EQ(data.size(), column.size());
        ASSERT_EQ(data.size(), document.child_count_by_name("comment"));

        for (size_t i = 0; i < data.size(); ++i)
        {
            SCOPED_TRACE(i);
            const auto& view = comments[i];
            ASSERT_EQ(data[i], view.data());
            ASSERT_EQ(wasp::COMMENT, view.type());
            std::string name = view.name();
            ASSERT_EQ("comment", name);
            ASSERT_EQ(line[i], view.line());
            ASSERT_EQ(column[i], view.column());
        }
    }
    {  // test to_int
        auto int_arrays = document.child_by_name("int_array");
        ASSERT_EQ(1, int_arrays.size());
        const auto& int_array = int_arrays.front();
        ASSERT_EQ(7, int_array.child_count());
        std::vector<int> value = {1, 2, 3, 4};
        ASSERT_EQ(value.size(), int_array.child_count_by_name("value"));
        const auto& ints = int_array.child_by_name("value");
        ASSERT_EQ(4, ints.size());
        for (size_t i = 0; i < ints.size(); ++i)
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(value[i], ints[i].to_int());
            ASSERT_FALSE(ints[i].is_decorative());
        }
        // TODO - add non_decorative_child test
    }
    {  // test to_double
        auto real_arrays = document.child_by_name("real_array");
        ASSERT_EQ(1, real_arrays.size());
        const auto& real_array = real_arrays.front();
        ASSERT_EQ(7, real_array.child_count());
        std::vector<double> value = {1.1, 2.2, 3.3, 4.4};
        ASSERT_EQ(value.size(), real_array.child_count_by_name("value"));
        const auto& reals = real_array.child_by_name("value");
        ASSERT_EQ(4, reals.size());
        for (size_t i = 0; i < reals.size(); ++i)
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(value[i], reals[i].to_double());
            ASSERT_FALSE(reals[i].is_decorative());
        }
    }
    {  // test to_string
        auto string_arrays = document.child_by_name("string_array");
        ASSERT_EQ(1, string_arrays.size());
        const auto& string_array = string_arrays.front();
        ASSERT_EQ(7, string_array.child_count());
        std::vector<std::string> value = {"s", "t", " g ", "h"};
        ASSERT_EQ(value.size(), string_array.child_count_by_name("value"));
        const auto& strings = string_array.child_by_name("value");
        ASSERT_EQ(4, strings.size());
        for (size_t i = 0; i < strings.size(); ++i)
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(value[i], strings[i].to_string());
            ASSERT_FALSE(strings[i].is_decorative());
        }
    }
    {  // test id methods
        auto objs = document.child_by_name("obj");
        ASSERT_EQ(1, objs.size());
        const auto& obj_view = objs.front();
        ASSERT_EQ("foo", obj_view.id());
        ASSERT_EQ("foo", obj_view.id_child().data());
        ASSERT_EQ("foo", obj_view.first_child_by_name("id").data());
        // 4 members + 1 (id is decorative) non decorative
        ASSERT_EQ(4, obj_view.non_decorative_children_count());
        ASSERT_EQ("", document.id());
        ASSERT_TRUE(document.id_child().is_null());
    }
    {  // test last_as_string
        auto keys = document.child_by_name("key");
        ASSERT_EQ(2, keys.size());
        const auto& key_view1 = keys.front();
        ASSERT_EQ("value1", key_view1.last_as_string());
        ASSERT_EQ("value1", key_view1.to_string());
        ASSERT_EQ("value1", key_view1.first_child_by_name("value").to_string());
        ASSERT_TRUE(
            key_view1.first_non_decorative_child_by_name("dead-ted").is_null());
        const auto& key_view2 = keys.back();
        ASSERT_EQ("value2", key_view2.last_as_string());
        ASSERT_EQ("value2", key_view2.to_string());
        ASSERT_TRUE(key_view1 < key_view2);
        ASSERT_FALSE(key_view2 < key_view1);
    }
}
TEST(SON, data_simple)
{
    std::stringstream input;
    input << "k=1  t =  5";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(9, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
}
TEST(SON, array_of_unary_minus)
{
    std::stringstream input;
    input << "a  [ -1 -2 -3 -4 -5 ]";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    EXPECT_EQ(10, interpreter.node_count());
    SONNodeView document = interpreter.root();
    EXPECT_EQ(1, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    auto array = document.first_child_by_name("a");
    ASSERT_FALSE(array.is_null());
    ASSERT_EQ(5, array.non_decorative_children_count());
}
TEST(SON, keyed_to_value)
{
    std::stringstream input;
    input << "a =ted b = 1 c =  1.3";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    EXPECT_EQ(13, interpreter.node_count());
    SONNodeView document = interpreter.root();
    EXPECT_EQ(3, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    {
        auto a = document.first_child_by_name("a");
        ASSERT_FALSE(a.is_null());
        ASSERT_EQ("ted", a.to_string());
    }
    {
        auto b = document.first_child_by_name("b");
        ASSERT_FALSE(b.is_null());
        ASSERT_EQ(1, b.to_int());
    }
    {
        auto c = document.first_child_by_name("c");
        ASSERT_FALSE(c.is_null());
        ASSERT_EQ(1.3, c.to_double());
    }
}
TEST(SON, data)
{
    std::stringstream input;
    input << R"INPUT(% start of line

   k=1 % trailing line

 t = [1 3 5 ])INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(15, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(4, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST the capture of an expression as the identifer of a key
 */
TEST(SON, key_expression_identifier)
{
    std::stringstream input;
    input << "k( my-id )=1";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    SONNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    ASSERT_EQ(input.str(), document.data());

    auto k_view = document.child_at(0);
    ASSERT_FALSE(k_view.is_null());
    ASSERT_EQ(6, k_view.child_count());
    ASSERT_EQ(1, k_view.non_decorative_children_count());
    auto k_id_view = k_view.id_child();
    ASSERT_FALSE(k_id_view.is_null());
    ASSERT_EQ("id", std::string(k_id_view.name()));
}

/**
 * @brief TEST the capture of an expression as the identifer of an object
 */
TEST(SON, object_expression_identifier)
{
    std::stringstream input;
    input << "obj(my-obj){ k( my-id )=1 }";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(21, interpreter.node_count());
    SONNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    ASSERT_EQ(input.str(), document.data());
    auto obj_view = document.child_at(0);
    ASSERT_EQ(7, obj_view.child_count());
    auto obj_id_view = obj_view.id_child();
    ASSERT_FALSE(obj_id_view.is_null());
    ASSERT_EQ("id", std::string(obj_id_view.name()));
    auto k_view = obj_view.first_child_by_name("k");
    ASSERT_FALSE(k_view.is_null());
    ASSERT_EQ(6, k_view.child_count());
    ASSERT_EQ(1, k_view.non_decorative_children_count());
    auto k_id_view = k_view.id_child();
    ASSERT_FALSE(k_id_view.is_null());
    ASSERT_EQ("id", std::string(k_id_view.name()));
}
TEST(SON, empty_block)
{
    std::stringstream input;
    input << R"INPUT([block])INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());

    std::string       expected_paths = R"INPUT(/
/block
/block/[ ([)
/block/decl (block)
/block/] (])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}
TEST(SON, comment_preceeding_empty_block)
{
    std::stringstream input;
    input << R"INPUT(# comment
    [block])INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(6, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());

    std::string       expected_paths = R"INPUT(/
/comment (# comment)
/block
/block/[ ([)
/block/decl (block)
/block/] (])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}
TEST(SON, populated_block)
{
    std::stringstream input;
    input << R"INPUT([block]
	k=v
	o={}
	a=[]
    # comment
)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(20, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto block_node = document.child_at(0);
    ASSERT_EQ("block", std::string(block_node.name()));

    std::string       expected_paths = R"INPUT(/
/block
/block/[ ([)
/block/decl (block)
/block/] (])
/block/k
/block/k/decl (k)
/block/k/= (=)
/block/k/value (v)
/block/o
/block/o/decl (o)
/block/o/= (=)
/block/o/{ ({)
/block/o/} (})
/block/a
/block/a/decl (a)
/block/a/= (=)
/block/a/[ ([)
/block/a/] (])
/block/comment (# comment)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}
TEST(SON, consecutive_populated_block)
{
    std::stringstream input;
    input << R"INPUT([block]
        k=v
        o={}
        a=[]
 [block1]
         k=v
         o={}
         a=[]
    # comment
)INPUT";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(38, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    auto block_node = document.child_at(0);
    ASSERT_EQ("block", std::string(block_node.name()));
    block_node = document.child_at(1);
    ASSERT_EQ("block1", std::string(block_node.name()));
    std::string       expected_paths = R"INPUT(/
/block
/block/[ ([)
/block/decl (block)
/block/] (])
/block/k
/block/k/decl (k)
/block/k/= (=)
/block/k/value (v)
/block/o
/block/o/decl (o)
/block/o/= (=)
/block/o/{ ({)
/block/o/} (})
/block/a
/block/a/decl (a)
/block/a/= (=)
/block/a/[ ([)
/block/a/] (])
/block1
/block1/[ ([)
/block1/decl (block1)
/block1/] (])
/block1/k
/block1/k/decl (k)
/block1/k/= (=)
/block1/k/value (v)
/block1/o
/block1/o/decl (o)
/block1/o/= (=)
/block1/o/{ ({)
/block1/o/} (})
/block1/a
/block1/a/decl (a)
/block1/a/= (=)
/block1/a/[ ([)
/block1/a/] (])
/block1/comment (# comment)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}
/**
 * @brief TEST the capture of an expression as the identifer of an array
 * DISABLED as the REDUCE/REDUCE conflict causes
 * the identifier pattern to be seen as an expression
 * causing a parse error when encountering the '='
 */
TEST(SON, DISABLED_array_expression_identifier)
{
    std::stringstream input;
    input << "arr(my-arr)[ k( my-id )=1 ]";
    DefaultSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(21, interpreter.node_count());
    SONNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    ASSERT_EQ(input.str(), document.data());
    auto obj_view = document.child_at(0);
    ASSERT_EQ(7, obj_view.child_count());
    auto obj_id_view = obj_view.id_child();
    ASSERT_FALSE(obj_id_view.is_null());
    ASSERT_EQ("id", std::string(obj_id_view.name()));
    auto k_view = obj_view.first_child_by_name("k");
    ASSERT_FALSE(k_view.is_null());
    ASSERT_EQ(6, k_view.child_count());
    ASSERT_EQ(1, k_view.non_decorative_children_count());
    auto k_id_view = k_view.id_child();
    ASSERT_FALSE(k_id_view.is_null());
    ASSERT_EQ("id", std::string(k_id_view.name()));
}


/**
 * @brief standalone file include not found error
 */
TEST(SON, only_include_not_found)
{
    std::stringstream input;
    input << R"I(`import('missing.son'))I" << std::endl;
    std::stringstream errors;
    DefaultSONInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input line:1 column:1 : could not find 'missing.son'" << std::endl;
    
    ASSERT_EQ(expected_errors.str(), errors.str());
}
/**
 * @brief standalone file include
 */
TEST(SON, only_include)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("data.son");
    block_file << "  key = 3" << std::endl;
    block_file.close();
    }
    
    std::stringstream input;
    input << R"I(`import ('data.son'))I" << std::endl;

    DefaultSONInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/import
/import/decl (`import)
/import/( (()
/import/path ('data.son')
/import/) ())
)INPUT";    

    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());

    std::vector<std::string> expected = {"key"};
    SONNodeView root = interpreter.root();
    size_t index = 0;
    for (auto itr = root.begin(); itr != root.end(); itr.next(), ++index)
    {
        ASSERT_EQ(expected[index], itr.get().name());
        // Test that the parent is the parent document's root, not the include'd files root
        ASSERT_EQ(root, itr.get().parent());
    }

    // Test conversion to XML
    // This illustrates that the 'import' is not seen by the conversion
    // and does not pollute the XML document with extraneous annotations
    std::stringstream xml;
    wasp::to_xml(root, xml);
    std::string expected_xml = R"INPUT(<document>
  <key>
    <decl loc="1.3" dec="true">key</decl>
    <ASSIGN loc="1.7" dec="true">=</ASSIGN>
    <value loc="1.9">3</value>
  </key>
</document>
)INPUT";
    ASSERT_EQ(expected_xml, xml.str());
}