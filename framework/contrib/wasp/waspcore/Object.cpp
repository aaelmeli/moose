#include "waspcore/Object.h"

#include "waspcore/wasp_bug.h"
#include "waspcore/utils.h"
#include <string.h>  // strdup, free
#include <cstdlib>   // atoi/atof, etc
#include <sstream>
#include <cassert>

namespace wasp
{
Value::Value() : m_allocated(false), m_type(TYPE_NULL)
{
}
Value::Value(const Value& orig)
{
    this->copy_from(orig);
}
Value::Value(Value&& orig)
    : m_allocated(orig.m_allocated), m_type(orig.m_type), m_data(orig.m_data)
{
    orig.m_allocated = false;
    orig.m_type      = TYPE_NULL;
}
Value::Value(bool v)
{
    m_data.m_bool = v;
    m_type        = TYPE_BOOLEAN;
}
Value::Value(int v)
{
    m_data.m_int = v;
    m_type       = TYPE_INTEGER;
}
Value::Value(double v)
{
    m_data.m_double = v;
    m_type          = TYPE_DOUBLE;
}
Value::Value(const char* v)
{
    wasp_require(v);
    m_data.m_string = strdup(v);
    m_type          = TYPE_STRING;
    m_allocated     = true;
}
Value::Value(const std::string& v)
{
    m_data.m_string = strdup(v.c_str());
    m_type          = TYPE_STRING;
    m_allocated     = true;
}
Value::Value(const DataArray& v)
{
    m_data.m_array = new DataArray(v);
    m_type         = TYPE_ARRAY;
    m_allocated    = true;
}
Value::Value(const DataObject& v)
{
    m_data.m_object = new DataObject(v);
    m_type          = TYPE_OBJECT;
    m_allocated     = true;
}
void Value::copy_from(const Value& orig)
{
    m_allocated = orig.m_allocated;
    m_type      = orig.m_type;
    switch (m_type)
    {
        case TYPE_NULL:
            break;
        case TYPE_BOOLEAN:
            m_data.m_bool = orig.m_data.m_bool;
            break;
        case TYPE_INTEGER:
            m_data.m_int = orig.m_data.m_int;
            break;
        case TYPE_DOUBLE:
            m_data.m_double = orig.m_data.m_double;
            break;
        case TYPE_STRING:
            wasp_check(m_allocated);
            m_data.m_string = strdup(orig.m_data.m_string);
            break;
        case TYPE_ARRAY:
            wasp_check(m_allocated);
            m_data.m_array = new DataArray(*orig.m_data.m_array);
            wasp_ensure(m_data.m_array);
            break;
        case TYPE_OBJECT:
            wasp_check(m_allocated);
            m_data.m_object = new DataObject(*orig.m_data.m_object);
            wasp_ensure(m_data.m_object);
            break;
    }
}

Value::Type Value::type() const
{
    return m_type;
}

std::string Value::categoryString() const
{
    switch (m_type)
    {
        case Type::TYPE_INTEGER:
        case Type::TYPE_DOUBLE:
            return "number";
        case Type::TYPE_STRING:
            return "string";
        case Type::TYPE_OBJECT:
            return "object";
        case Type::TYPE_ARRAY:
            return "array";
        case Type::TYPE_BOOLEAN:
            return "boolean";
        default:
            return "null";
    }
}

Value& Value::operator=(const Value& orig)
{
    // release any allocated memory
    nullify();
    // copy from the originator
    copy_from(orig);
    return *this;
}
Value& Value::operator=(Value&& orig)
{
    // release any allocated memory
    nullify();
    m_allocated = orig.m_allocated;
    m_data      = orig.m_data;
    m_type      = orig.m_type;

    orig.m_allocated = false;
    orig.m_type      = TYPE_NULL;

    return *this;
}
Value& Value::operator=(bool v)
{
    nullify();
    m_type        = TYPE_BOOLEAN;
    m_data.m_bool = v;
    return *this;
}
Value& Value::operator=(int v)
{
    nullify();
    m_type       = TYPE_INTEGER;
    m_data.m_int = v;
    return *this;
}
Value& Value::operator=(double v)
{
    nullify();
    m_type          = TYPE_DOUBLE;
    m_data.m_double = v;
    return *this;
}
Value& Value::operator=(const char* v)
{
    nullify();
    wasp_require(v);
    m_data.m_string = strdup(v);
    m_type          = TYPE_STRING;
    m_allocated     = true;
    return *this;
}
Value& Value::operator=(const std::string& v)
{
    nullify();
    m_data.m_string = strdup(v.c_str());
    m_type          = TYPE_STRING;
    m_allocated     = true;
    return *this;
}
Value& Value::operator=(const DataArray& v)
{
    nullify();
    m_data.m_array = new DataArray(v);
    m_type         = TYPE_ARRAY;
    m_allocated    = true;
    return *this;
}
Value& Value::operator=(const DataObject& v)
{
    nullify();
    m_data.m_object = new DataObject(v);
    m_type          = TYPE_OBJECT;
    m_allocated     = true;
    return *this;
}

void Value::assign(DataObject* obj)
{
    if (obj == nullptr)
    {
        m_type      = TYPE_NULL;
        m_allocated = false;
        return;
    }
    nullify();
    m_type          = TYPE_OBJECT;
    m_allocated     = true;
    m_data.m_object = obj;
}
void Value::assign(DataArray* array)
{
    if (array == nullptr)
    {
        m_type      = TYPE_NULL;
        m_allocated = false;
        return;
    }
    nullify();
    m_type         = TYPE_ARRAY;
    m_allocated    = true;
    m_data.m_array = array;
}
void Value::nullify()
{
    switch (m_type)
    {
        case TYPE_NULL:
            break;
        case TYPE_BOOLEAN:
            break;
        case TYPE_INTEGER:
            break;
        case TYPE_DOUBLE:
            break;
        case TYPE_STRING:
            wasp_check(m_allocated);
            free(m_data.m_string);
            break;
        case TYPE_ARRAY:
            wasp_check(m_allocated);
            delete m_data.m_array;
            break;
        case TYPE_OBJECT:
            wasp_check(m_allocated);
            delete m_data.m_object;
            break;
    }
    m_type      = TYPE_NULL;
    m_allocated = false;
}

Value::~Value()
{
    this->nullify();
}

int Value::to_int() const
{
    switch (m_type)
    {
        case TYPE_NULL:
            return 0;

        case TYPE_BOOLEAN:
            return int(m_data.m_bool);

        case TYPE_INTEGER:
            return m_data.m_int;

        case TYPE_DOUBLE:
            return int(m_data.m_double);

        case TYPE_STRING:
            wasp_ensure(m_allocated);
            wasp_ensure(m_data.m_string);
            return atoi(m_data.m_string);

        case TYPE_ARRAY:
            wasp_not_implemented("conversion of array to integer");

        case TYPE_OBJECT:
            wasp_not_implemented("conversion of object to integer");
    }
    wasp_not_implemented("unknown type conversion to integer");
}
double Value::to_double() const
{
    switch (m_type)
    {
        case TYPE_NULL:
            return 0.0;

        case TYPE_BOOLEAN:
            return double(m_data.m_bool);

        case TYPE_INTEGER:
            return double(m_data.m_int);

        case TYPE_DOUBLE:
            return m_data.m_double;

        case TYPE_STRING:
            wasp_ensure(m_allocated);
            wasp_ensure(m_data.m_string);
            return atof(m_data.m_string);

        case TYPE_ARRAY:
            wasp_not_implemented("conversion of array to double");

        case TYPE_OBJECT:
            wasp_not_implemented("conversion of object to double");
    }
    wasp_not_implemented("unknown type conversion to double");
}
bool Value::to_bool() const
{
    switch (m_type)
    {
        case TYPE_NULL:
            return false;

        case TYPE_BOOLEAN:
            return m_data.m_bool;

        case TYPE_INTEGER:
            return m_data.m_int ? true : false;

        case TYPE_DOUBLE:
            return m_data.m_double ? true : false;

        case TYPE_STRING:
            wasp_not_implemented("conversion of string to boolean");

        case TYPE_ARRAY:
            wasp_not_implemented("conversion of array to double");

        case TYPE_OBJECT:
            wasp_not_implemented("conversion of object to double");
    }
    wasp_not_implemented("unknown type conversion to bool");
}
const char* Value::to_cstring() const
{
    switch (m_type)
    {
        case TYPE_NULL:
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_DOUBLE:
            return nullptr;
        case TYPE_STRING:
            wasp_check(m_allocated);
            return m_data.m_string;

        case TYPE_ARRAY:
            wasp_not_implemented("conversion of array to cstring");

        case TYPE_OBJECT:
            wasp_not_implemented("conversion of object to cstring");
    }
    wasp_not_implemented("unknown type conversion to cstring");
}
std::string Value::to_string() const
{
    switch (m_type)
    {
        case TYPE_NULL:
            return "null";
        case TYPE_BOOLEAN:
            return m_data.m_bool ? "true" : "false";
        case TYPE_INTEGER:
            return std::to_string(m_data.m_int);
        case TYPE_DOUBLE:
            return std::to_string(m_data.m_double);
        case TYPE_STRING:
            wasp_check(m_allocated);
            return m_data.m_string;

        case TYPE_ARRAY:
            wasp_not_implemented("conversion of array to string");

        case TYPE_OBJECT:
            std::stringstream str;
            to_object()->pack_json(str);
            return str.str();
    }
    wasp_not_implemented("unknown type conversion to string");
}

DataArray* Value::to_array() const
{
    wasp_insist(convertable(TYPE_ARRAY),
                "Value object must be convertable to an array");
    return m_data.m_array;
}
DataObject* Value::to_object() const
{
    wasp_insist(convertable(TYPE_OBJECT),
                "Value object must be convertable to an object");
    return m_data.m_object;
}

const DataArray& Value::as_array() const
{
    wasp_insist(convertable(TYPE_ARRAY),
                "Value object must be convertable to an array");
    return *(m_data.m_array);
}

DataArray& Value::as_array()
{
    wasp_insist(convertable(TYPE_ARRAY),
                "Value object must be convertable to an array");
    return *(m_data.m_array);
}

const DataObject& Value::as_object() const
{
    wasp_insist(convertable(TYPE_OBJECT),
                "Value object must be convertable to an object");
    return *(m_data.m_object);
}

DataObject& Value::as_object()
{
    wasp_insist(convertable(TYPE_OBJECT),
                "Value object must be convertable to an object");
    return *(m_data.m_object);
}
bool Value::convertable(Value::Type to) const
{
    switch (to)
    {
        case TYPE_NULL:
            return (is_number() && to_double() == 0.0);
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_DOUBLE:
            return is_bool() || is_null() || is_number();
        case TYPE_STRING:
            return is_string();
        case TYPE_ARRAY:
            return is_array();
        case TYPE_OBJECT:
            return is_object();
    }
    wasp_not_implemented("unknown type conversion");
}

Value& Value::operator[](const std::string& name)
{
    wasp_check(is_object());
    DataObject* o = to_object();
    return o->operator[](name);
}
const Value& Value::operator[](const std::string& name) const
{
    wasp_check(is_object());
    const DataObject* o = to_object();
    return o->operator[](name);
}
Value& Value::operator[](size_t i)
{
    wasp_check(is_array());
    DataArray* a = to_array();
    return a->operator[](i);
}
const Value& Value::operator[](size_t i) const
{
    wasp_check(is_array());
    DataArray* a = to_array();
    return a->operator[](i);
}
bool Value::empty() const
{
    if (is_array())
    {
        wasp_check(to_array());
        return to_array()->empty();
    }
    if (is_object())
    {
        wasp_check(to_object());
        return to_object()->empty();
    }
    return is_null();
}
size_t Value::size() const
{
    if (is_array())
    {
        wasp_check(to_array());
        return to_array()->size();
    }
    if (is_object())
    {
        wasp_check(to_object());
        return to_object()->size();
    }
    return 0;
}

bool Value::format_json(std::ostream& out, int indent_level, int level) const
{
    if (is_object())
    {
        to_object()->format_json(out, indent_level, level);
    }
    else if (is_array())
    {
        to_array()->format_json(out, indent_level, level);
    }
    else
    {
        switch (type())
        {
            case TYPE_STRING:
                out << "\"" << json_escape_string(to_string()) << "\"";
                break;
            case TYPE_BOOLEAN:
                out << std::boolalpha << to_bool();
                break;
            case TYPE_INTEGER:
                out << to_int();
                break;
            case TYPE_DOUBLE:
                // the precision can be set by the caller
                out << to_double();
                break;
            case TYPE_NULL:
                out << "null";
                break;
            default:
                wasp_not_implemented("unknown Object value type json emission");
        }
    }
    return out.good();
}
bool Value::pack_json(std::ostream& out) const
{
    if (is_object())
    {
        to_object()->pack_json(out);
    }
    else if (is_array())
    {
        to_array()->pack_json(out);
    }
    else
    {
        switch (type())
        {
            case TYPE_STRING:
                out << "\"" << json_escape_string(to_string()) << "\"";
                break;
            case TYPE_BOOLEAN:
                out << std::boolalpha << to_bool();
                break;
            case TYPE_INTEGER:
                out << to_int();
                break;
            case TYPE_DOUBLE:
                // the precision can be set by the caller
                out << to_double();
                break;
            case TYPE_NULL:
                out << "null";
                break;
            default:
                wasp_not_implemented("unknown Object value type json emission");
        }
    }
    return out.good();
}
DataArray::DataArray()
{
}
DataArray::~DataArray()
{
}
DataArray::DataArray(const DataArray& orig) : m_data(orig.m_data)
{
}
size_t DataArray::size() const
{
    return m_data.size();
}
bool DataArray::empty() const
{
    return m_data.empty();
}
bool DataArray::format_json(std::ostream& out,
                            int           indent_level,
                            int           level) const
{
    wasp_require(indent_level >= 0);
    wasp_require(level >= 0);
    if (empty())
    {
        out << "[]";
        return true;
    }
    out << "[" << std::endl;
    std::string indent = std::string(indent_level * (level + 1), ' ');
    out << indent;
    at(0).format_json(out, indent_level, level);
    out << std::endl;

    for (size_t i = 1, count = size(); i < count; ++i)
    {
        out << indent << ",";
        if (!at(i).format_json(out, indent_level, level))
            return false;
        out << std::endl;
    }
    out << std::string(indent_level * (level), ' ') << "]";
    return out.good();
}
bool DataArray::pack_json(std::ostream& out) const
{
    if (empty())
    {
        out << "[]";
        return true;
    }
    out << "[";
    at(0).pack_json(out);

    for (size_t i = 1, count = size(); i < count; ++i)
    {
        out << ",";
        if (!at(i).pack_json(out))
            return false;
    }
    out << "]";
    return out.good();
}

void DataArray::merge(const DataArray& rhs)
{
    // loop over every item in rhs array
    size_t index = 0;
    for (auto item : rhs)
    {
        wasp_line("Checking inbound array index(" << index << ")");
        if (size() <= index)
        {
            wasp_line("lhs doesn't have index. adding rhs index to lhs");
            this->push_back(item);
        }
        else
        {
            // must check the type of lhs[index] against that of rhs[index]
            Value& child = m_data.at(index);
            // if Value is an object do a depth-first merge
            if (child.is_object())
            {
                // check that my object is indeed an object
                if (!item.is_object())
                {
                    std::stringstream ss;
                    ss << "Attempting to merge data arrary where member index("
                       << index << ") differs in type." << std::endl
                       << "Left-hand side type(object) differs from right-hand "
                          "side type("
                       << item.categoryString() << ")";
                    throw std::logic_error(ss.str());
                }
                // merge objects
                child.to_object()->merge(item.as_object());
            }
            else if (child.is_array())
            {  // if Value is an array do a depth-first merge
                if (!item.is_array())
                {
                    std::stringstream ss;
                    ss << "Attempting to merge data array where member index("
                       << index << " differs in type."
                       << "Left-hand side type(array) differs from right-hand "
                          "side type("
                       << item.categoryString() << ")";
                    throw std::logic_error(ss.str());
                }
                // merge array
                child.to_array()->merge(item.as_array());
            }
        }
        ++index;
    }
}
DataObject::DataObject()
{
}
DataObject::DataObject(const DataObject& orig) : m_data(orig.m_data)
{
}

DataObject::~DataObject()
{
}
size_t DataObject::size() const
{
    return m_data.size();
}
bool DataObject::empty() const
{
    return m_data.empty();
}

Value& DataObject::operator[](const std::string& name)
{
    // since c++11 std::map<>[] does insertion if key doesn't exist
    return m_data[name];
}
const Value& DataObject::operator[](const std::string& name) const
{
    auto itr = m_data.find(name);

    if (itr == m_data.end())
    {
        throw std::out_of_range(name + " not found in object");
    }
    return itr->second;
}
bool DataObject::format_json(std::ostream& out,
                             int           indent_level,
                             int           level) const
{
    wasp_require(indent_level >= 0);
    wasp_check(level >= 0);
    if (empty())
    {
        out << "{}";
        return true;
    }

    out << "{" << std::endl;
    std::string indent = std::string(indent_level * (level + 1), ' ');
    out << indent;
    auto itr = begin();
    out << "\"" << itr->first << "\" : ";
    itr->second.format_json(out, indent_level, level);
    out << std::endl;
    ++itr;
    for (; itr != end(); ++itr)
    {
        out << indent << ",";
        out << "\"" << itr->first << "\" : ";
        if (!itr->second.format_json(out, indent_level, level + 1))
            return false;
        out << std::endl;
    }

    out << std::string(indent_level * (level), ' ') << "}";
    return out.good();
}
bool DataObject::pack_json(std::ostream& out) const
{
    if (empty())
    {
        out << "{}";
        return true;
    }

    out << "{";
    auto itr = begin();
    out << "\"" << itr->first << "\":";
    itr->second.pack_json(out);
    ++itr;
    for (; itr != end(); ++itr)
    {
        out << ",";
        out << "\"" << itr->first << "\":";
        if (!itr->second.pack_json(out))
            return false;
    }

    out << "}";
    return out.good();
}

void DataObject::merge(const DataObject& rhs)
{
    // merge every Value, Object and Array from 'obj'
    // that does not exist in 'this'
    for (auto& item : rhs)
    {
        wasp_line("Checking inbound " << item.first);
        if (contains(item.first))
        {
            wasp_line("Destination contains " << item.first);
            Value& child = m_data.at(item.first);
            // if Value is an object do a depth-first merge
            if (child.is_object())
            {
                // check that my object is indeed an object
                if (!item.second.is_object())
                {
                    std::stringstream ss;
                    ss << "Attempting to merge data objects where member("
                       << item.first << ") differs in type." << std::endl
                       << "Left-hand side type(object) differs from right-hand "
                          "side type("
                       << item.second.categoryString() << ")";
                    throw std::logic_error(ss.str());
                }
                // merge objects
                child.to_object()->merge(item.second.as_object());
            }
            else if (child.is_array())
            {  // if Value is an array do a depth-first merge
                if (!item.second.is_array())
                {
                    std::stringstream ss;
                    ss << "Attempting to merge data objects where member("
                       << item.first << " differs in type."
                       << "Left-hand side type(array) differs from right-hand "
                          "side type("
                       << item.second.categoryString() << ")";
                    throw std::logic_error(ss.str());
                }
                // merge array
                child.to_array()->merge(item.second.as_array());
            }
        }
        else
        {  // no destination to merge so we can simply copy
            wasp_line("Destination does not contain " << item.first
                                                      << ". Copying...");
            this->insert(item);
        }
    }
}
}  // end of namespace wasp
