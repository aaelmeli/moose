#ifndef WASP_OBJECT_H
#define WASP_OBJECT_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "waspcore/decl.h"

namespace wasp
{
/**
 * @brief The Value class represents values of objects (null, integer, double,
 * string, array, or object)
 */
class WASP_PUBLIC Value
{
  public:
    typedef std::shared_ptr<Value> SP;
    enum Type : unsigned char
    {
        TYPE_NULL,
        TYPE_BOOLEAN,
        TYPE_INTEGER,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_ARRAY,
        TYPE_OBJECT
    };

  private:
    bool m_allocated;
    Type m_type;
    union DataUnion
    {
        bool              m_bool;
        double            m_double;
        int               m_int;
        char*             m_string;
        class DataArray*  m_array;
        class DataObject* m_object;
    } m_data;

  public:
    /// null constructor
    Value();
    /// copy constructor
    Value(const Value& orig);
    // move constructor
    Value(Value&& orig);

    // boolean
    Value(bool v);
    // integer
    Value(int v);
    // double
    Value(double v);
    // const char *
    Value(const char* v);
    // string
    Value(const std::string& v);
    // data array
    Value(const DataArray& d);
    // data object
    Value(const DataObject& d);
    ~Value();

    // assignment operators
    Value& operator=(const Value& v);
    Value& operator=(Value&& v);
    Value& operator=(bool v);
    Value& operator=(int v);
    Value& operator=(double v);
    Value& operator=(const char* v);
    Value& operator=(const std::string& v);
    Value& operator=(const DataArray& v);
    Value& operator=(const DataObject& v);

    Value::Type type() const;
    /**
     * @brief categoryString
     * @return Returns a string for type display (object, array, number,
     * boolean, null)
     */
    std::string categoryString() const;

    bool is_null() const { return m_type == TYPE_NULL; }
    bool is_int() const { return m_type == TYPE_INTEGER; }
    bool is_double() const { return m_type == TYPE_DOUBLE; }
    bool is_number() const { return is_int() || is_double(); }
    bool is_bool() const { return m_type == TYPE_BOOLEAN; }
    bool is_string() const { return m_type == TYPE_STRING; }
    bool is_array() const { return m_type == TYPE_ARRAY; }
    bool is_object() const { return m_type == TYPE_OBJECT; }
    bool is_primitive() const { return !(is_array() || is_object()); }

    bool convertable(Value::Type to) const;

    int         to_int() const;
    double      to_double() const;
    bool        to_bool() const;
    const char* to_cstring() const;
    std::string to_string() const;
    DataArray*  to_array() const;
    DataObject* to_object() const;

    const DataArray&  as_array() const;
    DataArray&        as_array();
    const DataObject& as_object() const;
    DataObject&       as_object();

    Value& operator[](const std::string& name);
    const Value& operator[](const std::string& name) const;
    Value& operator[](size_t i);
    const Value& operator[](size_t i) const;

    /**
     * @brief empty whether the value is empty
     * @return false when value is null or empty object/array
     */
    bool empty() const;

    /**
     * @brief size the number of elements (object keys or array indices)
     * @return size_t array element count, object member count, or 0
     */
    size_t size() const;

    bool
    format_json(std::ostream& out, int indent_level = 2, int level = 0) const;
    bool pack_json(std::ostream& out) const;

  private:
    friend class JSONObjectParser;
    void assign(DataObject* obj);
    void assign(DataArray* array);
    /**
     * @brief nullify deletes and nullifies this object
     */
    void nullify();

    /**
     * @brief copy_from copies the given value to this value
     * @param orig the value from which data will be copied
     */
    void copy_from(const Value& orig);
};

class WASP_PUBLIC DataArray
{
  public:
    typedef std::shared_ptr<DataArray> SP;
    typedef std::vector<Value>         storage_type;

  private:
    storage_type m_data;

  public:
    DataArray();
    DataArray(const DataArray& orig);
    ~DataArray();

    size_t size() const;
    bool   empty() const;

    storage_type::const_iterator begin() const { return m_data.begin(); }
    storage_type::const_iterator end() const { return m_data.end(); }
    storage_type::iterator       begin() { return m_data.begin(); }
    storage_type::iterator       end() { return m_data.end(); }

    const Value& operator[](size_t i) const
    {
        return m_data.at(i);
    }  // at(i) for exception
    Value& operator[](size_t i)
    {
        if (size() <= i)
            resize(i + 1);
        return m_data[i];
    }

    const Value& front() const { return m_data.front(); }
    Value&       front() { return m_data.front(); }
    const Value& back() const { return m_data.back(); }
    Value&       back() { return m_data.back(); }
    Value& at(size_t i) { return m_data.at(i); }
    const Value& at(size_t i) const { return m_data.at(i); }
    void push_back(const Value& n) { m_data.push_back(n); }
    void resize(size_t nsize) { m_data.resize(nsize); }

    bool
    format_json(std::ostream& out, int indent_level = 2, int level = 0) const;
    bool pack_json(std::ostream& out) const;
    void merge(const DataArray& rhs);
};

class WASP_PUBLIC DataObject
{
  public:
    typedef std::shared_ptr<DataObject> SP;
    typedef std::map<std::string, Value> storage_type;

  private:
    storage_type m_data;

  public:
    DataObject();
    DataObject(const DataObject& orig);
    ~DataObject();

    size_t size() const;
    bool   empty() const;

    storage_type::const_iterator find(const std::string& name) const
    {
        return m_data.find(name);
    }
    storage_type::iterator find(const std::string& name)
    {
        return m_data.find(name);
    }

    storage_type::const_iterator begin() const { return m_data.begin(); }
    storage_type::const_iterator end() const { return m_data.end(); }

    storage_type::iterator begin() { return m_data.begin(); }
    storage_type::iterator end() { return m_data.end(); }

    Value& operator[](const std::string& name);
    const Value& operator[](const std::string& name) const;

    bool contains(const std::string& name) const
    {
        return m_data.find(name) != end();
    }
    std::pair<storage_type::iterator, bool>
    insert(const std::pair<std::string, Value>& v)
    {
        return m_data.insert(v);
    }

    bool
    format_json(std::ostream& out, int indent_level = 2, int level = 0) const;
    bool pack_json(std::ostream& out) const;

    void merge(const DataObject& rhs);
};

template<class Interp>
WASP_PUBLIC bool
generate_object(DataObject::SP& obj, std::istream& input, std::ostream& errors)
{
    Interp interpreter(obj, input, errors, nullptr);
    bool   parsed = interpreter.parse() == 0;
    return parsed;
}

}  // end of namespace
#endif
