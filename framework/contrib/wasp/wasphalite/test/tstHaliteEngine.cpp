/*
 * File:   tstHaliteEngine.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
// cstdio - std::remove(file_name)
#include <cstdio>
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "wasphalite/HaliteInterpreter.h"
#include "wasphalite/DataAccessor.h"
#include "waspcore/Object.h"

#include "wasphalite/test/Paths.h"

#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(Halite, single_attribute)
{
    {
        std::stringstream input;
        input << R"INPUT(<attribute>)INPUT";
        DefaultHaliteInterpreter interpreter;
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        DataAccessor      data;
        data.store("attribute", "value");
        ASSERT_EQ(Context::Type::STRING, data.type("attribute"));
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ("value", out.str());
    }
    {
        std::stringstream input;
        input << R"INPUT(<attribute:fmt=%-10.1e>)INPUT";
        DefaultHaliteInterpreter interpreter;
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        DataAccessor      data;
        data.store("attribute", 1e10);
        ASSERT_EQ(Context::Type::REAL, data.type("attribute"));
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ("1.0e+10   ", out.str());
    }
}
TEST(Halite, single_quoted_attribute)
{
    std::stringstream input;
    input << R"INPUT(<'attr with space( )'>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    data.store("attr with space( )", "value");
    ASSERT_EQ(Context::Type::STRING, data.type("attr with space( )"));
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("value", out.str());
}
TEST(Halite, single_abscent_optional_attribute)
{
    std::stringstream input;
    input << R"INPUT(b<attribute:?>a)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("ba", out.str());
}
TEST(Halite, single_present_optional_attribute)
{
    std::stringstream input;
    input << R"INPUT(b<attribute:?>a)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    data.store("attribute", 3.14159);
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("b3.14159a", out.str());
}
TEST(Halite, single_silent_attribute)
{
    std::stringstream input;
    input << R"INPUT(b<attribute=4:|>a)INPUT";
    DefaultHaliteInterpreter interpreter(std::cout);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;

    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(Context::Type::INTEGER, data.type("attribute"));
    ASSERT_EQ(4, data.integer("attribute"));
    ASSERT_EQ("ba", out.str());
}
TEST(Halite, single_present_formatted_optional_attribute)
{
    std::stringstream input;
    input << R"INPUT(b<attribute:?fmt=%6.2f>a)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    data.store("attribute", 3.14159);
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("b  3.14a", out.str());
}
TEST(Halite, formatted_single_attribute)
{
    std::stringstream input;
    input << R"INPUT(<pi:fmt=rounded=%12.8f>
<pi><nl>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    data.add_default_variables();
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("rounded=  3.14159265\n3.14159\n", out.str());
}
TEST(Halite, iterative_formatted_attribute_only_start)
{
    std::stringstream input;
    input << R"INPUT(<i:fmt=rounded=%3d; i=1>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("rounded=  1", out.str());
}
TEST(Halite, iterative_formatted_attribute_embedded)
{
    std::stringstream input;
    input << R"INPUT(<i:fmt=%3d; i=<start>,<end>>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    data.store("start", 1);
    data.store("end", 2);
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("  1   2", out.str());
}
TEST(Halite, iterative_formatted_attribute_only_start_end)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i:fmt=%2d; i=1,3>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(" 1  2  3", out.str());
}
TEST(Halite, iterative_formatted_attribute)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i:fmt=%2d; i=1,3,2>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(" 1  3", out.str());
}
TEST(Halite, iterative_formatted_attribute_sep)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i:fmt=%2d;i=1,3,2;sep=,>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(" 1, 3", out.str());
}
TEST(Halite, iterative_formatted_sep_attribute)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i:fmt=%2d;sep=s;i=1,3,2;>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(" 1s 3", out.str());
}
TEST(Halite, iterative_formatted_sep_first_attribute)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i:sep=*;fmt=%2d;i=1,3,2;>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(" 1* 3", out.str());
}
TEST(Halite, iterative_formatted_attribute_multi_only_start)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i*j:j=3; fmt=%2d; i=1,3,2>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(" 3  9", out.str());
}
TEST(Halite, iterative_formatted_attribute_multi_only_start_end)
{
    std::stringstream input;
    // default separator is a whitespace making %2 behave like %3
    input << R"INPUT(<i*j: j=3,5; i=1,3,2; fmt=%2d; >)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ(" 3  9  4 12  5 15", out.str());
}
TEST(Halite, attribute_use_scope_object)
{
    std::stringstream input;
    // access to x,y,z are restricted to obj
    // use obj scope to facilitate access
    input << R"INPUT(<y+"-"+x*z: use=obj >)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["obj"]      = DataObject();
    o["obj"]["x"] = 2;
    o["obj"]["y"] = "ted";
    o["obj"]["z"] = 10.22;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("ted-20.440000", out.str());
}
TEST(Halite, attribute_object_hierarchy_reference)
{
    std::stringstream input;
    // access to x,y,z under obj
    input << R"INPUT(<obj.y+"-"+obj.x*obj.z>)INPUT";
    HaliteInterpreter<> interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o, nullptr, ".");
    o["obj"]      = DataObject();
    o["obj"]["x"] = 2;
    o["obj"]["y"] = "ted";
    o["obj"]["z"] = 10.22;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("ted-20.440000", out.str());
}
TEST(Halite, attribute_use_scope_array_optional)
{
    std::stringstream input;
    // access to x,y,z are restricted to each element of an array
    // use arrayscope to facilitate access
    input << R"INPUT(before<y+"-"+x*z:? use=array >after)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("beforeafter", out.str());
}
TEST(Halite, attribute_use_scope_array)
{
    std::stringstream input;
    // access to x,y,z are restricted to each element of an array
    // use arrayscope to facilitate access
    input << R"INPUT(<y+"-"+x*z: use=array >)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["array"]         = DataArray();
    o["array"][0]      = DataObject();
    o["array"][0]["x"] = 2;
    o["array"][0]["y"] = "fred";
    o["array"][0]["z"] = 10.22;
    o["array"][1]      = DataObject();
    o["array"][1]["x"] = 4;
    o["array"][1]["y"] = "ted";
    o["array"][1]["z"] = 10.22;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("fred-20.440000 ted-40.880000", out.str());
}
TEST(Halite, hierarchical_attribute_use_scope_array)
{
    std::stringstream input;
    // access to x,y,z are restricted to each element of an array
    // use arrayscope to facilitate access
    input << R"INPUT(<obj.value: use=array>)INPUT";
    HaliteInterpreter<> interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o, nullptr, ".");
    o["array"]         = DataArray();
    o["array"][0]      = DataObject();
    o["array"][0]["obj"]= DataObject();
    o["array"][0]["obj"]["value"] = 2;
    o["array"][1]      = DataObject();
    o["array"][1]["obj"]= DataObject();
    o["array"][1]["obj"]["value"] = 4;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("2 4", out.str());
}

TEST(Halite, iterative_attribute_use_scope_object)
{
    std::stringstream input;
    // access to x,y,z are restricted to obj
    // use obj scope to facilitate access
    input << R"INPUT(<y+"-"+(x*z+i): use=obj; i=0,1;sep=,>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["obj"]      = DataObject();
    o["obj"]["x"] = 2;
    o["obj"]["y"] = "ted";
    o["obj"]["z"] = 10.22;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("ted-20.440000,ted-21.440000", out.str());
}
TEST(Halite, iterative_attribute_hierarchy_object)
{
    std::stringstream input;
    // access to x,y,z are restricted to obj
    // use obj scope to facilitate access
    input << R"INPUT(<obj%y+"-"+(obj%x*obj%z+i): i=0,1;sep=,>)INPUT";
    HaliteInterpreter<> interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o, nullptr, "%");
    o["obj"]      = DataObject();
    o["obj"]["x"] = 2;
    o["obj"]["y"] = "ted";
    o["obj"]["z"] = 10.22;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    ASSERT_EQ("ted-20.440000,ted-21.440000", out.str());
}
TEST(Halite, indirect_attribute)
{
    std::stringstream input;
    input << R"INPUT(<<attr>> <<what>.<member>>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["attr"]   = std::string("what");
    o["what"]   = std::string("obj");
    o["member"] = std::string("x");
    o["obj.x"]  = 3.14159;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj 3.14159", out.str());
}
TEST(Halite, indirect_hierarchical_attribute)
{
    std::stringstream input;
    input << R"INPUT(<<attr>> <<what>.<member>:fmt=%.2f>)INPUT";
    HaliteInterpreter<> interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o,nullptr,".");
    o["attr"]   = std::string("what");
    o["what"]   = std::string("obj");
    o["member"] = std::string("x");
    o["obj.x"]  = 3.14159;
    o["obj"]    = DataObject();
    o["obj"]["x"] = 2.1;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj 2.10", out.str());
}
TEST(Halite, indirect_fake_hierarchical_attribute)
{
    std::stringstream input;
    input << R"INPUT(<<attr>> <<what>.<member>:fmt=%.4f>)INPUT";
    HaliteInterpreter<> interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o,nullptr,".");
    o["attr"]   = std::string("what");
    o["what"]   = std::string("obj");
    o["member"] = std::string("x");
    o["obj.x"]  = 3.14159; // <--- FAKE hierarchy... still works
//    o["obj"]    = DataObject();
//    o["obj"]["x"] = 2.1;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj 3.1416", out.str());
}
TEST(Halite, indirect_attribute_delim_output)
{
    std::stringstream input;
    input << R"INPUT(<<attr>> <_E_><_S_> <<what>.<member>>)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["attr"]   = std::string("what");
    o["what"]   = std::string("obj");
    o["member"] = std::string("x");
    o["obj.x"]  = 3.14159;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj >< 3.14159", out.str());
}
TEST(Halite, indirect_attribute_delim)
{
    std::stringstream input;
    input << R"INPUT({{attr}} {{what}.{member}})INPUT";
    DefaultHaliteInterpreter interpreter;
    interpreter.attr_start_delim() = "{";
    interpreter.attr_end_delim()   = "}";
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["attr"]   = std::string("what");
    o["what"]   = std::string("obj");
    o["member"] = std::string("x");
    o["obj.x"]  = 3.14159;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj 3.14159", out.str());
}

// Ensure that imported templates use the delimiters
// specified in the parent
TEST(Halite, inherited_attribute_delim)
{
    std::ofstream     import("inherited_delim.tmpl");
    import << "<<attr>>";
    import.close();
    std::stringstream input;
    input << "#import inherited_delim.tmpl";
    DefaultHaliteInterpreter interpreter;
    interpreter.attr_start_delim() = "<<";
    interpreter.attr_end_delim()   = ">>";
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["attr"]   = std::string("what");
    EXPECT_TRUE(interpreter.evaluate(out, data));
    EXPECT_EQ("what", out.str());
    std::remove("inherited_delim.tmpl");
}

TEST(Halite, indirect_attribute_preceeding_static)
{
    std::stringstream input;
    input << R"INPUT(line1
<jumperof<what>>
line3)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["jumperofhurdles"] = std::string("olympian");
    o["what"]            = std::string("hurdles");
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("line1\nolympian\nline3", out.str());
}
TEST(Halite, indirect_attribute_preceeding_static2)
{
    std::stringstream input;
    input << R"INPUT(line1
line2<jumperof<what>>
line3)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["jumperofhurdles"] = std::string("olympian");
    o["what"]            = std::string("hurdles");
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("line1\nline2olympian\nline3", out.str());
}
TEST(Halite, indirect_attribute_non_uniform_delim)
{
    std::stringstream input;
    input << R"INPUT(${${attr}} ${${what}.${member}} ${value:fmt=%d})INPUT";
    DefaultHaliteInterpreter interpreter;
    interpreter.attr_start_delim() = "${";
    interpreter.attr_end_delim()   = "}";
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["attr"]   = "what";
    o["what"]   = "obj";
    o["member"] = "x";
    o["obj.x"]  = 3.14159;
    o["value"]  = o["obj.x"];
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj 3.14159 3", out.str());
}
/**
 * @brief TEST that non-uniform delimiters are output
 */
