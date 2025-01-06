#include "wasphalite/DataAccessor.h"

namespace wasp
{
DataAccessor::DataAccessor(DataObject* data, DataAccessor* parent,
                           const std::string& hierarchy_operator)
    : Context(), m_parent(parent), m_current_data(data),
      m_hierarchy_operator(hierarchy_operator)
{
    // Inherit the hierarchy operator from the parent
    // only if not explicity specified
    if ( parent != nullptr && hierarchy_operator.empty() )
    {
        m_hierarchy_operator = parent->m_hierarchy_operator;
    }
}

DataAccessor::DataAccessor(const DataAccessor& orig)
    : Context(orig)
    , m_parent(orig.m_parent)
    , m_current_data(orig.m_current_data)
    , m_hierarchy_operator(orig.m_hierarchy_operator)
{
}

DataAccessor::~DataAccessor()
{
}

bool DataAccessor::exists(const std::string& vname) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::exists(name);
    }
    return current_data_exists || parent_data_exists;
}
Context::Type DataAccessor::type(const std::string& vname) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::type(name);
    }
    if (current_data_exists)
    {
        wasp_check(current_data);
        auto itr = current_data->find(name);
        if (itr == current_data->end())
            return Context::Type::UNDEFINED;
        const wasp::Value& variable = itr->second;
        if (variable.is_double())
            return Context::Type::REAL;
        if (variable.is_int())
            return Context::Type::INTEGER;
        if (variable.is_bool())
            return Context::Type::BOOLEAN;
        if (variable.is_string())
            return Context::Type::STRING;
        if (variable.is_object())
            return Context::Type::STRING;
    }
    else if (parent_data_exists)
    {
        wasp_check(m_parent);
        return m_parent->type(name);
    }
    return Context::Type::UNDEFINED;
}
Context::Type DataAccessor::type(const std::string& vname, size_t index) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::type(name, index);
    }
    if (current_data_exists)
    {
        wasp_check(current_data);
        auto itr = current_data->find(name);
        if (itr == current_data->end())
            return Context::Type::UNDEFINED;
        const wasp::Value& variable = itr->second;
        if (variable.is_array())
        {
            auto* array = variable.to_array();
            wasp_check(array);
            if (array->size() <= index)
            {
                return Context::Type::UNDEFINED;
            }
            const wasp::Value& variable = array->at(index);
            if (variable.is_double())
                return Context::Type::REAL;
            if (variable.is_int())
                return Context::Type::INTEGER;
            if (variable.is_bool())
                return Context::Type::BOOLEAN;
            if (variable.is_string())
                return Context::Type::STRING;
            if (variable.is_object())
                return Context::Type::STRING;
            return Context::Type::UNDEFINED;
        }
    }
    else if (parent_data_exists)
    {
        wasp_check(m_parent);
        return m_parent->type(name, index);
    }
    return Context::Type::UNDEFINED;
}

int DataAccessor::size(const std::string& vname) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::size(name);
    }
    if (current_data_exists)
    {
        wasp_check(current_data);
        auto itr = current_data->find(name);
        if (itr == current_data->end())
            return 0;
        const wasp::Value& variable = itr->second;
        if (variable.is_array())
            return variable.to_array()->size();
        if (variable.is_object())
            return variable.to_object()->size();
        return 0;
    }
    else if (parent_data_exists)
    {
        wasp_check(m_parent);
        return m_parent->size(name);
    }
    return 0;
}
bool DataAccessor::store(const std::string& vname, const bool& v)
{
    std::string name = vname;
    auto* current_data = scope(name);
    if (current_data == nullptr)
    {
        return Context::store(name, v);
    }
    (*current_data)[name] = v;
    return true;
}

bool DataAccessor::store(const std::string& vname, const int& v)
{
    std::string name = vname;
    auto* current_data = scope(name);
    if (current_data == nullptr)
    {
        return Context::store(name, v);
    }
    (*current_data)[name] = v;
    return true;
}

bool DataAccessor::store(const std::string& name, const char* v)
{
    return store(name, std::string(v));
}
bool DataAccessor::store(const std::string& vname, const std::string& v)
{
    std::string name = vname;
    auto* current_data = scope(name);
    if (current_data == nullptr)
    {
        return Context::store(name, v);
    }
    (*current_data)[name] = v;
    return true;
}

bool DataAccessor::store(const std::string& vname, const double& v)
{
    std::string name = vname;
    auto* current_data = scope(name);
    if (current_data == nullptr)
    {
        return Context::store(name, v);
    }
    (*current_data)[name] = v;
    return true;
}

