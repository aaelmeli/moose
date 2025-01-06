/*
 * File:   tstJSONView.cpp
 * Author: raq
 *
 * Created on July 28, 2018, 9:50 AM
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "waspjson/JSONInterpreter.h"
#include "waspjson/JSONNodeView.h"
#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(JSON, keyed_value)
{
    std::stringstream input;
    input << R"INPUT({"string" : "val",
                    "integer" : 10323,
                    "real" : 1.1232})INPUT";
    DefaultJSONInterpreter interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(17, interpreter.node_count());
    JSONNodeView document = interpreter.root();
    // { string , integer , real }
    ASSERT_EQ(7, document.child_count());
    std::string       expected_paths = R"INPUT(/
/{ ({)
/string
/string/decl ("string")
/string/: (:)
/string/value ("val")
/, (,)
/integer
/integer/decl ("integer")
/integer/: (:)
/integer/value (10323)
/, (,)
/real
/real/decl ("real")
/real/: (:)
/real/value (1.1232)
/} (})
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());

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
    auto children = document.non_decorative_children();
    // string, integer, real
    ASSERT_EQ(3, children.size());
    ASSERT_EQ(3, document.non_decorative_children_count());
    ASSERT_EQ("val", document.first_non_decorative_child_by_name("string").last_as_string());
}
