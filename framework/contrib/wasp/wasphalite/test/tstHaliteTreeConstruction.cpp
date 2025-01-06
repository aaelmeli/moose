/*
 * File:   tstHalite.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "wasphalite/HaliteInterpreter.h"

#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(Halite, static_text)
{
    std::stringstream input;
    input << R"INPUT(This is plain test
line2
 line3
   line
            )INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(6, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(5, document.child_count());
    std::string       expected_paths = R"INPUT(/
/txt (This is plain test)
/txt (line2)
/txt ( line3)
/txt (   line)
/txt (            )
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

TEST(Halite, all_but_constructs)
{
    std::stringstream input;
    input << R"INPUT(`1234567890-=
~!@#$%^&*()_+
    qwertyuiop[]\+_)(*&^%$#@!~
QWERTYUIOP{}|=-0987654321
asdfghjkl;'\][poiuytrewq
ASDFGHJKL:"';lkjhgfdsa'
zxcvbnm,./  MNBVCXZ
ZXCVBNM  ?.,mnbvcxz)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    auto              document       = interpreter.root();
    std::string       expected_paths = R"INPUT(/
/txt (`1234567890-=)
/txt (~!@#$%^&*()_+)
/txt (    qwertyuiop[]\+_)(*&^%$#@!~)
/txt (QWERTYUIOP{}|=-0987654321)
/txt (asdfghjkl;'\][poiuytrewq)
/txt (ASDFGHJKL:"';lkjhgfdsa')
/txt (zxcvbnm,./  MNBVCXZ)
/txt (ZXCVBNM  ?.,mnbvcxz)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
TEST(Halite, attribute_options_symbols)
{
    std::stringstream input;
    input << R"INPUT(:<a>: options delimiter
|<a>| options silent
?<a>? options optional
use=<a>use= options use object scope
fmt=<a>fmt= options format
sep=<a>sep= options separator)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    auto              document       = interpreter.root();
    std::string       expected_paths = R"INPUT(/
/txt (:)
/attr
/attr/< (<)
/attr/txt (a)
/attr/> (>)
/txt (: options delimiter)
/txt (|)
/attr
/attr/< (<)
/attr/txt (a)
/attr/> (>)
/txt (| options silent)
/txt (?)
/attr
/attr/< (<)
/attr/txt (a)
/attr/> (>)
/txt (? options optional)
/txt (use=)
/attr
/attr/< (<)
/attr/txt (a)
/attr/> (>)
/txt (use= options use object scope)
/txt (fmt=)
/attr
/attr/< (<)
/attr/txt (a)
/attr/> (>)
/txt (fmt= options format)
/txt (sep=)
/attr
/attr/< (<)
/attr/txt (a)
/attr/> (>)
/txt (sep= options separator)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
TEST(Halite, iterative_formatted_attribute_embedded_tree)
{
    std::stringstream input;
    input << R"INPUT(<i:fmt=%3d; i=1,<end>>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    auto              document       = interpreter.root();
    std::string       expected_paths = R"INPUT(/
/attr
/attr/< (<)
/attr/txt (i)
/attr/opt (:fmt=%3d; i=1,)
/attr/attr
/attr/attr/< (<)
/attr/attr/txt (end)
/attr/attr/> (>)
/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
TEST(Halite, simple_parameterized_text)
{
    std::stringstream input;
    input << R"INPUT(This is plain test
<attribute1><attr2><ted>
            )INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(15, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(5, document.child_count());
    std::string       expected_paths = R"INPUT(/
/txt (This is plain test)
/attr
/attr/< (<)
/attr/txt (attribute1)
/attr/> (>)
/attr
/attr/< (<)
/attr/txt (attr2)
/attr/> (>)
/attr
/attr/< (<)
/attr/txt (ted)
/attr/> (>)
/txt (            )
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test attributes with whitespace separations
 */