TEST(Halite, indirect_attribute_non_uniform_delim_output)
{
    std::stringstream input;
    input
        << R"INPUT(${${attr}} ${${what}.${member}} ${_S_}${_E_} ${value:fmt=%d})INPUT";
    DefaultHaliteInterpreter interpreter;
    interpreter.attr_start_delim() = "${";
    interpreter.attr_end_delim()   = "}";
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["attr"]   = "what";
    o["what"]   = "obj";
    o["member"] = "x";
    o["obj.x"]  = 3.14159;
    o["value"]  = o["obj.x"];
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ("obj 3.14159 ${} 3", out.str());
}
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
    std::stringstream out;
    DataAccessor      data;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(input.str(), out.str());
}
TEST(Halite, parameterized_text)
{
    std::stringstream input;
    input << R"INPUT(This is plain test
 < pi >  <pi>
 <pi>
   line
            )INPUT";
    std::stringstream expected;
    expected << R"INPUT(This is plain test
 3.14159  3.14159
 3.14159
   line
            )INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataAccessor      data;
    data.add_default_variables();
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(expected.str(), out.str());
}

TEST(Halite, array_access)
{
    std::stringstream input;
    input << R"INPUT( <var> "<str[0]>" <str[1]>
<int[0]> <int[1]>
 <double[0]> <double[1]>
<bool[0]> <bool[1]>

<mixed[0]> <mixed[1]> <mixed[2]:fmt=%.6f> <mixed[3]>
<mixed[i]:i=0,<size(mixed)-1>>)INPUT";
    std::stringstream expected;
    expected << R"INPUT( 1 "ted" fed
