#ifndef HALITE_DATA_ACCESSOR_H
#define HALITE_DATA_ACCESSOR_H

#include "waspcore/Object.h"
#include "waspexpr/ExprContext.h"
#include "waspcore/decl.h"

namespace wasp
{
/**
 * @brief The DataAccessor class is design as the intersection of data, the
 * halite engine, and the expression engine
 */
class WASP_PUBLIC DataAccessor : public Context
{
  public:
    DataAccessor(DataObject* data = nullptr, DataAccessor* parent = nullptr,
                 const std::string& hierarchy_operator="");
    DataAccessor(const DataAccessor& orig);
    virtual ~DataAccessor();

    virtual bool exists(const std::string& name) const;
    /// type getters - note objects are returned as Context::Type::STRING
    virtual Context::Type type(const std::string& name) const;
    virtual Context::Type type(const std::string& name, size_t index) const;

    /**
     * @brief size aquire the size of the given variable
     * @param name the variable for which the size is requested
     * @return the size of the variable... 0 if not a vector/map, etc.
     */
    virtual int size(const std::string& name) const;

    virtual bool store(const std::string& name, const bool& v);
    virtual bool store(const std::string& name, const int& v);
    virtual bool store(const std::string& name, const char* v);
    virtual bool store(const std::string& name, const std::string& v);
    virtual bool store(const std::string& name, const double& v);

    /// Boolean data getters
    virtual bool
    boolean(const std::string& name, size_t index, bool* ok = nullptr) const;
    virtual bool boolean(const std::string& name, bool* ok = nullptr) const;

    /// integer data getters
    virtual int
    integer(const std::string& name, size_t index, bool* ok = nullptr) const;
    virtual int integer(const std::string& name, bool* ok = nullptr) const;

    /// double data getters
    virtual double
    real(const std::string& name, size_t index, bool* ok = nullptr) const;
    virtual double real(const std::string& name, bool* ok = nullptr) const;

    /// string data getters - note Objects are returned as a 'packed' json
    /// (single line representation)
    virtual std::string
    string(const std::string& name, size_t index, bool* ok = nullptr) const;
    virtual std::string string(const std::string& name,
                               bool*              ok = nullptr) const;

    /**
     * @brief object acquire an object with the given name
     * @param name the name of the desired object to acquire
     * @return the object, iff an object with the given name exists, nullptr
     * otherwise
     */
    DataObject* object(const std::string& name) const;

    /**
     * @brief array acquire an array with the given name
     * @param name the name of the desired array to acquire
     * @return the array, iff an array with the given name exists, nullptr
     * otherwise
     */
    DataArray* array(const std::string& name) const;

    /**
     * @brief hierarchy_operator obtains the operator that dictates hierachy in
     * the variable name
     * @return returns the string indicating the hierarchy operator, default '.'
     */
    const std::string& hierarchy_operator() const {return m_hierarchy_operator;}

protected:
    /**
     * @brief scope acquire the DataObject scope for the given variable
     * @param name the name of the variable, updated to be the lowest scope
     * @return the DataObject for the variable, minimally this.
     * Note if given child.value, and child is an object,
     * child is returned and name is updated to be value.
     */
    DataObject* scope(std::string& name) const;
  private:    
    /**
     * @brief m_parent unmanaged data pointer to parent data layer
     */
    DataAccessor* m_parent;
    /**
     * @brief m_current_data unmanaged data pointer to current data hierarchy
     */
    DataObject* m_current_data;

    /**
     * @brief m_hierarchy_operator the character(s) that delimit variable hierarchy
     * E.g.,
     * operator = ".", e.g., parent.child.
     * operator = "::" e.g., parent::child.
     */
    std::string m_hierarchy_operator;
};

}  // wasp
#endif
