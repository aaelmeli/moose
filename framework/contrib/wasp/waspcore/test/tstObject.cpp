#include "gtest/gtest.h"

#include "waspcore/Object.h"
#include "waspcore/wasp_bug.h"
using namespace wasp;

TEST(Object, merge_basic)
{
    {  // basic merge
        DataObject obj1, obj2;
        obj1["basic_merge"] = 1;
        obj2["basic_merge"] = 2;
        obj2["f2"]          = 23;
        obj1.merge(obj2);
        EXPECT_EQ(obj1["basic_merge"].to_int(), 1);
        EXPECT_TRUE(obj1.contains("f2"));
    }
}

TEST(Object, merge_assertions)
{
    // check assertion type mismatch lhs(object) and rhs(array)
    {
        DataObject obj1, obj2;
        DataObject obj1_obj;
        obj1_obj["child"] = 1;
        DataArray obj2_array;
        obj2_array.push_back(2);
        obj1["assert_type"] = obj1_obj;
        obj2["assert_type"] = obj2_array;
        ASSERT_ANY_THROW(obj1.merge(obj2));
        // reverse
        // check assertion type mismatch lhs(array) and rhs(obj)
        obj1["assert_type"] = obj2_array;
        obj2["assert_type"] = obj1_obj;
        ASSERT_ANY_THROW(obj1.merge(obj2));
    }
    // check assertion type mismatch of index in array
    {
        DataArray a1, a2;

        DataArray  ca1;
        DataObject co1;
        a1.push_back(ca1);
        a2.push_back(co1);
        // check assertion is thrown for attempting to merge object into array
        ASSERT_ANY_THROW(a1.merge(a2));

        // reverse
        a1[0] = co1;
        a2[0] = ca1;
        // check assert is thrown for attempting to merge array into object
        ASSERT_ANY_THROW(a1.merge(a2));
    }
}
TEST(Object, merge_array)
{
    // lhs has less elements
    {
        DataArray a1;
        DataArray a2;

        a1.push_back(1);
        a1.push_back(2);
        a1.push_back(3);
        a2.push_back(10);
        a2.push_back(20);
        a2.push_back(30);
        a2.push_back(40);

        a1.merge(a2);
        EXPECT_EQ(a1.size(), a2.size());
        EXPECT_EQ(a1.back().to_int(), 40);
    }

    // rhs has less elements
    {
        DataArray a1;
        DataArray a2;

        a1.push_back(1);
        a1.push_back(2);
        a1.push_back(3);
        a1.push_back(40);
        a2.push_back(10);
        a2.push_back(20);
        a2.push_back(30);

        a1.merge(a2);
        EXPECT_NE(a1.size(), a2.size());
        EXPECT_EQ(a1.back().to_int(), 40);
    }

    // array containing object member merger
    {
        DataArray  a1;
        DataObject o1;
        DataArray  a2;
        DataObject o2;

        o1["obj1 number"] = 1;
        o2["obj2 number"] = 2;
        o2["obj1 number"] = 2;

        a1.push_back(o1);
        a2.push_back(o2);

        a1.merge(a2);
        EXPECT_EQ(a1.size(), a2.size());
        EXPECT_EQ(a1.size(), 1);
        EXPECT_EQ(a1[0].size(), 2);
        ASSERT_TRUE(a1[0].is_object());
        ASSERT_TRUE(a1[0].as_object().contains("obj1 number"));
        ASSERT_TRUE(a1[0].as_object().contains("obj2 number"));
        EXPECT_EQ(a1[0]["obj1 number"].to_int(), 1);
        EXPECT_EQ(a1[0]["obj2 number"].to_int(), 2);
    }
}

