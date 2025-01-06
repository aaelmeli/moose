/*
 * File:   tstJSON.cpp
 * Author: raq
 *
 * Created on September 26, 2013, 11:43 AM
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "waspjson/JSONInterpreter.h"
#include "waspjson/JSONObjectParser.hpp"
#include "waspcore/Object.h"
#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

/**
 * @brief TEST simple json object data with mix of standard
 */
TEST(JSON, simple_object)
{
    std::stringstream input;
    input << R"INPUT({
 "key_string":"value1",
 "key_int" : 1 ,
 "key_double" : 1.03,
 "key_bool_true" : true,
 "key_bool_false" : false,
 "key_null" : null,
 "object_empty": { },
 "object_mixed": { "o":{}, "a bc \"esca ped\" de f":[], "k":1.0 },
 "array_empty" : [],
 "array_mixed" : [{}, [], 1, 1.004, true, false, null ]
})INPUT";
    DefaultJSONInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(89, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(21, document.child_count());
    ASSERT_EQ(wasp::OBJECT, document.type());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    ASSERT_EQ(input.str(), document.data());
    std::stringstream paths;
    document.paths(paths);
    std::stringstream expected_paths;
    expected_paths << R"INPUT(/
/{ ({)
/key_string
/key_string/decl ("key_string")
/key_string/: (:)
/key_string/value ("value1")
/, (,)
/key_int
/key_int/decl ("key_int")
/key_int/: (:)
/key_int/value (1)
/, (,)
/key_double
/key_double/decl ("key_double")
/key_double/: (:)
/key_double/value (1.03)
/, (,)
/key_bool_true
/key_bool_true/decl ("key_bool_true")
/key_bool_true/: (:)
/key_bool_true/value (true)
/, (,)
/key_bool_false
/key_bool_false/decl ("key_bool_false")
/key_bool_false/: (:)
/key_bool_false/value (false)
/, (,)
/key_null
/key_null/decl ("key_null")
/key_null/: (:)
/key_null/value (null)
/, (,)
/object_empty
/object_empty/decl ("object_empty")
/object_empty/: (:)
/object_empty/{ ({)
/object_empty/} (})
/, (,)
/object_mixed
/object_mixed/decl ("object_mixed")
/object_mixed/: (:)
/object_mixed/{ ({)
/object_mixed/o
/object_mixed/o/decl ("o")
/object_mixed/o/: (:)
/object_mixed/o/{ ({)
/object_mixed/o/} (})
/object_mixed/, (,)
/object_mixed/a bc \"esca ped\" de f
/object_mixed/a bc \"esca ped\" de f/decl ("a bc \"esca ped\" de f")
/object_mixed/a bc \"esca ped\" de f/: (:)
/object_mixed/a bc \"esca ped\" de f/[ ([)
/object_mixed/a bc \"esca ped\" de f/] (])
/object_mixed/, (,)
/object_mixed/k
/object_mixed/k/decl ("k")
/object_mixed/k/: (:)
/object_mixed/k/value (1.0)
/object_mixed/} (})
/, (,)
/array_empty
/array_empty/decl ("array_empty")
/array_empty/: (:)
/array_empty/[ ([)
/array_empty/] (])
/, (,)
/array_mixed
/array_mixed/decl ("array_mixed")
/array_mixed/: (:)
/array_mixed/[ ([)
/array_mixed/value
/array_mixed/value/{ ({)
/array_mixed/value/} (})
/array_mixed/, (,)
/array_mixed/value
/array_mixed/value/[ ([)
/array_mixed/value/] (])
/array_mixed/, (,)
/array_mixed/value (1)
/array_mixed/, (,)
/array_mixed/value (1.004)
/array_mixed/, (,)
/array_mixed/value (true)
/array_mixed/, (,)
/array_mixed/value (false)
/array_mixed/, (,)
/array_mixed/value (null)
/array_mixed/] (])
/} (})
)INPUT";
    ASSERT_EQ(expected_paths.str(), paths.str());

    DataObject        json;
    std::stringstream gen_errors;
    bool json_object_generated = interpreter.generate_object(json, gen_errors);
    std::cout << gen_errors.str();
    ASSERT_TRUE(json_object_generated);

    ASSERT_EQ(10, json.size());
    ASSERT_EQ("value1", json["key_string"].to_string());
    ASSERT_EQ(1, json["key_int"].to_int());
    ASSERT_EQ(1.03, json["key_double"].to_double());
    ASSERT_TRUE(true == json["key_bool_true"].to_bool());
    ASSERT_TRUE(false == json["key_bool_false"].to_bool());
    ASSERT_FALSE(json["key_null"].to_bool());
    ASSERT_TRUE(json["key_null"].is_null());

    ASSERT_TRUE(json["object_empty"].empty());
    ASSERT_EQ(0, json["object_empty"].size());

    ASSERT_EQ(3, json["object_mixed"].size());
    ASSERT_FALSE(json["object_mixed"].empty());

    ASSERT_EQ(0, json["object_mixed"]["o"].size());
    ASSERT_TRUE(json["object_mixed"]["o"].empty());

    ASSERT_EQ(0, json["object_mixed"]["a"].size());
    ASSERT_TRUE(json["object_mixed"]["a"].empty());
    ASSERT_EQ(1.0, json["object_mixed"]["k"].to_double());

    ASSERT_TRUE(json["array_empty"].empty());
    ASSERT_EQ(0, json["array_empty"].size());

    ASSERT_FALSE(json["array_mixed"].empty());
    ASSERT_EQ(7, json["array_mixed"].size());

    ASSERT_TRUE(json["array_mixed"][0].is_object());
    ASSERT_TRUE(json["array_mixed"][0].empty());

    ASSERT_TRUE(json["array_mixed"][1].is_array());
    ASSERT_TRUE(json["array_mixed"][1].empty());

    ASSERT_EQ(1, json["array_mixed"][2].to_int());
    ASSERT_TRUE(json["array_mixed"][2].is_int());

    ASSERT_EQ(1.004, json["array_mixed"][3].to_double());
    ASSERT_TRUE(json["array_mixed"][3].is_double());

    ASSERT_EQ(1.0, json["array_mixed"][4].to_double());
    ASSERT_EQ(1, json["array_mixed"][4].to_int());
    ASSERT_TRUE(json["array_mixed"][4].to_bool());
    ASSERT_TRUE(json["array_mixed"][4].is_bool());

    ASSERT_EQ(0.0, json["array_mixed"][5].to_double());
    ASSERT_EQ(0, json["array_mixed"][5].to_int());
    ASSERT_FALSE(json["array_mixed"][5].to_bool());
    ASSERT_TRUE(json["array_mixed"][5].is_bool());

    // null
    ASSERT_EQ(0.0, json["array_mixed"][6].to_double());
    ASSERT_EQ(0, json["array_mixed"][6].to_int());
    ASSERT_FALSE(json["array_mixed"][6].to_bool());
    ASSERT_FALSE(json["array_mixed"][6].is_bool());
    ASSERT_TRUE(json["array_mixed"][6].is_null());

    std::stringstream json_formatted;
    json.format_json(json_formatted);
    //    std::cout<<json_formatted.str();
    std::stringstream expected_json_formatted;
    expected_json_formatted << R"INPUT({
  "array_empty" : []
  ,"array_mixed" : [
    {}
    ,[]
    ,1
    ,1.004
    ,true
    ,false
    ,null
  ]
  ,"key_bool_false" : false
  ,"key_bool_true" : true
  ,"key_double" : 1.03
  ,"key_int" : 1
  ,"key_null" : null
  ,"key_string" : "value1"
  ,"object_empty" : {}
  ,"object_mixed" : {
    "a" : null
    ,"a bc \"esca ped\" de f" : []
    ,"k" : 1
    ,"o" : {}
  }
})INPUT";
    ASSERT_EQ(expected_json_formatted.str(), json_formatted.str());
    std::stringstream expected_json_packed;
    expected_json_packed
        << R"INPUT({"array_empty":[],"array_mixed":[{},[],1,1.004,true,false,null],"key_bool_false":false,"key_bool_true":true,"key_double":1.03,"key_int":1,"key_null":null,"key_string":"value1","object_empty":{},"object_mixed":{"a":null,"a bc \"esca ped\" de f":[],"k":1,"o":{}}})INPUT";
    std::stringstream packed;
    json.pack_json(packed);
    ASSERT_EQ(expected_json_packed.str(), packed.str());
    DataObject::SP    json_copy;
    std::stringstream unpack_errors;
    ASSERT_TRUE(wasp::generate_object<JSONObjectParser>(json_copy, packed,
                                                        unpack_errors));
    json_formatted.str("");  // clear old results
    ASSERT_EQ("", json_formatted.str());
    json_copy->format_json(json_formatted);
    // check round-tripped formatted json
    ASSERT_EQ(expected_json_formatted.str(), json_formatted.str());
}