TEST(Halite, simple_parameterized_text_wss)
{
    std::stringstream input;
    input << R"INPUT(This is plain test
 <attribute1>  <attr2>   <ted>
            )INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(18, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(8, document.child_count());
    std::string       expected_paths = R"INPUT(/
/txt (This is plain test)
/txt ( )
/attr
/attr/< (<)
/attr/txt (attribute1)
/attr/> (>)
/txt (  )
/attr
/attr/< (<)
/attr/txt (attr2)
/attr/> (>)
/txt (   )
/attr
/attr/< (<)
/attr/txt (ted)
/attr/> (>)
/txt (            )
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test nested empty attribute
 */
TEST(Halite, nested_attr_empty)
{
    std::stringstream input;
    input << "<><<>><<<>>>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(19, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::string       expected_paths = R"INPUT(/
/attr
/attr/< (<)
/attr/> (>)
/attr
/attr/< (<)
/attr/attr
/attr/attr/< (<)
/attr/attr/> (>)
/attr/> (>)
/attr
/attr/< (<)
/attr/attr
/attr/attr/< (<)
/attr/attr/attr
/attr/attr/attr/< (<)
/attr/attr/attr/> (>)
/attr/attr/> (>)
/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test nested attribute, suffixed text
 */
TEST(Halite, nested_attr_suffix)
{
    std::stringstream input;
    input << "<<a>b><<<c>d>e>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(21, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    std::string       expected_paths = R"INPUT(/
/attr
/attr/< (<)
/attr/attr
/attr/attr/< (<)
/attr/attr/txt (a)
/attr/attr/> (>)
/attr/txt (b)
/attr/> (>)
/attr
/attr/< (<)
/attr/attr
/attr/attr/< (<)
/attr/attr/attr
/attr/attr/attr/< (<)
/attr/attr/attr/txt (c)
/attr/attr/attr/> (>)
/attr/attr/txt (d)
/attr/attr/> (>)
/attr/txt (e)
/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test nested attribute, prefixed text
 */
TEST(Halite, nested_attr_prefix)
{
    std::stringstream input;
    input << "<b<a>><e<d<c>>>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(21, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    std::string       expected_paths = R"INPUT(/
/attr
/attr/< (<)
/attr/txt (b)
/attr/attr
/attr/attr/< (<)
/attr/attr/txt (a)
/attr/attr/> (>)
/attr/> (>)
/attr
/attr/< (<)
/attr/txt (e)
/attr/attr
/attr/attr/< (<)
/attr/attr/txt (d)
/attr/attr/attr
/attr/attr/attr/< (<)
/attr/attr/attr/txt (c)
/attr/attr/attr/> (>)
/attr/attr/> (>)
/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test nested attribute, prefixed text
 */
TEST(Halite, nested_attr_infix)
{
    std::stringstream input;
    input << "<b< a >b1><e<d<c>d1>e1>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    std::string       expected_paths = R"INPUT(/
/attr
/attr/< (<)
/attr/txt (b)
/attr/attr
/attr/attr/< (<)
/attr/attr/txt ( a )
/attr/attr/> (>)
/attr/txt (b1)
/attr/> (>)
/attr
/attr/< (<)
/attr/txt (e)
/attr/attr
/attr/attr/< (<)
/attr/attr/txt (d)
/attr/attr/attr
/attr/attr/attr/< (<)
/attr/attr/attr/txt (c)
/attr/attr/attr/> (>)
/attr/attr/txt (d1)
/attr/attr/> (>)
/attr/txt (e1)
/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test attribute with surrounding text
 */
TEST(Halite, nested_attr_surrounding)
{
    std::stringstream input;
    input << "prefix <> suffix ";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(6, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::string       expected_paths = R"INPUT(/
/txt (prefix )
/attr
/attr/< (<)
/attr/> (>)
/txt ( suffix )
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test attribute option of formatting
 */
TEST(Halite, attr_options_surrounding)
{
    std::stringstream input;
    input << "prefix <pi:fmt=%2.8f> suffix ";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(8, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    ASSERT_EQ(3, document.child_count());
    auto attr = document.child_at(1);
    ASSERT_EQ(wasp::IDENTIFIER, attr.type());
    ASSERT_EQ(4, attr.child_count());
    std::string       expected_paths = R"INPUT(/
/txt (prefix )
/attr
/attr/< (<)
/attr/txt (pi)
/attr/opt (:fmt=%2.8f)
/attr/> (>)
/txt ( suffix )
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test import statement
 */
TEST(Halite, import_basic)
{
    std::stringstream input;
    input << "#import some/path/to/some/file";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/import
/import/decl (#import)
/import/txt ( some/path/to/some/file)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test repeat statement
 */
TEST(Halite, repeat_basic)
{
    std::stringstream input;
    input << "#repeat some/path/to/some/file";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(wasp::REPEAT, document.child_at(0).type());
    std::string       expected_paths = R"INPUT(/
/repeat
/repeat/decl (#repeat)
/repeat/txt ( some/path/to/some/file)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test parameterized import statement
 */
TEST(Halite, import_parameterized)
{
    std::stringstream input;
    input << "#import <ned>/<ted>/to/some/<fred>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(18, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/import
/import/decl (#import)
/import/txt ( )
/import/attr
/import/attr/< (<)
/import/attr/txt (ned)
/import/attr/> (>)
/import/txt (/)
/import/attr
/import/attr/< (<)
/import/attr/txt (ted)
/import/attr/> (>)
/import/txt (/to/some/)
/import/attr
/import/attr/< (<)
/import/attr/txt (fred)
/import/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test parameterized import statement
 */
TEST(Halite, import_parameterized_using)
{
    std::stringstream input;
    input << "#import <ned>/<ted>/to/some/file using <zed>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(18, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/import
/import/decl (#import)
/import/txt ( )
/import/attr
/import/attr/< (<)
/import/attr/txt (ned)
/import/attr/> (>)
/import/txt (/)
/import/attr
/import/attr/< (<)
/import/attr/txt (ted)
/import/attr/> (>)
/import/txt (/to/some/file using )
/import/attr
/import/attr/< (<)
/import/attr/txt (zed)
/import/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

TEST(Halite, import_parameterized_using_inline)
{
    std::stringstream input;
    input
        << R"INPUT(#import ./import_by_name_inline.tmpl using {"foo":"bar"})INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    //    ASSERT_EQ(18, interpreter.node_count() );
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/import
/import/decl (#import)
/import/txt ( ./import_by_name_inline.tmpl using {"foo":"bar"})
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test parameterized import statement
 */
TEST(Halite, multiple_import_parameterized_using)
{
    std::stringstream input;
    input << "#import <ned>/<ted>/to/some/file using <zed>" << std::endl
          << "random text line" << std::endl
          << "#import some/other/import using <fred>";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(26, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::string       expected_paths = R"INPUT(/
/import
/import/decl (#import)
/import/txt ( )
/import/attr
/import/attr/< (<)
/import/attr/txt (ned)
/import/attr/> (>)
/import/txt (/)
/import/attr
/import/attr/< (<)
/import/attr/txt (ted)
/import/attr/> (>)
/import/txt (/to/some/file using )
/import/attr
/import/attr/< (<)
/import/attr/txt (zed)
/import/attr/> (>)
/txt (random text line)
/import
/import/decl (#import)
/import/txt ( some/other/import using )
/import/attr
/import/attr/< (<)
/import/attr/txt (fred)
/import/attr/> (>)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test ifdef
 */
TEST(Halite, ifdef)
{
    std::stringstream input;
    input << "#ifdef something" << std::endl << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(8, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/ifdef
/A/ifdef/decl (#ifdef)
/A/ifdef/C
/A/ifdef/C/txt ( something)
/A/ifdef/T
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test ifdef with parameter and body
 */
TEST(Halite, ifdef_parameterized)
{
    std::stringstream input;
    input << "#ifdef <x>" << std::endl
          << " intermediate text " << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(13, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/ifdef
/A/ifdef/decl (#ifdef)
/A/ifdef/C
/A/ifdef/C/txt ( )
/A/ifdef/C/attr
/A/ifdef/C/attr/< (<)
/A/ifdef/C/attr/txt (x)
/A/ifdef/C/attr/> (>)
/A/ifdef/T
/A/ifdef/T/txt ( intermediate text )
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test ifndef with parameter and body
 */
TEST(Halite, ifndef_parameterized)
{
    std::stringstream input;
    input << "#ifndef <x>" << std::endl
          << " intermediate text " << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(13, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/ifndef
/A/ifndef/decl (#ifndef)
/A/ifndef/C
/A/ifndef/C/txt ( )
/A/ifndef/C/attr
/A/ifndef/C/attr/< (<)
/A/ifndef/C/attr/txt (x)
/A/ifndef/C/attr/> (>)
/A/ifndef/T
/A/ifndef/T/txt ( intermediate text )
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test if with parameter and body
 */
TEST(Halite, if_parameterized)
{
    std::stringstream input;
    input << "#if <x> < <y>" << std::endl
          << " only if x is less than y " << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(18, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/if
/A/if/decl (#if)
/A/if/C
/A/if/C/txt ( )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (x)
/A/if/C/attr/> (>)
/A/if/C/txt ( < )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (y)
/A/if/C/attr/> (>)
/A/if/T
/A/if/T/txt ( only if x is less than y )
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}

/**
 * @brief TEST test if elseif with parameter and body
 */
TEST(Halite, elseif_parameterized)
{
    std::stringstream input;
    input << "#if <x> < <y>" << std::endl
          << " only if x is less than y " << std::endl
          << "#elseif condition " << std::endl
          << " only if x is not less than y and condition is true " << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/if
/A/if/decl (#if)
/A/if/C
/A/if/C/txt ( )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (x)
/A/if/C/attr/> (>)
/A/if/C/txt ( < )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (y)
/A/if/C/attr/> (>)
/A/if/T
/A/if/T/txt ( only if x is less than y )
/A/elseif
/A/elseif/decl (#elseif)
/A/elseif/C
/A/elseif/C/txt ( condition )
/A/elseif/T
/A/elseif/T/txt ( only if x is not less than y and condition is true )
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test if elseif else with parameter and body
 */
TEST(Halite, if_elseif_else_parameterized)
{
    std::stringstream input;
    input << "#if <x> < <y>" << std::endl
          << " only if x is less than y " << std::endl
          << "#elseif condition " << std::endl
          << " only if x is not less than y and condition is true " << std::endl
          << "#else" << std::endl
          << "last bastion" << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(27, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/if
/A/if/decl (#if)
/A/if/C
/A/if/C/txt ( )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (x)
/A/if/C/attr/> (>)
/A/if/C/txt ( < )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (y)
/A/if/C/attr/> (>)
/A/if/T
/A/if/T/txt ( only if x is less than y )
/A/elseif
/A/elseif/decl (#elseif)
/A/elseif/C
/A/elseif/C/txt ( condition )
/A/elseif/T
/A/elseif/T/txt ( only if x is not less than y and condition is true )
/A/else
/A/else/decl (#else)
/A/else/txt (last bastion)
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test if elseif with parameter and body
 */
TEST(Halite, else_parameterized)
{
    std::stringstream input;
    input << "#if <x> < <y>" << std::endl
          << " only if x is less than y " << std::endl
          << "#else" << std::endl
          << " only if x is not less than y " << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(21, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::string       expected_paths = R"INPUT(/
/A
/A/if
/A/if/decl (#if)
/A/if/C
/A/if/C/txt ( )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (x)
/A/if/C/attr/> (>)
/A/if/C/txt ( < )
/A/if/C/attr
/A/if/C/attr/< (<)
/A/if/C/attr/txt (y)
/A/if/C/attr/> (>)
/A/if/T
/A/if/T/txt ( only if x is less than y )
/A/else
/A/else/decl (#else)
/A/else/txt ( only if x is not less than y )
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
/**
 * @brief TEST test if conditionals with parameters, body, and nested conditions
 */
TEST(Halite, conditionals)
{
    std::stringstream input;
    input << "conditional template" << std::endl
          << "#if cond1" << std::endl
          << " cond1 must be true" << std::endl
          << "#ifdef cond2" << std::endl
          << " cond1 must be true and 2 must be defined (eval to true)"
          << std::endl
          << "#else" << std::endl
          << " cond1 must be true and 2 must not be defined (eval to false)"
          << std::endl
          << "#endif" << std::endl
          << "#elseif cond3" << std::endl
          << " cond1 is false and cond3 true" << std::endl
          << "#ifndef cond4" << std::endl
          << " cond1 must be false, 3 true and 4 not defined (evals to false)"
          << std::endl
          << "#elseif cond5" << std::endl
          << " cond1 is false, 3 is true, 4 not defined, 5 true" << std::endl
          << "#else" << std::endl
          << " cond1 is false, 3 is true, 4 not defined, 5 false" << std::endl
          << "#endif" << std::endl
          << "#else" << std::endl
          << "cond1 and 3 are false" << std::endl
          << "#if cond6" << std::endl
          << " cond1 must be false, 3 false and 6 true" << std::endl
          << "#elseif cond7" << std::endl
          << " cond1 is false, 3 is false, 6 false, and 7 true" << std::endl
          << "#else" << std::endl
          << " cond1 is false, 3 is false, 6 false, and 7 false" << std::endl
          << "#endif" << std::endl
          << "#endif";
    DefaultHaliteInterpreter interpreter(std::cout);
    ASSERT_TRUE(interpreter.parse(input));
    //    ASSERT_EQ(18, interpreter.node_count() );
    auto document = interpreter.root();
    //    ASSERT_EQ( 2, document.child_count() );
    std::string       expected_paths = R"INPUT(/
/txt (conditional template)
/A
/A/if
/A/if/decl (#if)
/A/if/C
/A/if/C/txt ( cond1)
/A/if/T
/A/if/T/txt ( cond1 must be true)
/A/if/T/A
/A/if/T/A/ifdef
/A/if/T/A/ifdef/decl (#ifdef)
/A/if/T/A/ifdef/C
/A/if/T/A/ifdef/C/txt ( cond2)
/A/if/T/A/ifdef/T
/A/if/T/A/ifdef/T/txt ( cond1 must be true and 2 must be defined (eval to true))
/A/if/T/A/else
/A/if/T/A/else/decl (#else)
/A/if/T/A/else/txt ( cond1 must be true and 2 must not be defined (eval to false))
/A/if/T/A/endif (#endif)
/A/elseif
/A/elseif/decl (#elseif)
/A/elseif/C
/A/elseif/C/txt ( cond3)
/A/elseif/T
/A/elseif/T/txt ( cond1 is false and cond3 true)
/A/elseif/T/A
/A/elseif/T/A/ifndef
/A/elseif/T/A/ifndef/decl (#ifndef)
/A/elseif/T/A/ifndef/C
/A/elseif/T/A/ifndef/C/txt ( cond4)
/A/elseif/T/A/ifndef/T
/A/elseif/T/A/ifndef/T/txt ( cond1 must be false, 3 true and 4 not defined (evals to false))
/A/elseif/T/A/elseif
/A/elseif/T/A/elseif/decl (#elseif)
/A/elseif/T/A/elseif/C
/A/elseif/T/A/elseif/C/txt ( cond5)
/A/elseif/T/A/elseif/T
/A/elseif/T/A/elseif/T/txt ( cond1 is false, 3 is true, 4 not defined, 5 true)
/A/elseif/T/A/else
/A/elseif/T/A/else/decl (#else)
/A/elseif/T/A/else/txt ( cond1 is false, 3 is true, 4 not defined, 5 false)
/A/elseif/T/A/endif (#endif)
/A/else
/A/else/decl (#else)
/A/else/txt (cond1 and 3 are false)
/A/else/A
/A/else/A/if
/A/else/A/if/decl (#if)
/A/else/A/if/C
/A/else/A/if/C/txt ( cond6)
/A/else/A/if/T
/A/else/A/if/T/txt ( cond1 must be false, 3 false and 6 true)
/A/else/A/elseif
/A/else/A/elseif/decl (#elseif)
/A/else/A/elseif/C
/A/else/A/elseif/C/txt ( cond7)
/A/else/A/elseif/T
/A/else/A/elseif/T/txt ( cond1 is false, 3 is false, 6 false, and 7 true)
/A/else/A/else
/A/else/A/else/decl (#else)
/A/else/A/else/txt ( cond1 is false, 3 is false, 6 false, and 7 false)
/A/else/A/endif (#endif)
/A/endif (#endif)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ(input.str(), document.data());
}
TEST(Halite, endif_error)
{
    std::stringstream input;
    input << "conditional template" << std::endl << "#endif";
    std::stringstream        error;
    DefaultHaliteInterpreter interpreter(error);
    ASSERT_FALSE(interpreter.parse(input));
    auto        document       = interpreter.root();
    std::string expected_paths = R"INPUT(/
/txt (conditional template)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ("conditional template", document.data());
    ASSERT_EQ("***Error : line 2 is an unmatched conditional terminator - "
              "'#endif'. The matching #if, #ifdef, #ifndef, #elseif, or #else "
              "is missing.\n",
              error.str());
}

TEST(Halite, elseif_error)
{
    std::stringstream input;
    input << "conditional template" << std::endl << "#elseif blah";
    std::stringstream        error;
    DefaultHaliteInterpreter interpreter(error);
    ASSERT_FALSE(interpreter.parse(input));
    auto        document       = interpreter.root();
    std::string expected_paths = R"INPUT(/
/txt (conditional template)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ("conditional template", document.data());
    ASSERT_EQ("***Error : line 2 is an unmatched conditional elseif. The "
              "matching #if, #ifdef, #ifndef, or #elseif is missing.\n",
              error.str());
}

TEST(Halite, else_error)
{
    std::stringstream input;
    input << "conditional template" << std::endl << std::endl << "#else blah";
    std::stringstream        error;
    DefaultHaliteInterpreter interpreter(error);
    ASSERT_FALSE(interpreter.parse(input));
    auto        document       = interpreter.root();
    std::string expected_paths = R"INPUT(/
/txt (conditional template)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    ASSERT_EQ("conditional template", document.data());
    ASSERT_EQ("***Error : line 3 is an unmatched conditional else. The "
              "matching #if, #ifdef, #ifndef, or #elseif is missing.\n",
              error.str());
}