TEST(Value, constructors)
{
    {
        Value v;
        ASSERT_TRUE(v.is_null());
        ASSERT_EQ(0, v.to_int());
        ASSERT_THROW(v.to_array(), std::runtime_error);
        ASSERT_THROW(v.to_object(), std::runtime_error);
        ASSERT_EQ(Value::TYPE_NULL, v.type());
    }
    {
        Value v(true);
        ASSERT_TRUE(v.is_bool());
        ASSERT_EQ(1, v.to_int());
        ASSERT_EQ(1.0, v.to_double());
        ASSERT_TRUE(v.to_bool());
        ASSERT_EQ(Value::TYPE_BOOLEAN, v.type());
    }
    {
        Value v(1);
        ASSERT_TRUE(v.is_int());
        ASSERT_TRUE(v.is_number());
        ASSERT_EQ(1, v.to_int());
        ASSERT_EQ(1.0, v.to_double());
        ASSERT_EQ(Value::TYPE_INTEGER, v.type());
    }
    {
        Value v(1.1);
        ASSERT_FALSE(v.is_string());
        ASSERT_TRUE(v.is_double());
        ASSERT_TRUE(v.is_number());
        ASSERT_EQ(1, v.to_int());
        ASSERT_EQ(1.1, v.to_double());
        ASSERT_EQ(Value::TYPE_DOUBLE, v.type());
    }
    {
        Value v("ted");
        ASSERT_TRUE(v.is_string());
        ASSERT_FALSE(v.is_number());
        ASSERT_EQ("ted", v.to_string());
        ASSERT_EQ(Value::TYPE_STRING, v.type());
    }
    {
        Value v(std::string("fred"));
        ASSERT_TRUE(v.is_string());
        ASSERT_EQ("fred", v.to_string());
        std::string d = v.to_cstring();
        ASSERT_EQ("fred", d);
        ASSERT_EQ(Value::TYPE_STRING, v.type());
        v = 1;
        ASSERT_TRUE(v.is_int());
        ASSERT_TRUE(v.is_number());
        ASSERT_EQ(1, v.to_int());
        ASSERT_EQ(1.0, v.to_double());
        ASSERT_EQ(Value::TYPE_INTEGER, v.type());
    }
    {
        DataArray da;
        Value     v(da);
        ASSERT_FALSE(v.is_string());
        ASSERT_TRUE(v.is_array());
        ASSERT_EQ(Value::TYPE_ARRAY, v.type());
    }
    {
        DataObject obj;
        Value      v(obj);
        ASSERT_FALSE(v.is_string());
        ASSERT_TRUE(v.is_object());
        ASSERT_EQ(Value::TYPE_OBJECT, v.type());
    }
}