10 20
 1.1 3.14159
0 1

0 19 500.123123 dead ted
0 19 500.123 dead ted)INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["var"]       = true;
    o["str"]       = DataArray();
    o["str"][0]    = "ted";
    o["str"][1]    = "fed";
    o["int"]       = DataArray();
    o["int"][0]    = 10;
    o["int"][1]    = 20;
    o["double"]    = DataArray();
    o["double"][0] = 1.1;
    o["double"][1] = 3.14159;
    o["bool"]      = DataArray();
    o["bool"][0]   = false;
    o["bool"][1]   = true;

    o["mixed"]    = DataArray();
    o["mixed"][0] = false;
    o["mixed"][1] = 19;
    o["mixed"][2] = 500.123123;
    o["mixed"][3] = std::string("dead ted");
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(expected.str(), out.str());
}
/**
 * @brief test attribute acquisition using DataAccessor layer
 */
TEST(Halite, parameterized_text_data_accessed)
{
    std::stringstream input;
    input << R"INPUT(This is plain test
 < ted >  <fred>
 <x>
   line
            )INPUT";
    std::stringstream expected;
    expected << R"INPUT(This is plain test
 wombat  3.14159
 2
   line
            )INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["ted"]  = "wombat";
    o["fred"] = 3.14159;
    o["x"]    = 2;
    ASSERT_TRUE(interpreter.evaluate(out, data));
    ASSERT_EQ(expected.str(), out.str());
}

/**
 * @brief test attribute usage from nested template
 * This ensures the data layer is properly propogated
 * to the nested halite engine
 */
TEST(Halite, attributed_text_fileimport)
{
    std::ofstream     import("nested parameterized template.tmpl");
    std::stringstream content;
    content << "this is" << std::endl
            << "nested files using <ted>" << std::endl
            << std::endl  // empty line
            << "and assigning fred to < fred = x >" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(#import ./nested parameterized template.tmpl
<ted> has <fred> brothers
)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["ted"]  = "wombat";
    o["fred"] = 3.14159;
    ASSERT_EQ(Value::TYPE_STRING, o["ted"].type());
    ASSERT_EQ(Value::TYPE_DOUBLE, o["fred"].type());
    o["x"] = 2;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(this is
nested files using wombat

and assigning fred to 2
wombat has 2 brothers
)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("nested parameterized template.tmpl");
    ASSERT_EQ(Value::TYPE_INTEGER, o["fred"].type());
    ASSERT_EQ(2, o["fred"].to_int());
}

