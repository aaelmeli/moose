/*
 * File:   tstHalite.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>

#include <utility>
#include "waspcore/Object.h"
#include "wasphalite/DataAccessor.h"
#include "waspjson/JSONObjectParser.hpp"

#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

/**
 * @brief TEST data accessor with default data store
 */
TEST(Halite, data_accessor_store)
{
    DataAccessor a;
    ASSERT_TRUE(a.store("i", 10));
    ASSERT_EQ(Context::Type::INTEGER, a.type("i"));
    ASSERT_EQ(10, a.integer("i"));

    ASSERT_TRUE(a.store("d", 10.012));
    ASSERT_EQ(Context::Type::REAL, a.type("d"));
    ASSERT_EQ(10.012, a.real("d"));

    ASSERT_TRUE(a.store("b", true));
    ASSERT_EQ(Context::Type::BOOLEAN, a.type("b"));
    ASSERT_TRUE(a.boolean("b"));

    // must wrap raw char* in std::string() to disambiguate implicit bool
    // conversion
    ASSERT_TRUE(a.store("s", std::string("10aC")));
    ASSERT_EQ(Context::Type::STRING, a.type("s"));
    ASSERT_EQ("10aC", a.string("s"));
}

TEST(Halite, data_accessor_store_object)
{
    DataObject   o;
    DataAccessor a(&o);
    ASSERT_TRUE(a.store("i", 10));
    ASSERT_EQ(Context::Type::INTEGER, a.type("i"));
    ASSERT_EQ(10, a.integer("i"));

    ASSERT_TRUE(a.store("d", 10.012));
    ASSERT_EQ(Context::Type::REAL, a.type("d"));
    ASSERT_EQ(10.012, a.real("d"));

    ASSERT_TRUE(a.store("b", true));
    ASSERT_EQ(Context::Type::BOOLEAN, a.type("b"));
    ASSERT_TRUE(a.boolean("b"));

    // must wrap raw char* in std::string() to disambiguate implicit bool
    // conversion
    ASSERT_TRUE(a.store("s", std::string("10aC")));
    ASSERT_EQ(Context::Type::STRING, a.type("s"));
    ASSERT_EQ("10aC", a.string("s"));
}

TEST(Halite, data_accessor_dataobject_backed)
{
    DataObject   o;
    DataAccessor a(&o);
    ASSERT_TRUE(a.store("i", 10));
    ASSERT_EQ(Context::Type::INTEGER, a.type("i"));
    ASSERT_EQ(10, a.integer("i"));

    ASSERT_TRUE(a.store("d", 10.012));
    ASSERT_EQ(Context::Type::REAL, a.type("d"));
    ASSERT_EQ(10.012, a.real("d"));

    ASSERT_TRUE(a.store("b", true));
    ASSERT_EQ(Context::Type::BOOLEAN, a.type("b"));
    ASSERT_TRUE(a.boolean("b"));

    // must wrap raw char* in std::string() to disambiguate implicit bool
    // conversion
    ASSERT_TRUE(a.store("s", std::string("10aC")));
    ASSERT_EQ(Context::Type::STRING, a.type("s"));
    ASSERT_EQ("10aC", a.string("s"));

    o["i1"] = 11;
    o["d1"] = 10.014;
    o["b1"] = false;
    o["s1"] = "ten a C";

    ASSERT_EQ(Context::Type::UNDEFINED, a.type("undefined"));

    ASSERT_EQ(Context::Type::INTEGER, a.type("i1"));
    ASSERT_EQ(11, a.integer("i1"));

    ASSERT_EQ(Context::Type::REAL, a.type("d1"));
    ASSERT_EQ(10.014, a.real("d1"));

    ASSERT_EQ(Context::Type::BOOLEAN, a.type("b1"));
    ASSERT_FALSE(a.boolean("b1"));

    ASSERT_EQ(Context::Type::STRING, a.type("s1"));
    ASSERT_EQ("ten a C", a.string("s1"));
}