bool DataAccessor::boolean(const std::string& vname,
                           size_t             index,
                           bool*              ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::boolean(name, index, ok);
    }
    if (current_data_exists)
    {
        auto       itr   = current_data->find(name);
        DataArray* array = nullptr;
        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_array();
            if (*ok)
            {
                array = itr->second.to_array();
                *ok   = array->at(index).is_primitive();
                if (*ok == false)
                    return false;
            }
        }
        if (itr->second.is_array())
        {
            array = itr->second.to_array();
            return array->at(index).to_bool();
        }
    }
    else if (parent_data_exists)
    {
        return m_parent->boolean(name, index, ok);
    }
    return std::numeric_limits<bool>::quiet_NaN();
}
bool DataAccessor::boolean(const std::string& vname, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::boolean(name, ok);
    }
    if (current_data_exists)
    {
        auto itr = current_data->find(name);

        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_primitive();
        }
        return itr->second.to_bool();
    }
    else if (parent_data_exists)
    {
        return m_parent->boolean(name, ok);
    }
    return std::numeric_limits<bool>::quiet_NaN();
}
int DataAccessor::integer(const std::string& vname, size_t index, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists =
          (m_parent && m_parent->exists(name))))
    {
        return Context::integer(name, index, ok);
    }
    if (current_data_exists)
    {
        auto       itr   = current_data->find(name);
        DataArray* array = nullptr;
        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_array();
            if (*ok)
            {
                array = itr->second.to_array();
                *ok   = array->at(index).is_primitive();
                if (*ok == false)
                    return std::numeric_limits<int>::quiet_NaN();
            }
        }
        if (itr->second.is_array())
        {
            array = itr->second.to_array();
            return array->at(index).to_int();
        }
    }
    else if (parent_data_exists)
    {
        return m_parent->integer(name, index, ok);
    }
    return std::numeric_limits<int>::quiet_NaN();
}
int DataAccessor::integer(const std::string& vname, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists =
          (m_parent && m_parent->exists(name))))
    {
        return Context::integer(name, ok);
    }
    if (current_data_exists)
    {
        auto itr = current_data->find(name);

        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_primitive();
        }
        return itr->second.to_int();
    }
    else if (parent_data_exists)
    {
        return m_parent->integer(name, ok);
    }
    return std::numeric_limits<int>::quiet_NaN();
}

double DataAccessor::real(const std::string& vname, size_t index, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::real(name, index, ok);
    }
    if (current_data_exists)
    {
        auto       itr   = current_data->find(name);
        DataArray* array = nullptr;
        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_array();
            if (*ok)
            {
                array = itr->second.to_array();
                *ok   = array->at(index).is_primitive();
                if (*ok == false)
                    return std::numeric_limits<double>::quiet_NaN();
            }
        }
        if (itr->second.is_array())
        {
            array = itr->second.to_array();
            return array->at(index).to_double();
        }
    }
    else if (parent_data_exists)
    {
        return m_parent->real(name, index, ok);
    }
    return std::numeric_limits<double>::quiet_NaN();
}
double DataAccessor::real(const std::string& vname, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::real(name, ok);
    }
    if (current_data_exists)
    {
        auto itr = current_data->find(name);

        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_primitive();
        }
        return itr->second.to_double();
    }
    else if (parent_data_exists)
    {
        return m_parent->real(name, ok);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

std::string
DataAccessor::string(const std::string& vname, size_t index, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::string(name, index, ok);
    }
    if (current_data_exists)
    {
        auto       itr   = current_data->find(name);
        DataArray* array = nullptr;
        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_array();
            if (*ok)
            {
                array = itr->second.to_array();
                *ok   = array->at(index).is_primitive();
                if (*ok == false)
                    return "";
            }
        }
        if (itr->second.is_array())
        {
            array = itr->second.to_array();
            return array->at(index).to_string();
        }
    }
    else if (parent_data_exists)
    {
        return m_parent->string(name, index, ok);
    }
    return "";
}
std::string DataAccessor::string(const std::string& vname, bool* ok) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return Context::string(name, ok);
    }
    if (current_data_exists)
    {
        auto itr = current_data->find(name);

        if (ok && itr == current_data->end())
            *ok = false;
        else if (ok)
        {
            *ok = itr->second.is_primitive() || itr->second.is_object();
        }
        return itr->second.to_string();
    }
    else if (parent_data_exists)
    {
        return m_parent->string(name, ok);
    }
    return "";
}

DataObject* DataAccessor::object(const std::string& vname) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return nullptr;
    }
    if (current_data_exists)
    {
        auto itr = current_data->find(name);
        if (itr == current_data->end())
        {
            return nullptr;
        }
        if (itr->second.is_object())
        {
            return itr->second.to_object();
        }
    }
    else if (parent_data_exists)
    {
        return m_parent->object(name);
    }
    return nullptr;
}
DataArray* DataAccessor::array(const std::string& vname) const
{
    std::string name = vname;
    const auto* current_data = scope(name);
    bool current_data_exists =
        current_data != nullptr && current_data->contains(name);
    bool parent_data_exists = false;
    if (current_data_exists == false &&
        !(parent_data_exists = (m_parent && m_parent->exists(name))))
    {
        return nullptr;
    }
    if (current_data_exists)
    {
        auto itr = current_data->find(name);
        if (itr == current_data->end())
        {
            return nullptr;
        }
        if (itr->second.is_array())
        {
            return itr->second.to_array();
        }
    }
    else if (parent_data_exists)
    {
        return m_parent->array(name);
    }
    return nullptr;
}

wasp::DataObject* DataAccessor::scope(std::string& name) const
{
    // no operator, it better be scoped by this accessor's data
    if (m_hierarchy_operator.empty()
            || m_current_data == nullptr)
    {
        return m_current_data;
    }

    std::string original_name = name;
    //
    // Search down from this accessor scope
    //
    auto* current_data = m_current_data;

    size_t i_op = 0;
    while ( (i_op = name.find(m_hierarchy_operator, i_op) )
            != std::string::npos )
    {
        std::string object_name = name.substr(0,i_op);
        // Check Object existence
        auto itr = current_data->find(object_name);
        if ( itr != current_data->end()
             && itr->second.is_object())
        {
            current_data = itr->second.to_object();
        }
        else
        {
            // could not find the next object
            // attempt to address as a missname
            if (m_current_data->find(original_name) != m_current_data->end())
            {
                name = original_name;
                return m_current_data;
            }
            return current_data;
        }
        name = name.substr(i_op+m_hierarchy_operator.size());
        i_op = 0;
    }
    return current_data;
}
}  // end of namespace