/**
 * @brief test file repeat (single) iteration
 */
TEST(Halite, repeat_fileimport)
{
    std::ofstream     import("nested template.tmpl");
    std::stringstream content;
    content << "this is" << std::endl
            << "nested files using nothing" << std::endl
            << std::endl  // empty line
            << "and assigning fred to < fred = 3.14159 >" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
            Some preceeding text
#repeat ./nested template.tmpl
no iteration. Fred = <fred>)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
            Some preceeding text
this is
nested files using nothing

and assigning fred to 3.14159
no iteration. Fred = 3.14159)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("nested template.tmpl");
    ASSERT_EQ(Value::TYPE_DOUBLE, o["fred"].type());
    ASSERT_EQ(3.14159, o["fred"].to_double());
}
TEST(Halite, range)
{
    std::vector<DefaultHaliteInterpreter::Range> expected_imports = {
        {"x", 1, 1, 1}, {"x", 1, 1, 1}, {"x", 1, 2, 1},
        {"x", 1, 2, 1}, {"x", 1, 3, 2}, {"x", 6, 6, 1}};
    std::vector<std::string> ranges = {
        " x=1", "x = 1", " x=1,2", " x = 1 , 2 ", " x = 1 , 3, 2 ", " x = 6"};
    ASSERT_EQ(expected_imports.size(), ranges.size());
    for (size_t i = 0; i < expected_imports.size(); ++i)
    {
        SCOPED_TRACE(i);
        SCOPED_TRACE(ranges[i]);
        std::vector<DefaultHaliteInterpreter::Range> imports;
        std::string                                  range = ranges[i];
        std::string                                  error;
        EXPECT_TRUE(
            DefaultHaliteInterpreter::extract_ranges(range, imports, error));
        std::cout << error << std::endl;
        ASSERT_FALSE(imports.empty());
        EXPECT_EQ(expected_imports[i].name, imports.front().name);
        EXPECT_EQ(expected_imports[i].start, imports.front().start);
        EXPECT_EQ(expected_imports[i].end, imports.front().end);
        EXPECT_EQ(expected_imports[i].stride, imports.front().stride);
    }
}
TEST(Halite, multi_range)
{
    std::vector<std::vector<DefaultHaliteInterpreter::Range>> expected_imports =
        {{{"x", 1, 1, 1}, {"y", 1, 1, 1}},
         {{"x", 1, 2, 1}, {"y", 2, 2, 1}},
         {{"x", 1, 2, 3}, {"y", 3, 3, 1}},
         {{"x", 1, 2, 1}, {"y", 4, 5, 1}},
         {{"x", 1, 3, 2}, {"y", 6, 12, 3}},
         {{"x", 1, 3, 2}, {"y", 7, 7, 1}, {"z", 3, 3, 1}}};
    std::vector<std::string> ranges = {" x=1;y=1",
                                       "x = 1,2;y=2",
                                       " x=1,2,3;y=3",
                                       " x = 1 , 2 ; y = 4, 5",
                                       " x = 1 , 3, 2 ; y= 6, 12, 3",
                                       " x = 1 , 3, 2 ; y = 7; z=3"};
    ASSERT_EQ(expected_imports.size(), ranges.size());
    for (size_t i = 0; i < expected_imports.size(); ++i)
    {
        SCOPED_TRACE(i);
        SCOPED_TRACE(ranges[i]);
        std::vector<DefaultHaliteInterpreter::Range> imports;
        std::string                                  range = ranges[i];
        std::string                                  error;
        EXPECT_TRUE(
            DefaultHaliteInterpreter::extract_ranges(range, imports, error));
        std::cout << error << std::endl;
        ASSERT_FALSE(imports.empty());
        ASSERT_EQ(expected_imports[i].size(), imports.size());

        for (size_t j = 0; j < expected_imports[i].size(); ++j)
        {
            EXPECT_EQ(expected_imports[i][j].name, imports[j].name);
            EXPECT_EQ(expected_imports[i][j].start, imports[j].start);
            EXPECT_EQ(expected_imports[i][j].end, imports[j].end);
            EXPECT_EQ(expected_imports[i][j].stride, imports[j].stride);
        }
    }
}

/**
 * @brief test file repeat (single) iteration
 */
TEST(Halite, repeat_fileimport_using_single_range)
{
    std::ofstream     import("nested repeated template.tmpl");
    std::stringstream content;
    content << "i=<i>" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
            Some preceeding text
#repeat ./nested repeated template.tmpl using i=1,3

text that follows)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
            Some preceeding text
i=1
i=2
i=3

text that follows)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("nested repeated template.tmpl");
}
/**
 * @brief test file repeat (single) iteration
 */
TEST(Halite, repeat_fileimport_using_multiple_range)
{
    std::ofstream     import("nested repeated template.tmpl");
    std::stringstream content;
    content << "i=<i> j=<j> k=<k>" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
            Some preceeding text
#repeat ./nested repeated template.tmpl using i=1,3; j=1,1; k=0,2,2

text that follows)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
            Some preceeding text