TEST(Halite, data_accessor_parent_access)
{
    DataObject   o;
    DataAccessor a(&o);
    DataAccessor l(nullptr, &a);
    ASSERT_TRUE(a.store("i", 10));
    ASSERT_EQ(Context::Type::INTEGER, l.type("i"));
    ASSERT_EQ(10, l.integer("i"));

    ASSERT_TRUE(a.store("d", 10.012));
    ASSERT_EQ(Context::Type::REAL, l.type("d"));
    ASSERT_EQ(10.012, l.real("d"));

    ASSERT_TRUE(a.store("b", true));
    ASSERT_EQ(Context::Type::BOOLEAN, l.type("b"));
    ASSERT_TRUE(l.boolean("b"));

    // must wrap raw char* in std::string() to disambiguate implicit bool
    // conversion
    ASSERT_TRUE(a.store("s", std::string("10aC")));
    ASSERT_EQ(Context::Type::STRING, l.type("s"));
    ASSERT_EQ("10aC", l.string("s"));

    o["i1"]         = 11;
    o["d1"]         = 10.014;
    o["b1"]         = false;
    o["s1"]         = "ten a C";
    o["a"]          = DataArray();
    o["a"][0]       = 2;
    o["a"][1]       = 3.14;
    o["a"][2]       = true;
    o["a"][3]       = std::string("ted");
    o["obj"]        = DataObject();
    o["obj"]["var"] = 6.28;

    ASSERT_EQ(Context::Type::UNDEFINED, l.type("undefined"));

    // access variables via child layer
    ASSERT_EQ(Context::Type::INTEGER, l.type("i1"));
    ASSERT_EQ(Context::Type::INTEGER, l.type("a", 0));
    ASSERT_EQ(11, l.integer("i1"));
    ASSERT_EQ(2, l.integer("a", size_t(0)));

    ASSERT_EQ(Context::Type::REAL, l.type("d1"));
    ASSERT_EQ(Context::Type::REAL, l.type("a", 1));
    ASSERT_EQ(10.014, l.real("d1"));
    ASSERT_EQ(3.14, l.real("a", 1));

    ASSERT_EQ(Context::Type::BOOLEAN, l.type("b1"));
    ASSERT_EQ(Context::Type::BOOLEAN, l.type("a", 2));
    ASSERT_FALSE(l.boolean("b1"));
    ASSERT_TRUE(l.boolean("a", 2));

    ASSERT_EQ(Context::Type::STRING, l.type("s1"));
    ASSERT_EQ(Context::Type::STRING, l.type("a", 3));
    ASSERT_EQ("ten a C", l.string("s1"));
    ASSERT_EQ("ted", l.string("a", 3));

    ASSERT_NE(nullptr, l.object("obj"));

    // Get a DataAccess at the level of 'obj'
    // and request the obj sibling array 'a'
    DataAccessor obj_access(l.object("obj"), &l);
    ASSERT_NE(nullptr, obj_access.array("a"));
    DataArray* array_a = obj_access.array("a");
    ASSERT_EQ(4, array_a->size());
    ASSERT_TRUE(array_a->at(0).is_int());
    ASSERT_EQ(2, array_a->at(0).to_int());
}


/**
 * @brief TEST hierarchy access
 */