TEST(DataArray, methods)
{
    DataArray a;
    ASSERT_EQ(0, a.size());
    ASSERT_TRUE(a.empty());
    ASSERT_TRUE(a.begin() == a.end());
    a.push_back(Value(1));
    ASSERT_EQ(1, a.size());
    ASSERT_FALSE(a.empty());
    ASSERT_EQ(Value::TYPE_INTEGER, a.back().type());
    ASSERT_EQ(1, a.back().to_int());

    a.push_back("ted");
    ASSERT_EQ(Value::TYPE_STRING, a.back().type());
    ASSERT_EQ("ted", a.back().to_string());
    ASSERT_EQ(2, a.size());

    //    const DataArray& b = a;
    // below check makes no sense (auto resizing array on const DataArray)
    //    ASSERT_EQ(Value::TYPE_NULL, b[a.size()+1].type() );

    // check object in array

    {
        DataObject obj;
        obj["int"]    = 1;
        obj["float"]  = 2.f;
        obj["double"] = 3.0;
        a.push_back(obj);
    }
    {
        ASSERT_EQ(3, a.size());
        const DataObject& obj = a[2].as_object();
        EXPECT_EQ(Value::TYPE_INTEGER, obj["int"].type());
        EXPECT_EQ(obj["int"].to_int(), 1);
        EXPECT_EQ(Value::TYPE_DOUBLE, obj["float"].type());
        EXPECT_DOUBLE_EQ(obj["float"].to_double(), 2.);
        EXPECT_EQ(Value::TYPE_DOUBLE, obj["double"].type());
        EXPECT_DOUBLE_EQ(obj["double"].to_double(), 3.);
        // check editing reference
        DataObject& o = a[2].as_object();
        o["int"]      = 1.0;
        o["float"]    = 1;
        o["double"]   = 20.;
    }
    {
        const DataObject& obj = a[2].as_object();
        EXPECT_EQ(Value::TYPE_DOUBLE, obj["int"].type());
        EXPECT_DOUBLE_EQ(obj["int"].to_double(), 1.);
        EXPECT_EQ(Value::TYPE_INTEGER, obj["float"].type());
        EXPECT_EQ(obj["float"].to_int(), 1);
        EXPECT_EQ(Value::TYPE_DOUBLE, obj["double"].type());
        EXPECT_DOUBLE_EQ(obj["double"].to_double(), 20.);
    }
}
TEST(DataObject, methods)
{
    DataObject o;
    ASSERT_EQ(0, o.size());
    ASSERT_TRUE(o.empty());
    ASSERT_TRUE(o.begin() == o.end());
    ASSERT_FALSE(o.contains("fed"));
    ASSERT_TRUE(o.insert(std::make_pair("fed", Value("hi"))).second);
    ASSERT_FALSE(o.insert(std::make_pair("fed", Value("bye"))).second);
    ASSERT_EQ(1, o.size());
    ASSERT_FALSE(o.empty());
    o["ted"] = 1;
    ASSERT_EQ(2, o.size());
    ASSERT_TRUE(o.contains("ted"));
    ASSERT_EQ(Value::TYPE_INTEGER, o["ted"].type());
    ASSERT_EQ(1, o["ted"].to_int());
    o["ted"] = 1.4;
    ASSERT_EQ(2, o.size());
    ASSERT_TRUE(o.contains("ted"));
    ASSERT_EQ(Value::TYPE_DOUBLE, o["ted"].type());
    ASSERT_EQ(1.4, o["ted"].to_double());

    {
        DataObject o2;
        o2["fred"] = "teds brother";
        o["ted"]   = o2;
    }
    ASSERT_EQ(Value::TYPE_OBJECT, o["ted"].type());
    ASSERT_TRUE(o["ted"].to_object() != nullptr);
    ASSERT_TRUE(o["ted"].to_object()->contains("fred"));
    ASSERT_TRUE(o["ted"].as_object().contains("fred"));
    ASSERT_EQ(Value::TYPE_STRING, o["ted"]["fred"].type());
    ASSERT_EQ("teds brother", o["ted"]["fred"].to_string());
    {
        DataArray a;
        for (int i = 0; i < 10; ++i)
        {
            a.push_back(i);
        }
        o["ted"]["arabic numbers"] = a;
    }
    ASSERT_TRUE(o["ted"].to_object()->contains("arabic numbers"));
    ASSERT_TRUE(o["ted"].as_object().contains("arabic numbers"));
    DataArray* a    = o["ted"]["arabic numbers"].to_array();
    DataArray& aref = o["ted"]["arabic numbers"].as_array();
    ASSERT_TRUE(a != nullptr);
    ASSERT_EQ(10, a->size());
    ASSERT_EQ(10, aref.size());
    for (size_t i = 0; i < a->size(); ++i)
    {
        SCOPED_TRACE(i);
        ASSERT_EQ(i, a->at(i).to_int());
        ASSERT_EQ(i, a->operator[](i).to_int());
        ASSERT_EQ(i, aref[i].to_int());  //<-- so much cleaner
        ASSERT_EQ(Value::TYPE_INTEGER, o["ted"]["arabic numbers"][i].type());
        ASSERT_EQ(i, o["ted"]["arabic numbers"][i].to_int());
    }
    Value vo(o);
    ASSERT_TRUE(vo.is_object());
    ASSERT_FALSE(vo.is_null());
    Value moved(std::move(vo));
    ASSERT_FALSE(vo.is_object());
    ASSERT_TRUE(vo.is_null());
    {
        ASSERT_EQ(Value::TYPE_OBJECT, moved["ted"].type());
        ASSERT_TRUE(moved["ted"].to_object() != nullptr);
        ASSERT_TRUE(moved["ted"].to_object()->contains("fred"));
        ASSERT_EQ(Value::TYPE_STRING, moved["ted"]["fred"].type());
        ASSERT_EQ("teds brother", o["ted"]["fred"].to_string());
        {
            DataArray a;
            for (int i = 0; i < 10; ++i)
            {
                a.push_back(i);
            }
            moved["ted"]["arabic numbers"] = a;
        }
        ASSERT_TRUE(moved["ted"].to_object()->contains("arabic numbers"));
        DataArray* a = moved["ted"]["arabic numbers"].to_array();
        ASSERT_TRUE(a != nullptr);
        ASSERT_EQ(10, a->size());
        for (size_t i = 0; i < a->size(); ++i)
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(i, a->at(i).to_int());
            ASSERT_EQ(Value::TYPE_INTEGER,
                      moved["ted"]["arabic numbers"][i].type());
            ASSERT_EQ(i, moved["ted"]["arabic numbers"][i].to_int());
        }
    }
}