i=1 j=1 k=0
i=1 j=1 k=2
i=2 j=1 k=0
i=2 j=1 k=2
i=3 j=1 k=0
i=3 j=1 k=2

text that follows)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("nested repeated template.tmpl");
}
TEST(Halite, file_import_using_object_by_name)
{
    std::ofstream     import("import_by_name_template.tmpl");
    std::stringstream content;
    content << "this is" << std::endl
            << "nested files using ted's foo (<foo>)" << std::endl
            << std::endl  // empty line
            << "and assigning foo to < foo = 9 > and p (<p>) to <p=4>"
            << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
    text prior
#import ./import_by_name_template.tmpl using ted
 text after)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["p"]          = 3;
    o["ted"]        = DataObject();
    o["ted"]["foo"] = std::string("bar");
    ASSERT_EQ(Value::TYPE_OBJECT, o["ted"].type());
    ASSERT_EQ(Value::TYPE_STRING, o["ted"]["foo"].type());
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
    text prior
this is
nested files using ted's foo (bar)

and assigning foo to 9 and p (3) to 4
 text after)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("import_by_name_template.tmpl");
    ASSERT_EQ(Value::TYPE_INTEGER, o["p"].type());
    ASSERT_EQ(3, o["p"].to_int());  // didnt change!!!
    // ensure the child was passed by reference (can be changed)
    ASSERT_EQ(Value::TYPE_INTEGER, o["ted"]["foo"].type());
    ASSERT_EQ(9, o["ted"]["foo"].to_int());
}
TEST(Halite, file_import_using_object_by_copy)
{
    std::ofstream     import("import_by_name_template.tmpl");
    std::stringstream content;
    content << "this is" << std::endl
            << "nested files using ted's foo (<foo:fmt=%-10.1e>)" << std::endl
            << std::endl  // empty line
            << "and assigning foo to < foo = 9 >" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
    text prior
#import ./import_by_name_template.tmpl using <ted>
 text after
)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["ted"]        = DataObject();
    o["ted"]["foo"] = 1.0e+10;
    ASSERT_EQ(Value::TYPE_OBJECT, o["ted"].type());
    //    ASSERT_EQ( Value::TYPE_STRING, o["ted"]["foo"].type() );
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
    text prior
this is
nested files using ted's foo (1.0e+10   )

and assigning foo to 9
 text after
)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("import_by_name_template.tmpl");
    // ensure the child was passed by copy (cannot be changed)
    //    ASSERT_EQ( Value::TYPE_STRING, o["ted"]["foo"].type() );
    //    ASSERT_EQ( "bar", o["ted"]["foo"].to_string() );
}
TEST(Halite, file_import_using_inline)
{
    std::ofstream     import("import_by_name_inline.tmpl");
    std::stringstream content;
    content << "this is" << std::endl
            << "nested files using ted's foo (<foo>)" << std::endl
            << std::endl  // empty line
            << "and assigning foo to < foo = 9 >" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
    text prior
#import ./import_by_name_inline.tmpl using {"foo":"bar"}
 text after
)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
    text prior
this is
nested files using ted's foo (bar)

and assigning foo to 9
 text after
)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("import_by_name_inline.tmpl");
    // ensure the variable is not present
    ASSERT_FALSE(data.exists("foo"));
}
TEST(Halite, file_import_using_array_by_name)
{
    std::ofstream     import("import_by_name_iterative.tmpl");
    std::stringstream content;
    content << "nested template" << std::endl
            << "using parameter <name>" << std::endl
            << "with value <value>" << std::endl
            << "and assigning value its own name <value=name>" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(
    text prior
#import ./import_by_name_iterative.tmpl using my_array
 text after
)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    // template line counts always include 1 additional line
    ASSERT_EQ(5, interpreter.line_count());
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["my_array"]             = DataArray();
    o["my_array"][0]          = DataObject();
    o["my_array"][0]["name"]  = "ted1";
    o["my_array"][0]["value"] = 1;

    o["my_array"][1]          = DataObject();
    o["my_array"][1]["name"]  = "ted2";
    o["my_array"][1]["value"] = 2.2;

    o["my_array"][2]          = DataObject();
    o["my_array"][2]["name"]  = "ted3";
    o["my_array"][2]["value"] = "fred";
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(
    text prior
nested template
using parameter ted1
with value 1
and assigning value its own name ted1
nested template
using parameter ted2
with value 2.2
and assigning value its own name ted2
nested template
using parameter ted3
with value fred
and assigning value its own name ted3
 text after
)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("import_by_name_iterative.tmpl");
    // ensure the child was passed by reference (can be changed)
    ASSERT_EQ(Value::TYPE_STRING, o["my_array"][0]["value"].type());
    ASSERT_EQ("ted1", o["my_array"][0]["value"].to_string());
}

TEST(Halite, parent_scope_var_use_in_fileimport)
{
    std::ofstream     import("parent_scope_var_use.tmpl");
    std::stringstream content;
    content << "<'var name':use=pobj>" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << "<'var name':use=pobj;fmt=%4d>" << std::endl;
    input << "#import ./parent_scope_var_use.tmpl using p2obj" << std::endl;

    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        root;
    DataObject        pobj;
    DataObject        p2obj;
    DataAccessor      data(&root);
    pobj["var name"]   = 3.14;
    root["pobj"]       = pobj;
    p2obj["var2 name"] = 6.28;
    root["p2obj"]      = p2obj;
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << "   3" << std::endl << "3.14" << std::endl;
    ASSERT_EQ(expected.str(), out.str());
    std::remove("parent_scope_var_use.tmpl");
}
/**
 * @brief test attribute usage from nested template where nested path is
 * parameterized
 */