TEST(Halite, hierarchy)
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
 "object": { "o":{"r":1.1, "i":4, "bt" :true ,"bf":false, "n": null, "a" : [2,3],
                "o":{"r":1.1, "i":4, "bt" :true ,"bf":false, "n": null, "a" : [2,3]}
             },
             "a":[1,2,3], "r":1.0, "i":3, "bt" :true ,"bf":false, "n": null}
})INPUT";
    DataObject::SP json_ptr;
    {
        JSONObjectParser generator(json_ptr, input, std::cerr, nullptr);
        ASSERT_EQ(0, generator.parse());
    }
    ASSERT_TRUE(json_ptr != nullptr);
    DataObject& json = *(json_ptr.get());

    { // test '.'
        DataAccessor accessor(&json, nullptr, ".");

        ASSERT_TRUE(accessor.exists("key_bool_false"));
        ASSERT_FALSE(accessor.boolean("key_bool_false"));
        ASSERT_TRUE(accessor.exists("key_bool_true"));
        ASSERT_TRUE(accessor.boolean("key_bool_true"));
        ASSERT_TRUE(accessor.exists("object.bt"));
        ASSERT_TRUE(accessor.boolean("object.bt"));
        ASSERT_TRUE(accessor.exists("object.bf"));
        ASSERT_FALSE(accessor.boolean("object.bf"));
        ASSERT_TRUE(accessor.exists("object.i"));
        ASSERT_EQ( 3, accessor.integer("object.i"));
        ASSERT_TRUE(accessor.exists("object.r"));
        ASSERT_EQ( 1.0, accessor.real("object.r"));
        ASSERT_NE( nullptr, accessor.array("object.a"));
        ASSERT_EQ( 3, accessor.array("object.a")->size());
        ASSERT_EQ( 1, accessor.array("object.a")->at(0).to_int());
        ASSERT_EQ( 2, accessor.array("object.a")->at(1).to_int());

        ASSERT_TRUE(accessor.exists("object.o.bt"));
        ASSERT_TRUE(accessor.boolean("object.o.bt"));
        ASSERT_TRUE(accessor.exists("object.o.bf"));
        ASSERT_FALSE(accessor.boolean("object.o.bf"));
        ASSERT_TRUE(accessor.exists("object.o.i"));
        ASSERT_EQ( 4, accessor.integer("object.o.i"));
        ASSERT_TRUE(accessor.exists("object.o.r"));
        ASSERT_EQ( 1.1, accessor.real("object.o.r"));
        ASSERT_NE ( nullptr, accessor.array("object.o.a"));
        ASSERT_EQ( 2, accessor.array("object.o.a")->size());
        ASSERT_EQ( 2, accessor.array("object.o.a")->at(0).to_int());
        ASSERT_EQ( 3, accessor.array("object.o.a")->at(1).to_int());

        ASSERT_TRUE(accessor.exists("object.o.o.bt"));
        ASSERT_TRUE(accessor.boolean("object.o.o.bt"));
        ASSERT_TRUE(accessor.exists("object.o.o.bf"));
        ASSERT_FALSE(accessor.boolean("object.o.o.bf"));
        ASSERT_TRUE(accessor.exists("object.o.o.i"));
        ASSERT_EQ( 4, accessor.integer("object.o.o.i"));
        ASSERT_TRUE(accessor.exists("object.o.o.r"));
        ASSERT_EQ( 1.1, accessor.real("object.o.o.r"));
        ASSERT_NE ( nullptr, accessor.array("object.o.o.a"));
        ASSERT_EQ( 2, accessor.array("object.o.o.a")->size());
        ASSERT_EQ( 2, accessor.array("object.o.o.a")->at(0).to_int());
        ASSERT_EQ( 3, accessor.array("object.o.o.a")->at(1).to_int());

        // Test parent scoping
        DataObject* object = accessor.object("object");
        ASSERT_NE( nullptr, object);
        DataAccessor accessor_object(object, &accessor, ".");

        DataObject* object_o = accessor.object("object.o");
        ASSERT_NE( nullptr, object_o);
        DataAccessor accessor_object_o(object_o, &accessor_object, ".");

        DataObject* object_o_o = accessor.object("object.o.o");
        ASSERT_NE( nullptr, object_o);
        DataAccessor accessor_object_o_o(object_o_o, &accessor_object_o, ".");

        // Test that the parent scopes are used to identify variables
        ASSERT_TRUE(accessor_object_o_o.exists("key_bool_false"));
        ASSERT_FALSE(accessor_object_o_o.boolean("key_bool_false"));
        ASSERT_TRUE(accessor_object_o_o.exists("key_bool_true"));
        ASSERT_TRUE(accessor_object_o_o.boolean("key_bool_true"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.bt"));
        ASSERT_TRUE(accessor_object_o_o.boolean("object.bt"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.bf"));
        ASSERT_FALSE(accessor_object_o_o.boolean("object.bf"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.i"));
        ASSERT_EQ( 3, accessor_object_o_o.integer("object.i"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.r"));
        ASSERT_EQ( 1.0, accessor_object_o_o.real("object.r"));
        ASSERT_NE( nullptr, accessor_object_o_o.array("object.a"));
        ASSERT_EQ( 3, accessor_object_o_o.array("object.a")->size());
        ASSERT_EQ( 1, accessor_object_o_o.array("object.a")->at(0).to_int());
        ASSERT_EQ( 2, accessor_object_o_o.array("object.a")->at(1).to_int());

        ASSERT_TRUE(accessor_object_o_o.exists("object.o.bt"));
        ASSERT_TRUE(accessor_object_o_o.boolean("object.o.bt"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.o.bf"));
        ASSERT_FALSE(accessor_object_o_o.boolean("object.o.bf"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.o.i"));
        ASSERT_EQ( 4, accessor_object_o_o.integer("object.o.i"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.o.r"));
        ASSERT_EQ( 1.1, accessor_object_o_o.real("object.o.r"));
        ASSERT_NE ( nullptr, accessor_object_o_o.array("object.o.a"));
        ASSERT_EQ( 2, accessor_object_o_o.array("object.o.a")->size());
        ASSERT_EQ( 2, accessor_object_o_o.array("object.o.a")->at(0).to_int());
        ASSERT_EQ( 3, accessor_object_o_o.array("object.o.a")->at(1).to_int());

        ASSERT_TRUE(accessor_object_o_o.exists("object.o.o.bt"));
        ASSERT_TRUE(accessor_object_o_o.boolean("object.o.o.bt"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.o.o.bf"));
        ASSERT_FALSE(accessor_object_o_o.boolean("object.o.o.bf"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.o.o.i"));
        ASSERT_EQ( 4, accessor_object_o_o.integer("object.o.o.i"));
        ASSERT_TRUE(accessor_object_o_o.exists("object.o.o.r"));
        ASSERT_EQ( 1.1, accessor_object_o_o.real("object.o.o.r"));
        ASSERT_NE ( nullptr, accessor_object_o_o.array("object.o.o.a"));
        ASSERT_EQ( 2, accessor_object_o_o.array("object.o.o.a")->size());
        ASSERT_EQ( 2, accessor_object_o_o.array("object.o.o.a")->at(0).to_int());
        ASSERT_EQ( 3, accessor_object_o_o.array("object.o.o.a")->at(1).to_int());
        ASSERT_NE( nullptr, accessor.object("object.o.o"));
    }

    { // test '::'
        DataAccessor accessor(&json, nullptr, "%%");

        ASSERT_TRUE(accessor.exists("key_bool_false"));
        ASSERT_FALSE(accessor.boolean("key_bool_false"));
        ASSERT_TRUE(accessor.exists("key_bool_true"));
        ASSERT_TRUE(accessor.boolean("key_bool_true"));
        ASSERT_TRUE(accessor.exists("object%%bt"));
        ASSERT_TRUE(accessor.boolean("object%%bt"));
        ASSERT_TRUE(accessor.exists("object%%bf"));
        ASSERT_FALSE(accessor.boolean("object%%bf"));
        ASSERT_TRUE(accessor.exists("object%%i"));
        ASSERT_EQ( 3, accessor.integer("object%%i"));
        ASSERT_TRUE(accessor.exists("object%%r"));
        ASSERT_EQ( 1.0, accessor.real("object%%r"));
        ASSERT_NE( nullptr, accessor.array("object%%a"));
        ASSERT_EQ( 3, accessor.array("object%%a")->size());
        ASSERT_EQ( 1, accessor.array("object%%a")->at(0).to_int());
        ASSERT_EQ( 2, accessor.array("object%%a")->at(1).to_int());

        ASSERT_TRUE(accessor.exists("object%%o%%bt"));
        ASSERT_TRUE(accessor.boolean("object%%o%%bt"));
        ASSERT_TRUE(accessor.exists("object%%o%%bf"));
        ASSERT_FALSE(accessor.boolean("object%%o%%bf"));
        ASSERT_TRUE(accessor.exists("object%%o%%i"));
        ASSERT_EQ( 4, accessor.integer("object%%o%%i"));
        ASSERT_TRUE(accessor.exists("object%%o%%r"));
        ASSERT_EQ( 1.1, accessor.real("object%%o%%r"));
        ASSERT_NE ( nullptr, accessor.array("object%%o%%a"));
        ASSERT_EQ( 2, accessor.array("object%%o%%a")->size());
        ASSERT_EQ( 2, accessor.array("object%%o%%a")->at(0).to_int());
        ASSERT_EQ( 3, accessor.array("object%%o%%a")->at(1).to_int());

        ASSERT_TRUE(accessor.exists("object%%o%%o%%bt"));
        ASSERT_TRUE(accessor.boolean("object%%o%%o%%bt"));
        ASSERT_TRUE(accessor.exists("object%%o%%o%%bf"));
        ASSERT_FALSE(accessor.boolean("object%%o%%o%%bf"));
        ASSERT_TRUE(accessor.exists("object%%o%%o%%i"));
        ASSERT_EQ( 4, accessor.integer("object%%o%%o%%i"));
        ASSERT_TRUE(accessor.exists("object%%o%%o%%r"));
        ASSERT_EQ( 1.1, accessor.real("object%%o%%o%%r"));
        ASSERT_NE ( nullptr, accessor.array("object%%o%%o%%a"));
        ASSERT_EQ( 2, accessor.array("object%%o%%o%%a")->size());
        ASSERT_EQ( 2, accessor.array("object%%o%%o%%a")->at(0).to_int());
        ASSERT_EQ( 3, accessor.array("object%%o%%o%%a")->at(1).to_int());
    }
}