/**
 * @brief TEST simple json array data with mix of standard
 */
TEST(JSON, simple_array)
{
    std::stringstream input;
    input << "[]";  // empty
    DefaultJSONInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(3, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(wasp::ARRAY, document.type());
    std::stringstream printed_document;
    wasp::print(printed_document, document);
    ASSERT_EQ(input.str(), printed_document.str());
    ASSERT_EQ(input.str(), document.data());
}

TEST(JSON, simple_nulls)
{
    DataObject::SP    json;
    std::stringstream json_formatted;
    json_formatted << R"INPUT({
    "array" : [null, null, null]
    ,"key" : null
    ,"object" : {"key":null}
    })INPUT";
    std::stringstream unpack_errors;
    bool              readNull = wasp::generate_object<JSONObjectParser>(
        json, json_formatted, unpack_errors);

    if (!readNull)
    {
        std::cerr << unpack_errors.str() << std::endl;
    }
    ASSERT_TRUE(readNull);

    std::stringstream expected_json_packed;
    expected_json_packed
        << R"INPUT({"array":[null,null,null],"key":null,"object":{"key":null}})INPUT";
    std::stringstream packed;
    json->pack_json(packed);
    ASSERT_EQ(expected_json_packed.str(), packed.str());
    // test writting
    DataObject::SP json_copy;
    ASSERT_TRUE(wasp::generate_object<JSONObjectParser>(json_copy, packed,
                                                        unpack_errors));
    ASSERT_TRUE(json_copy->operator[]("array").as_array().size() == 3);
    ASSERT_TRUE(json_copy->operator[]("array").as_array()[0].is_null());
    ASSERT_TRUE(json_copy->operator[]("key").is_null());
    ASSERT_TRUE(json_copy->operator[]("object").is_object());
    ASSERT_TRUE(json_copy->operator[]("object").as_object()["key"].is_null());
    std::stringstream temp_stream;
    json_copy->pack_json(temp_stream);
    // check round-tripped formatted json
    ASSERT_EQ(expected_json_packed.str(), temp_stream.str());
}
// TODO - add test for each potential fail scenario and verify expected message