TEST(Halite, parameterized_fileimport)
{
    std::ofstream     import("nested attr wombat.tmpl");
    std::stringstream content;
    content << "this is" << std::endl
            << "nested files using <ted>" << std::endl
            << std::endl  // empty line
            << "and assigning fred to < fred = x >" << std::endl;
    import << content.str();
    import.close();
    std::stringstream input;
    input << R"INPUT(#import ./nested <x> <ted>.tmpl
<ted> has <fred> brothers)INPUT";
    DefaultHaliteInterpreter interpreter;

    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream out;
    DataObject        o;
    DataAccessor      data(&o);
    o["ted"]  = "wombat";
    o["fred"] = 3.14159;
    ASSERT_EQ(Value::TYPE_STRING, o["ted"].type());
    ASSERT_EQ(Value::TYPE_DOUBLE, o["fred"].type());
    o["x"] = "attr";
    ASSERT_TRUE(interpreter.evaluate(out, data));

    std::stringstream expected;
    expected << R"INPUT(this is
nested files using wombat

and assigning fred to attr
wombat has attr brothers)INPUT";
    ASSERT_EQ(expected.str(), out.str());
    std::remove("nested attr wombat.tmpl");
    ASSERT_EQ(Value::TYPE_STRING, o["fred"].type());
    ASSERT_EQ("attr", o["fred"].to_string());
}

TEST(Halite, simple_conditional)
{
    std::stringstream input;
    input << R"INPUT(#ifdef pi


<pi> is defined as pi math constant


#else

 some else statement


text



#endif
)INPUT";

    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    {  // test defined path through template
        std::stringstream expected;
        expected << R"INPUT(

3.14159 is defined as pi math constant


)INPUT";
        std::stringstream out;
        DataObject        o;
        o["pi"] = 3.14159;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test defined path through template
        std::stringstream expected;
        expected << R"INPUT(
 some else statement


text



)INPUT";
        std::stringstream out;
        DataObject        o;
        DataAccessor      data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
}
TEST(Halite, nested_conditional)
{
    std::stringstream input;
    input << R"INPUT(#ifdef pi

#if <<pi:fmt=%.0f>==3>
<pi> is defined as pi math constant
#endif

#else

 some else statement

#ifndef pi
text
#endif


#endif)INPUT";

    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    {  // test defined path through template
        std::stringstream expected;
        expected << R"INPUT(
3.14159 is defined as pi math constant

)INPUT";
        std::stringstream out;
        DataObject        o;
        o["pi"] = 3.14159;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test defined path through template
        std::stringstream expected;
        expected << R"INPUT(
 some else statement

text


)INPUT";
        std::stringstream out;
        DataObject        o;
        DataAccessor      data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
}

/**
 * @brief test conditional blocks
 */
TEST(Halite, conditional_text_data_accessed)
{
    std::stringstream input;
    input << R"INPUT(#ifdef x
x is defined and has a value of <x>
#elseif    <defined(y)>
x is not defined,
 but y is defined with value as an int of <y:fmt=%.0f>
#else
x and y are not defined
#endif
   line
            )INPUT";
    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    {  // test defined path through template
        std::stringstream expected;
        expected << R"INPUT(x is defined and has a value of 3.14159
   line
            )INPUT";
        std::stringstream out;
        DataObject        o;
        o["x"] = 3.14159;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }

    {  // test elseif  path through template
        std::stringstream expected;
        expected << R"INPUT(x is not defined,
 but y is defined with value as an int of 7
   line
            )INPUT";
        std::stringstream out;
        DataObject        o;
        o["y"] = 7.14159;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test undefined path through template
        std::stringstream expected;
        expected << R"INPUT(x and y are not defined
   line
            )INPUT";
        std::stringstream out;
        DataObject        o;
        DataAccessor      data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
}

/**
 * @brief TEST issue discovered by Jordan Lefebvre
 */
TEST(Halite, simple_if_issue_52)
{
    std::stringstream input;
    input << R"INPUT(#if <var==1>
action1
#endif
#if <var==2>
action2
#endif
#if <var==3>
action3
#endif
)INPUT";

    DefaultHaliteInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    {  // test undefined path through template
        std::stringstream expected;
        expected << R"INPUT()INPUT";
        std::stringstream out;
        DataObject        o;
        o["var"] = 0;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test defined path action1 through template
        std::stringstream expected;
        expected << R"INPUT(action1
)INPUT";
        std::stringstream out;
        DataObject        o;
        o["var"] = 1;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test defined path action2 through template
        std::stringstream expected;
        expected << R"INPUT(action2
)INPUT";
        std::stringstream out;
        DataObject        o;
        o["var"] = 2;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test defined path actoin3 through template
        std::stringstream expected;
        expected << R"INPUT(action3
)INPUT";
        std::stringstream out;
        DataObject        o;
        o["var"] = 3;
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
}

/**
 * @brief TEST test that the emission of a configurable delimiter functions as
 * expected
 * The configurable delimiter allows for fixed width situations where
 * at a given index stride a delimiter must be emitted, typically a newline.
 */
