/*
 * File:   tstHaliteView.cpp
 * Author: raq
 *
 * Created on July 28, 2018, 9:50 AM
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "wasphalite/HaliteInterpreter.h"
#include "wasphalite/HaliteNodeView.h"
#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(Halite, keyed_value)
{
    std::stringstream input;
    input << R"INPUT(<name>
#ifdef 1
#elseif 2
#else 3
#endif
#import path
#repeat path
)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(26, interpreter.node_count());
    HaliteNodeView document = interpreter.root();
    //
    ASSERT_EQ(4, document.child_count());
    std::string       expected_paths = R"INPUT(/
/attr
/attr/< (<)
/attr/txt (name)
/attr/> (>)
/A
/A/ifdef
/A/ifdef/decl (#ifdef)
/A/ifdef/C
/A/ifdef/C/txt ( 1)
/A/ifdef/T
/A/elseif
/A/elseif/decl (#elseif)
/A/elseif/C
/A/elseif/C/txt ( 2)
/A/elseif/T
/A/else
/A/else/decl (#else)
/A/else/txt ( 3)
/A/endif (#endif)
/import
/import/decl (#import)
/import/txt ( path)
/repeat
/repeat/decl (#repeat)
/repeat/txt ( path)
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());

    auto children = document.non_decorative_children();
    // string, integer, real
    ASSERT_EQ(4, children.size());
    ASSERT_EQ(4, document.non_decorative_children_count());
    ASSERT_EQ("<name>", document.first_non_decorative_child_by_name("attr").id());
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
