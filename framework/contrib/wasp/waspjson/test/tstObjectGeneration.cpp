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
 "key_string":"val\"ue1\\","key_int" : 1 ,
 "key_double" : 1.03,
 "key_bool_true" : true,
 "key_bool_false" : false,
 "key_null" : null,
 "object_empty": { },
 "object_mixed": { "o":{}, "a bc \"esca ped\" de f":[], "k":1.0 },
 "array_empty" : [],
 "array_mixed" : [{}, [], 1, 1.004, true, false, null ]
})INPUT";
    DataObject::SP json_ptr;
    {
        JSONObjectParser generator(json_ptr, input, std::cerr, nullptr);
        ASSERT_EQ(0, generator.parse());
    }
    ASSERT_TRUE(json_ptr != nullptr);
    DataObject& json = *(json_ptr.get());

    ASSERT_EQ(10, json.size());

    ASSERT_EQ(1, json["key_int"].to_int());
    ASSERT_EQ(1.03, json["key_double"].to_double());
    ASSERT_EQ("val\"ue1\\", json["key_string"].to_string());
    ASSERT_TRUE(json["key_bool_true"].to_bool());
    ASSERT_FALSE(json["key_bool_false"].to_bool());
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
}