TEST(Halite, iterative_configurable_delimiter_emission)
{
    DataObject   o;
    DataAccessor data(&o);
    data.add_default_variables();  // newline
    o["times"]    = DataArray();
    o["times"][0] = 1;
    o["times"][1] = 2;
    o["times"][2] = 3;
    o["times"][3] = 4;
    o["times"][4] = 5;
    {  // test 5th element emission. No emission occurs
        std::stringstream input;
        input
            << R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep= ;emit=<nl>,5>)INPUT";
        DefaultHaliteInterpreter interpreter;
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        ASSERT_TRUE(interpreter.evaluate(out, data));
        std::stringstream expected;
        expected << "1 2 3 4 5";
        ASSERT_EQ(expected.str(), out.str());
    }
    //    { // test 5th element emission where line has newline. No emission
    //    occurs
    //        std::stringstream input;
    //        input<< R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep=
    //        ;emit=<nl>,5>
    //)INPUT";
    //        DefaultHaliteInterpreter interpreter;
    //        ASSERT_TRUE( interpreter.parse(input) );
    //        std::stringstream out;
    //        ASSERT_TRUE( interpreter.evaluate(out,data) );
    //        std::stringstream expected;
    //        expected<<"1 2 3 4 5"<<std::endl;
    //        ASSERT_EQ( expected.str(), out.str() );
    //    }
    {  // test 4th element emission. Emission occurs with an additional newline
        // and no separator on new line
        std::stringstream input;
        input
            << R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep= ;emit=<nl>,4>)INPUT";
        DefaultHaliteInterpreter interpreter;
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        ASSERT_TRUE(interpreter.evaluate(out, data));
        std::stringstream expected;
        expected << "1 2 3 4" << std::endl << "5";
        EXPECT_EQ(expected.str(), out.str());
    }
    //    { // test 4th element emission where line has newline. Emission occurs
    //    with an additional newline and no separator on new line
    //        std::stringstream input;
    //        input<< R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep=
    //        ;emit=<nl>,4>
    //)INPUT";
    //        DefaultHaliteInterpreter interpreter;
    //        ASSERT_TRUE( interpreter.parse(input) );
    //        std::stringstream out;
    //        ASSERT_TRUE( interpreter.evaluate(out,data) );
    //        std::stringstream expected;
    //        expected<<"1 2 3 4"<<std::endl<<"5"<<std::endl;
    //        EXPECT_EQ( expected.str(), out.str() );
    //    }
    {  // test 6th element emission. No emission occurs
        std::stringstream input;
        input
            << R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep= ;emit=<nl>,6>)INPUT";
        DefaultHaliteInterpreter interpreter;
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        ASSERT_TRUE(interpreter.evaluate(out, data));
        std::stringstream expected;
        expected << "1 2 3 4 5";
        ASSERT_EQ(expected.str(), out.str());
    }
    {  // test fixed column block
        std::stringstream input;
        input << R"INPUT(<j:j=10,0,-1;fmt=%2d;sep=;emit=# comment<nl>,2>)INPUT";
        DefaultHaliteInterpreter interpreter;
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        ASSERT_TRUE(interpreter.evaluate(out, data));
        std::stringstream expected;
        expected << "10 9# comment" << std::endl
                 << " 8 7# comment" << std::endl
                 << " 6 5# comment" << std::endl
                 << " 4 3# comment" << std::endl
                 << " 2 1# comment" << std::endl
                 << " 0";
        ASSERT_EQ(expected.str(), out.str());
    }
    //    { // test 6th element emission where line has newline. No emission
    //    occurs
    //        std::stringstream input;
    //        input<< R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep=
    //        ;emit=<nl>,6>
    //)INPUT";
    //        DefaultHaliteInterpreter interpreter;
    //        ASSERT_TRUE( interpreter.parse(input) );
    //        std::stringstream out;
    //        ASSERT_TRUE( interpreter.evaluate(out,data) );
    //        std::stringstream expected;
    //        expected<<"1 2 3 4 5"<<std::endl;
    //        ASSERT_EQ( expected.str(), out.str() );
    //    }

    {  // test error of invalid range
        std::stringstream input;
        input
            << R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep= ;emit=<nl>,-5>)INPUT";
        std::stringstream        errors;
        DefaultHaliteInterpreter interpreter(errors);
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        ASSERT_FALSE(interpreter.evaluate(out, data));
        std::stringstream expected;
        expected << "***Error : the emit stride on line 1 must be a "
                    "non-negative integer. Found '-5' processed as '-5'."
                 << std::endl;
        ASSERT_EQ(expected.str(), errors.str());
    }
    {  // test error of fliped args
        std::stringstream input;
        input
            << R"INPUT(<times[j]:j=0,<size(times)-1>;fmt=%g;sep= ;emit=5,ted>)INPUT";
        std::stringstream        errors;
        DefaultHaliteInterpreter interpreter(errors);
        ASSERT_TRUE(interpreter.parse(input));
        std::stringstream out;
        ASSERT_FALSE(interpreter.evaluate(out, data));
        std::stringstream expected;
        expected << "***Error : the emit stride failed to be processed on line "
                    "1. Looking for a non-negative integer, found 'ted'."
                 << std::endl;
        ASSERT_EQ(expected.str(), errors.str());
    }
}

/**
 * @brief test conditional blocks
 */
TEST(Halite, simple_parameterized_conditional)
{
    std::stringstream input;
    input << "#if     <InputType == \"key_value\">" << std::endl
          << "<InputName> = <InputValue>" << std::endl
          << "#elseif <InputType == \"key_array\">" << std::endl
          << "<InputName> = ' <InputValue> '" << std::endl
          << "#endif" << std::endl;
    HaliteInterpreter<> interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    // template line count is always +1 to account for trailing eof newline
    ASSERT_EQ(6, interpreter.line_count());
    {  // test defined path through template
        std::stringstream expected;
        expected << "X = Y" << std::endl;
        std::stringstream out;
        DataObject        o;
        o["InputType"]  = std::string("key_value");
        o["InputName"]  = std::string("X");
        o["InputValue"] = std::string("Y");
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }

    {  // test elseif  path through template
        std::stringstream expected;
        expected << "Y = ' ARRAY '" << std::endl;
        std::stringstream out;
        DataObject        o;
        o["InputType"]  = std::string("key_array");
        o["InputName"]  = std::string("Y");
        o["InputValue"] = std::string("ARRAY");
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
    {                                // test undefined path through template
        std::stringstream expected;  // empty string
        std::stringstream out;
        DataObject        o;
        o["InputType"] = std::string("undefined");
        DataAccessor data(&o);
        ASSERT_TRUE(interpreter.evaluate(out, data));
        ASSERT_EQ(expected.str(), out.str());
    }
}

TEST(Halite, non_default_attribute_delim)
{
    std::string template_file =
        wasp::dir_name(SOURCE_DIR+"/") + "/data/non_default_attribute_delim.tmpl";
    std::string json_data_file =
        wasp::dir_name(SOURCE_DIR+"/") + "/data/non_default_attribute_delim.json";

    std::stringstream output_stream;

    bool template_result =
        wasp::expand_template(output_stream, std::cerr, std::cerr,
                              template_file, json_data_file, true, true,
                              "{","}",".");

    ASSERT_TRUE(template_result);
    std::stringstream expected_ss;
    expected_ss << "here is an" <<std::endl
                <<std::endl
                <<"attribute"<<std::endl
                <<std::endl
                <<"that is 3.14159"<<std::endl;

    EXPECT_EQ(expected_ss.str(), output_stream.str());
}
/**
 * @brief TEST file repeated importing a subtemplate
 *  This exercises the newline emission logic for templates
 * that consist solely of subtemplate
 */
TEST(Halite, nested_repeated_file)
{
    std::string template_file =
        wasp::dir_name(SOURCE_DIR+"/") + "/data/array_sub_one.tmpl";
    std::string json_data_file =
        wasp::dir_name(SOURCE_DIR+"/") + "/data/array_sub_one.json";

    std::stringstream output_stream;

    bool template_result =
        wasp::expand_template(output_stream, std::cerr, std::cerr,
                              template_file, json_data_file, true, true);

    ASSERT_TRUE(template_result);
    std::stringstream expected_ss;
    expected_ss << "' z = 1 " << std::endl
                << "    3 3" << std::endl
                << "    3 3" << std::endl
                << "    3 3" << std::endl;

    EXPECT_EQ(expected_ss.str(), output_stream.str());
}

/**
 * @brief TEST file repeated importing a subtemplate
 *  This exercises the newline emission logic for templates
 * that consist solely of subtemplate but also contain trailing text, which
 * test the newline emission logic surrounding data before and after repeats
 */
TEST(Halite, nested_repeated_file_with_trailing_content)
{
    std::string template_file =
        wasp::dir_name(SOURCE_DIR+"/") + "/data/array_sub_one_trailing.tmpl";
    std::string json_data_file =
        wasp::dir_name(SOURCE_DIR+"/") + "/data/array_sub_one.json";

    std::stringstream output_stream;

    bool template_result =
        wasp::expand_template(output_stream, std::cerr, std::cerr,
                              template_file, json_data_file, true, true);

    ASSERT_TRUE(template_result);
    std::stringstream expected_ss;
    expected_ss << "' z = 1 " << std::endl
                << "    3 3" << std::endl
                << "    3 3" << std::endl
                << "    3 3" << std::endl
                << "' end of level 1 of z" << std::endl;

    EXPECT_EQ(expected_ss.str(), output_stream.str());
}

/**
 * @brief TEST file repeated importing a subtemplate with zero repeats
 *  This exercises the newline emission logic for templates
 * that consist of zero-repeated subtemplate, trailing text, which
 * test the newline emission logic surrounding data before and after
 * zero-repeats
 */
TEST(Halite, nested_repeated_file_with_zero_repeats)
{
    std::string template_file =
        wasp::dir_name(SOURCE_DIR + "/") + "/data/array_sub_one_trailing.tmpl";
    std::string json_data_file = wasp::dir_name(SOURCE_DIR + "/") +
                                 "/data/array_sub_one_zero_repeat.json";

    std::stringstream output_stream;

    bool template_result =
        wasp::expand_template(output_stream, std::cerr, std::cerr,
                              template_file, json_data_file, true, true);

    ASSERT_TRUE(template_result);
    std::stringstream expected_ss;
    expected_ss << "' z = 1 " << std::endl
                << "' end of level 1 of z" << std::endl;

    EXPECT_EQ(expected_ss.str(), output_stream.str());
}
