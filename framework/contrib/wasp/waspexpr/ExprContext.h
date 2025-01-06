#ifndef WASP_EXPRCONTEXT_H
#define WASP_EXPRCONTEXT_H

#include "waspcore/wasp_bug.h"
#include "waspcore/Format.h"
#include <cmath>
#include <limits>
#include <sstream>
#include <vector>
#include <map>
#include <type_traits>
#include "waspcore/utils.h"
#include "waspcore/decl.h"
#include "waspcore/wasp_node.h"
#include <algorithm>

namespace wasp
{
/**
 * @brief The Context class provides variable capability
 */
class WASP_PUBLIC Context
{
  public:
    // type of the context
    enum Type : unsigned char
    {
        BOOLEAN,
        INTEGER,
        REAL,
        STRING,
        WEC_ERROR,
        UNDEFINED
    };
    Context() {}
    Context(const Context& orig) {}
    virtual ~Context() { clear(); }

    /**
     * @brief add_default_variables add set of default variables
     * @return this Context with addition of variables
     * Default variables are :
     * 'pi' - pi ~3.14 or (2 * std::acos(0.0))
     * 'e' - 2.7182818284590452353602874713527
     */
    Context& add_default_variables();
    /**
     * @brief add_default_variables add set of default functions
     * @return this Context with addition of functions
     * Default functions are :
     * sin(x) - the sine is a trigonometric function of an angle (radians)
     */
    Context& add_default_functions();
    /**
     * @brief clear deletes all associated variables and function
     */
    void clear();

    virtual Context::Type type(const std::string& name) const
    {
        auto* v = variable(name);
        if (v == nullptr)
            return Context::Type::UNDEFINED;
        return v->type();
    }
    /**
     * @brief type request the type of the element at the given index
     * @param name the name of the array
     * @param index the index into the array
     * @return the type of the element at the array
     * Note : Base Context implementation of array's are contain a single type
     * This interface allows subclasses of Context to work with heterogenously
     * typed elements
     */
    virtual Context::Type type(const std::string& name, size_t index) const
    {
        auto* v = variable(name);
        if (v == nullptr)
            return Context::Type::UNDEFINED;
        return v->type();  // base vectors are always homogeneously typed
    }

    virtual int size(const std::string& name) const
    {
        auto* v = variable(name);
        if (v)
            return v->size();
        return 0;
    }

    virtual bool
    boolean(const std::string& name, size_t index, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->boolean(index, ok);
    }
    virtual bool boolean(const std::string& name, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->boolean(ok);
    }
    virtual int
    integer(const std::string& name, size_t index, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->integer(index, ok);
    }
    virtual int integer(const std::string& name, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->integer(ok);
    }
    virtual double
    real(const std::string& name, size_t index, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->real(index, ok);
    }
    virtual double real(const std::string& name, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->real(ok);
    }
    virtual std::string
    string(const std::string& name, size_t index, bool* ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->string(index, ok);
    }
    virtual std::string string(const std::string& name,
                               bool*              ok = nullptr) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->string(ok);
    }

    virtual bool store(const std::string& name, size_t index, int v) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->store(index, v);
    }
    virtual bool store(const std::string& name, size_t index, double v) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->store(index, v);
    }
    virtual bool store(const std::string& name, size_t index, bool v) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->store(index, v);
    }
    virtual bool
    store(const std::string& name, size_t index, const std::string& v) const
    {
        auto* var = variable(name);
        wasp_require(var);
        return var->store(index, v);
    }
    virtual bool exists(const std::string& name) const
    {
        // TODO - ensure it is not nullptr
        return m_variables.find(name) != m_variables.end();
    }
    bool store_ref(const std::string& name, bool& v)
    {
        return store_ref<VarRefBool>(name, v);
    }
    bool store_ref(const std::string& name, int& v)
    {
        return store_ref<VarRefInt>(name, v);
    }
    bool store_ref(const std::string& name, double& v)
    {
        return store_ref<VarRefReal>(name, v);
    }
    bool store_ref(const std::string& name, std::string& v)
    {
        return store_ref<VarRefString>(name, v);
    }
    template<typename DT>
    bool store_ref(const std::string& name, std::vector<DT>& v)
    {
        return store_ref<VarRefVector<DT>>(name, v);
    }
    virtual bool store(const std::string& name, const bool& v)
    {
        return store_ref<VarBool>(name, v);
    }
    virtual bool store(const std::string& name, const int& v)
    {
        return store_ref<VarInt>(name, v);
    }
    virtual bool store(const std::string& name, const double& v)
    {
        return store_ref<VarReal>(name, v);
    }
    virtual bool store(const std::string& name, const char* v)
    {
        return store_ref<VarString>(name, v);
    }
    virtual bool store(const std::string& name, const std::string& v)
    {
        return store_ref<VarString>(name, v);
    }
    template<typename DT>
    bool store(const std::string& name, const std::vector<DT>& v)
    {
        return store_ref<VarVector<DT>>(name, v);
    }
    bool function_exists(const std::string& name) const;

    /**
     * @brief add_function adds the given function to this context
     * @param name the name of the function (e.g., 'sin', 'cos')
     * @param f the function pointer to reference upon evaluation
     * @return this context with addition of function
     * If the function f already exists the
     */
    Context& add_function(const std::string& name, class Function* f)
    {
        wasp_require(function_exists(name) == false);
        m_functions[name] = f;
        return *this;
    }

    class Function* function(const std::string& name) const
    {
        auto itr = m_functions.find(name);
        if (itr == m_functions.end())
            return nullptr;
        return itr->second;
    }

  private:
    /**
     * @brief The Variable class is an abstract interface for dealing with
     */
    class Variable
    {
      public:
        virtual Context::Type type() const = 0;
        virtual int           size() const { return 0; }
        virtual ~Variable() {}
        virtual int integer(bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring integer value");
        }
        virtual double real(bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring double precision value");
        }
        virtual bool boolean(bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring boolean value");
        }
        virtual std::string string(bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring string value");
        }
        virtual int integer(size_t i, bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring integer value at index");
        }
        virtual double real(size_t i, bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring double precision value at index");
        }
        virtual bool boolean(size_t i, bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring boolean value at index");
        }
        virtual std::string string(size_t i, bool* ok = nullptr) const
        {
            if (ok != nullptr)
                *ok = false;
            wasp_not_implemented("acquiring string value at index");
        }
        virtual bool store(bool v) { return false; }
        virtual bool store(int v) { return false; }
        virtual bool store(double v) { return false; }
        virtual bool store(const std::string& v) { return false; }
        virtual bool store(size_t i, bool v) { return false; }
        virtual bool store(size_t i, int v) { return false; }
        virtual bool store(size_t i, double v) { return false; }
        virtual bool store(size_t i, const std::string& v) { return false; }
    };
    class VarRefInt : public Variable
    {
      public:
        VarRefInt(int& i) : v(i) {}
        Context::Type  type() const { return Context::Type::INTEGER; }
        bool boolean(bool* ok) const
        {
            if (ok)
                *ok = true;
            return bool(v);
        }
        int integer(bool* ok) const
        {
            if (ok)
                *ok = true;
            return v;
        }
        double real(bool* ok) const
        {
            if (ok)
                *ok = true;
            return double(v);
        }
        std::string string(bool* ok) const { return to_string(v, ok); }

        bool store(bool v)
        {
            this->v = v;
            return true;
        }
        bool store(int v)
        {
            this->v = v;
            return true;
        }
        bool store(double v)
        {
            this->v = v;
            return true;
        }

      private:
        int& v;
    };
    class VarInt : public VarRefInt
    {
      public:
        VarInt(int i) : VarRefInt(v), v(i) {}

      private:
        int v;
    };

    class VarRefBool : public Variable
    {
      public:
        VarRefBool(bool& b) : v(b) {}
        Context::Type    type() const { return Context::Type::BOOLEAN; }
        bool boolean(bool* ok) const
        {
            if (ok)
                *ok = true;
            return bool(v);
        }
        int integer(bool* ok) const
        {
            if (ok)
                *ok = true;
            return v;
        }
        double real(bool* ok) const
        {
            if (ok)
                *ok = true;
            return double(v);
        }
        std::string string(bool* ok) const { return to_string(v, ok); }

        bool store(bool v)
        {
            this->v = v;
            return true;
        }
        bool store(int v)
        {
            this->v = v;
            return true;
        }
        bool store(double v)
        {
            this->v = v;
            return true;
        }

      private:
        bool& v;
    };
    class VarBool : public VarRefBool
    {
      public:
        VarBool(bool b) : VarRefBool(v), v(b) {}

      private:
        bool v;
    };
    class VarRefReal : public Variable
    {
      public:
        VarRefReal(double& d) : v(d) {}
        Context::Type      type() const { return Context::Type::REAL; }
        bool boolean(bool* ok) const
        {
            if (ok)
                *ok = true;
            return bool(v);
        }
        int integer(bool* ok) const
        {
            if (ok)
                *ok = true;
            return int(v);
        }
        double real(bool* ok) const
        {
            if (ok)
                *ok = true;
            return double(v);
        }
        std::string string(bool* ok) const { return to_string(v, ok); }

        bool store(bool v)
        {
            this->v = v;
            return true;
        }
        bool store(int v)
        {
            this->v = v;
            return true;
        }
        bool store(double v)
        {
            this->v = v;
            return true;
        }

      private:
        double& v;
    };
    class VarReal : public VarRefReal
    {
      public:
        VarReal(double d) : VarRefReal(v), v(d) {}

      private:
        double v;
    };
    class VarRefString : public Variable
    {
      public:
        VarRefString(std::string& s) : v(s) {}
        Context::Type             type() const { return Context::Type::STRING; }
        std::string string(bool* ok) const
        {
            if (ok)
                *ok = true;
            return v;
        }

        bool store(bool v)
        {
            bool k  = false;
            this->v = to_string(v, &k);
            return k;
        }
        bool store(int v)
        {
            bool k  = false;
            this->v = to_string(v, &k);
            return k;
        }
        bool store(double v)
        {
            bool k  = false;
            this->v = to_string(v, &k);
            return k;
        }
        bool store(const std::string& v)
        {
            this->v = v;
            return true;
        }

      private:
        std::string& v;
    };
    class VarString : public VarRefString
    {
      public:
        VarString(const std::string& s) : VarRefString(v), v(s) {}

      private:
        std::string v;
    };
    template<class T>
    class VarRefVector : public Variable
    {
      public:
        VarRefVector(std::vector<T>& v) : v(v) {}
        int                          size() const { return v.size(); }
        Context::Type                type() const { return type(v); }
        Context::Type                type(const std::vector<int>&) const
        {
            return Context::Type::INTEGER;
        }
        Context::Type type(const std::vector<double>&) const
        {
            return Context::Type::REAL;
        }
        Context::Type type(const std::vector<std::string>&) const
        {
            return Context::Type::STRING;
        }

        int integer(size_t i, bool* ok) const
        {
            if (ok)
                *ok = true;
            return get(i, v);
        }
        double real(size_t i, bool* ok) const
        {
            if (ok)
                *ok = true;
            return get(i, v);
        }
        std::string string(size_t i, bool* ok) const
        {
            return to_string(get(i, v), ok);
        }

        int get(size_t i, const std::vector<int>& d) const
        {
            wasp_require(i < d.size());
            return d[i];
        }
        double get(size_t i, const std::vector<double>& d) const
        {
            wasp_require(i < d.size());
            return d[i];
        }
        std::string get(size_t i, const std::vector<std::string>& d) const
        {
            wasp_require(i < d.size());
            return d[i];
        }
        bool store(size_t i, int vv) { return store_t(i, vv); }
        bool store(size_t i, double vv) { return store_t(i, vv); }
        //            void store(size_t i, const std::string& vv){
        //                store_t(i,vv);
        //            }
        bool store_t(size_t i, const T& vv)
        {
            if (this->v.size() < i)
                this->v.resize(i + 1);
            this->v[i] = vv;
            return true;
        }

      private:
        std::vector<T>& v;
    };
    template<class T>
    class VarVector : public VarRefVector<T>
    {
      public:
        VarVector(const T& d) : VarRefVector<T>(v), v(d) {}
        Context::Type      type() const { return type(v); }
        Context::Type      type(const std::vector<int>&)
        {
            return Context::Type::INTEGER;
        }
        Context::Type type(const std::vector<double>&)
        {
            return Context::Type::REAL;
        }
        Context::Type type(const std::vector<std::string>&)
        {
            return Context::Type::STRING;
        }

      private:
        T v;
    };

  private:
    template<class T, class V>
    typename std::enable_if<std::is_pod<V>::value, bool>::type
    store_ref(const std::string& name, V& v)
    {
        auto* existing_var = variable(name);
        if (existing_var != nullptr)
        {  // check if new variable is compatible
            return existing_var->store(v);
        }
        auto* ptr         = new T(v);
        m_variables[name] = ptr;
        return ptr != nullptr;
    }
    template<class T, class V>
    typename std::enable_if<!std::is_pod<V>::value, bool>::type
    store_ref(const std::string& name, V& v)
    {
        auto* existing_var = variable(name);
        if (existing_var != nullptr)
        {
            return false;
        }  // cannot assign complex types (arrays, etc)
        auto* ptr         = new T(v);
        m_variables[name] = ptr;
        return ptr != nullptr;
    }
    Variable* variable(const std::string& name) const
    {
        auto itr = m_variables.find(name);
        if (itr == m_variables.end())
            return nullptr;
        return itr->second;
    }
    // variables
    std::map<std::string, Variable*>       m_variables;
    std::map<std::string, class Function*> m_functions;

};  // end of class Context

/**
 * @brief The Result class simple class for evaluating an expression
 */
class WASP_PUBLIC Result
{
  public:
    Result() : m_type(Context::Type::UNDEFINED) {}
    Result(const Result& orig)
        : m_type(orig.m_type), m_value(orig.m_value), m_string(orig.m_string)
    {
    }
    ~Result() {}

    template<class T>
    Result& evaluate(const T& tree_view, Context& context);
    bool format(std::ostream& out) const;
    bool
    format(std::ostream& out, const std::string& fmt, std::ostream& err) const;
    std::string as_string() const
    {
        if (is_bool())
            return "bool(" + std::to_string(boolean()) + ")";
        if (is_integer())
            return "int(" + std::to_string(integer()) + ")";
        if (is_real())
            return "double(" + std::to_string(real()) + ")";
        if (is_string())
            return "string(" + string() + ")";
        if (is_error())
            return "error(" + string() + ")";
        return "unknown!";
    }

  private:
    template<class TV>
    bool binary_recurse(const TV& left,
                        Result&   right_op,
                        const TV& right,
                        Context&  context)
    {
        // evaluate this result as the left operation
        wasp_require(!left.is_null());
        wasp_require(!right.is_null());
        evaluate(left, context);
        if (is_error())
        {
            return false;
        }
        right_op.evaluate(right, context);
        if (right_op.is_error())
        {
            m_type   = Context::Type::WEC_ERROR;
            string() = right_op.string();
            return false;
        }
        return true;
    }

    bool and_expr(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() && a.number();
        }
        else if (is_bool() && a.is_bool())
        {
            m_value.m_bool = boolean() && a.boolean();
        }
        else
        {
            // NOT IMPLEMENTED
            m_value.m_bool = false;
        }
        m_type = Context::Type::BOOLEAN;
        return m_value.m_bool;
    }
    bool or_expr(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() || a.number();
        }
        else if (is_bool() && a.is_bool())
        {
            m_value.m_bool = boolean() || a.boolean();
        }
        else
        {
            // NOT IMPLEMENTED
            m_value.m_bool = false;
        }
        m_type = Context::Type::BOOLEAN;
        return m_value.m_bool;
    }
    bool not_equal(const Result& a)
    {
        m_value.m_bool = !equal(a);
        m_type         = Context::Type::BOOLEAN;
        return m_value.m_bool;
    }
    bool equal(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() == a.number();
        }
        else if (is_string() && a.is_string())
        {
            m_value.m_bool = string() == a.string();
        }
        else if (is_bool() && a.is_bool())
        {
            m_value.m_bool = boolean() == a.boolean();
        }
        else
        {
            m_value.m_bool = false;
        }

        m_type = Context::Type::BOOLEAN;
        return m_value.m_bool;
    }
    bool less(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() < a.number();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }
        if (is_string() && a.is_string())
        {
            m_value.m_bool = string() < a.string();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }

        return false;
    }
    bool less_or_equal(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() <= a.number();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }
        if (is_string() && a.is_string())
        {
            m_type         = Context::Type::BOOLEAN;
            m_value.m_bool = string() <= a.string();
            return m_value.m_bool;
        }
        return false;
    }
    bool greater(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() > a.number();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }
        if (is_string() && a.is_string())
        {
            m_value.m_bool = string() > a.string();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }
        return false;
    }
    bool greater_or_equal(const Result& a)
    {
        if (is_number() && a.is_number())
        {
            m_value.m_bool = number() >= a.number();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }
        if (is_string() && a.is_string())
        {
            m_value.m_bool = string() >= a.string();
            m_type         = Context::Type::BOOLEAN;
            return m_value.m_bool;
        }
        return false;
    }
    Result& pow(const Result& a)
    {
        if (is_integer())
        {
            // integer ^ interger
            if (a.is_integer())
            {
                m_value.m_int = std::pow(integer(), a.integer());
            }
            // integer ^ real
            else if (a.is_real())
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = std::pow(integer(), a.real());
            }
            // integer ^ string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(integer()) + a.string();
            }
        }
        else if (is_real())
        {
            // real ^ integer
            if (a.is_integer())
            {
                m_value.m_real = std::pow(real(), a.integer());
            }
            // real ^ real
            else if (a.is_real())
            {
                m_value.m_real = std::pow(real(), a.real());
            }
            // real ^ string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(real()) + a.string();
            }
        }
        else if (is_string())
        {
            // string ^ integer
            if (a.is_integer())
            {
                string() += std::to_string(a.integer());
            }
            // string ^ real
            else if (a.is_real())
            {
                string() += std::to_string(a.real());
            }
            // string ^ string
            else if (a.is_string())
            {
                string() += a.string();
            }
        }
        else
        {
            wasp_not_implemented("operation with unknown type...");
        }
        return *this;
    }

    Result& plus(const Result& a)
    {
        if (is_integer())
        {
            // integer + interger
            if (a.is_integer())
            {
                m_value.m_int += a.integer();
            }
            // integer + real
            else if (a.is_real())
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = integer() + a.real();
            }
            // integer + string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(integer()) + a.string();
            }
        }
        else if (is_real())
        {
            // real + integer
            if (a.is_integer())
            {
                m_value.m_real += a.integer();
            }
            // real + real
            else if (a.is_real())
            {
                m_value.m_real += a.real();
            }
            // real + string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(real()) + a.string();
            }
        }
        else if (is_string())
        {
            // string + integer
            if (a.is_integer())
            {
                string() += std::to_string(a.integer());
            }
            // string + real
            else if (a.is_real())
            {
                string() += std::to_string(a.real());
            }
            // string + string
            else if (a.is_string())
            {
                string() += a.string();
            }
        }
        else
        {
            wasp_not_implemented("operation with unknown type...");
        }
        return *this;
    }

    Result& minus(const Result& a)
    {
        if (is_integer())
        {
            // integer - interger
            if (a.is_integer())
            {
                m_value.m_int -= a.integer();
            }
            // integer - real
            else if (a.is_real())
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = integer() - a.real();
            }
            // integer - string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(integer()) + "-" + a.string();
            }
        }
        else if (is_real())
        {
            // real - integer
            if (a.is_integer())
            {
                m_value.m_real -= a.integer();
            }
            // real - real
            else if (a.is_real())
            {
                m_value.m_real -= a.real();
            }
            // real - string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(real()) + a.string();
            }
        }
        else if (is_string())
        {
            // string - integer
            if (a.is_integer())
            {
                string() += "-" + std::to_string(a.integer());
            }
            // string - real
            else if (a.is_real())
            {
                string() += "-" + std::to_string(a.real());
            }
            // string - string
            else if (a.is_string())
            {
                string() += "-" + a.string();
            }
        }
        else
        {
            wasp_not_implemented("operation with unknown type...");
        }
        return *this;
    }
    Result& div(const Result& a)
    {
        if (is_integer())
        {
            // integer / interger
            if (a.is_integer())
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = integer() / float(a.integer());
            }
            // integer / real
            else if (a.is_real())
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = integer() / a.real();
            }
            // integer / string
            else if (a.is_string())
            {
                bool   ok     = false;
                double result = integer() / a.to_real(&ok);
                // fall back to string concat
                if (!ok)
                {
                    m_type   = Context::Type::STRING;
                    string() = std::to_string(integer()) + "/" + a.string();
                }
                else
                {
                    m_type         = Context::Type::REAL;
                    m_value.m_real = result;
                }
            }
        }
        else if (is_real())
        {
            // real / integer
            if (a.is_integer())
            {
                m_value.m_real /= a.integer();
            }
            // real / real
            else if (a.is_real())
            {
                m_value.m_real /= a.real();
            }
            // real / string
            else if (a.is_string())
            {
                bool   ok     = false;
                double result = real() / a.to_real(&ok);
                // fall back to string concat
                if (!ok)
                {
                    m_type   = Context::Type::STRING;
                    string() = std::to_string(real()) + "/" + a.string();
                }
                else
                {
                    m_type         = Context::Type::REAL;
                    m_value.m_real = result;
                }
            }
        }
        else if (is_string())
        {
            bool   lok    = false;
            bool   rok    = false;
            double result = to_real(&lok) / a.to_real(&rok);
            if (!lok || !rok)
            {
                m_type = Context::Type::STRING;
                string() += "/" + a.to_string();
            }
            else
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = result;
            }
        }
        else
        {
            wasp_not_implemented("operation with unknown type...");
        }
        return *this;
    }
    Result& mult(const Result& a)
    {
        if (is_integer())
        {
            // integer * interger
            if (a.is_integer())
            {
                m_value.m_int *= a.integer();
            }
            // integer * real
            else if (a.is_real())
            {
                m_type         = Context::Type::REAL;
                m_value.m_real = integer() * a.real();
            }
            // integer * string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(integer()) + a.string();
            }
        }
        else if (is_real())
        {
            // real * integer
            if (a.is_integer())
            {
                m_value.m_real *= a.integer();
            }
            // real * real
            else if (a.is_real())
            {
                m_value.m_real *= a.real();
            }
            // real * string
            else if (a.is_string())
            {
                m_type   = Context::Type::STRING;
                string() = std::to_string(real()) + a.string();
            }
        }
        else if (is_string())
        {
            // string * integer
            if (a.is_integer())
            {
                string() += std::to_string(a.integer());
            }
            // string * real
            else if (a.is_real())
            {
                string() += std::to_string(a.real());
            }
            // string * string
            else if (a.is_string())
            {
                string() += a.string();
            }
        }
        else
        {
            wasp_not_implemented("operation with unknown type...");
        }

        return *this;
    }
    Result& unary_not()
    {
        if (is_bool())
        {
            m_value.m_bool = !m_value.m_bool;
            m_type         = Context::Type::BOOLEAN;
        }
        else if (is_integer())
        {
            m_value.m_bool = !bool(integer());
            m_type         = Context::Type::BOOLEAN;
        }
        else if (is_real())
        {
            m_value.m_bool = !bool(real());
            m_type         = Context::Type::BOOLEAN;
        }
        else if (is_string())
        {
            string() = "!" + string();
            m_type   = Context::Type::STRING;
        }

        return *this;
    }
    Result& unary_minus()
    {
        if (is_integer())
        {
            m_value.m_int = -(integer());
        }
        else if (is_real())
        {
            m_value.m_real = -(real());
        }
        else if (is_string())
        {
            string() = "-" + string();
        }
        return *this;
    }

  public:
    int    integer() const { return m_value.m_int; }
    double real() const { return m_value.m_real; }
    double number() const
    {
        switch (m_type)
        {
            case Context::Type::INTEGER:
                return m_value.m_int;
            case Context::Type::REAL:
                return m_value.m_real;
            case Context::Type::BOOLEAN:
                return boolean();
            default:
                break;
        }
        return std::numeric_limits<double>::quiet_NaN();
    }
    double to_real(bool* ok = nullptr) const
    {
        double result = 1.0;
        if (is_string())
        {
            to_type(result, string(), ok);
            wasp_tagged_line("acquiring '" << string() << "' as a double "
                                           << result);
        }
        else
        {
            wasp_tagged_line("acquiring value as a number...");
            result = number();
            if (ok)
                *ok = true;
        }
        return result;
    }
    bool boolean() const { return m_value.m_bool; }
    bool to_bool() const
    {
        switch (m_type)
        {
            case Context::Type::BOOLEAN:
                return boolean();
            case Context::Type::INTEGER:
            case Context::Type::REAL:
                return number() != 0.0;
            case Context::Type::STRING:
                return string() != "false";
            case Context::Type::WEC_ERROR:
                return false;
            default:
            case Context::Type::UNDEFINED:
                wasp_not_implemented("converting undefined data to boolean");
        }
        return false;
    }
    const std::string& string() const { return (m_string); }

    std::string& string() { return (m_string); }

    std::string to_string(bool* ok = nullptr) const
    {
        if (ok)
            *ok = true;
        switch (m_type)
        {
            case Context::Type::BOOLEAN:
                return boolean() ? "1" : "0";
            case Context::Type::INTEGER:
                return std::to_string(integer());
            case Context::Type::REAL:
                return std::to_string(real());
            case Context::Type::STRING:
                return string();
            case Context::Type::WEC_ERROR:
                if (ok)
                    *ok = false;
                return "";
            default:
            case Context::Type::UNDEFINED:
                wasp_not_implemented("converting undefined data to string");
        }
        if (ok)
            *ok = false;
        return "";
    }
    bool is_bool() const { return m_type == Context::Type::BOOLEAN; }
    bool is_integer() const { return m_type == Context::Type::INTEGER; }
    bool is_real() const { return m_type == Context::Type::REAL; }
    bool is_number() const
    {
        return m_type == Context::Type::INTEGER ||
               m_type == Context::Type::REAL;
    }
    bool is_error() const { return m_type == Context::Type::WEC_ERROR; }
    bool is_string() const { return m_type == Context::Type::STRING; }

  private:
    Context::Type m_type;
    union Value
    {
        bool   m_bool;
        int    m_int;
        double m_real;
    } m_value;
    std::string m_string;

    template<class T>
    std::string error_msg(const T& tree_view, const std::string& msg_str)
    {
        std::stringstream msg;
        msg << "***Error : " << tree_view.name();
        // if the view is a leaf, incorporate the leaf's data in the message.
        // This should improve context
        if (tree_view.child_count() == 0)
        {
            msg << " (" << tree_view.data() << ")";
        }
        msg << " at line " << tree_view.line() << " and column "
            << tree_view.column() << " - " << msg_str << std::endl;
        return msg.str();
    }
};  // end of Result
/**
 * @brief The Function class wraps Context accessible functions
 */
class WASP_PUBLIC Function
{
  public:
    typedef std::vector<Result> Args;
    virtual Context::Type       type() const = 0;
    virtual ~Function() {}
    virtual int
    integer(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::integer");
    }
    virtual double
    real(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::real");
    }
    virtual bool
    boolean(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::boolean");
    }
    virtual std::string
    string(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::string");
    }
};
class WASP_PUBLIC RealFunction : public Function
{
  public:
    typedef std::vector<Result> Args;
    virtual Context::Type       type() const { return Context::Type::REAL; }
    virtual int
    integer(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        return real(args, err, ok);
    }
    virtual double
    real(const Args& args, std::ostream& err, bool* ok = nullptr) const = 0;
    virtual bool
    boolean(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        return real(args, err, ok);
    }
    virtual std::string
    string(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        return to_string(real(args, err, ok));
    }
};
class WASP_PUBLIC IntegerFunction : public Function
{
  public:
    typedef std::vector<Result> Args;
    virtual Context::Type       type() const { return Context::Type::INTEGER; }
    virtual int
    integer(const Args& args, std::ostream& err, bool* ok = nullptr) const = 0;
    virtual double
    real(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        return integer(args, err, ok);
    };
    virtual bool
    boolean(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        return integer(args, err, ok);
    }
    virtual std::string
    string(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        return to_string(integer(args, err, ok));
    }
};
class WASP_PUBLIC StringFunction : public Function
{
  public:
    typedef std::vector<Result> Args;
    virtual Context::Type       type() const { return Context::Type::STRING; }
    virtual int
    integer(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::integer");
    }
    virtual double
    real(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::real");
    };
    virtual bool
    boolean(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        if (ok != nullptr)
            *ok = false;
        wasp_not_implemented("Function::boolean");
    }
    virtual std::string
    string(const Args& args, std::ostream& err, bool* ok = nullptr) const = 0;
};
class WASP_PUBLIC FFmt : public StringFunction
{
  public:
    typedef std::vector<Result> Args;
    std::string
    string(const Args& args, std::ostream& err, bool* ok = nullptr) const
    {
        // must have two arguments
        if (args.size() != 2)
        {
            if (ok != nullptr)
                *ok = false;
            err << "function expects 1 argument, given " << args.size() << ".";
            return "";
        }
        // second arguement must be a string
        if (!args[1].is_string())
        {
            if (ok != nullptr)
                *ok = false;
            err << "function second argument is not a string";
            return "";
        }
        std::stringstream fout, ferr;
        // format must return true
        if (!args[0].format(fout, args[1].string(), ferr))
        {
            if (ok != nullptr)
                *ok = false;
            err << ferr.str();
            return "";
        }
        // return result
        if (ok != nullptr)
            *ok = true;
        return fout.str();
    }
};
#define WASP_INTEGER_FUNCTION_1ARG(NAME, XTENS, CALL)                          \
    class WASP_PUBLIC NAME : public XTENS                                      \
    {                                                                          \
      public:                                                                  \
        typedef std::vector<Result> Args;                                      \
        virtual int                                                            \
        integer(const Args& args, std::ostream& err, bool* ok = nullptr) const \
        {                                                                      \
            bool l_ok = true;                                                  \
            if (args.size() != 1)                                              \
            {                                                                  \
                l_ok = false;                                                  \
                err << "function expects 1 argument, given " << args.size()    \
                    << ".";                                                    \
            }                                                                  \
            else if (!args.front().is_number())                                \
            {                                                                  \
                l_ok = false;                                                  \
                err << "function expects a number";                            \
            }                                                                  \
            if (ok != nullptr)                                                 \
                *ok       = l_ok;                                              \
            const auto& a = args.front();                                      \
            if (a.is_number())                                                 \
            {                                                                  \
                return CALL;                                                   \
            }                                                                  \
            return std::numeric_limits<int>::quiet_NaN();                      \
        }                                                                      \
    };

#define WASP_REAL_FUNCTION_1ARG(NAME, XTENS, CALL)                          \
    class WASP_PUBLIC NAME : public XTENS                                   \
    {                                                                       \
      public:                                                               \
        typedef std::vector<Result> Args;                                   \
        virtual double                                                      \
        real(const Args& args, std::ostream& err, bool* ok = nullptr) const \
        {                                                                   \
            bool l_ok = true;                                               \
            if (args.size() != 1)                                           \
            {                                                               \
                l_ok = false;                                               \
                err << "function expects 1 argument, given " << args.size() \
                    << ".";                                                 \
            }                                                               \
            else if (!args.front().is_number())                             \
            {                                                               \
                l_ok = false;                                               \
                err << "function expects a number";                         \
            }                                                               \
            if (ok != nullptr)                                              \
                *ok       = l_ok;                                           \
            const auto& a = args.front();                                   \
            if (a.is_number())                                              \
            {                                                               \
                return CALL;                                                \
            }                                                               \
            return std::numeric_limits<double>::quiet_NaN();                \
        }                                                                   \
    };

WASP_REAL_FUNCTION_1ARG(FSin, RealFunction, std::sin(a.number()))
WASP_REAL_FUNCTION_1ARG(FASin, RealFunction, std::asin(a.number()))
WASP_REAL_FUNCTION_1ARG(FASinh, RealFunction, std::asinh(a.number()))
WASP_REAL_FUNCTION_1ARG(FCos, RealFunction, std::cos(a.number()))
WASP_REAL_FUNCTION_1ARG(FACos, RealFunction, std::acos(a.number()))
WASP_REAL_FUNCTION_1ARG(FACosh, RealFunction, std::acosh(a.number()))
WASP_REAL_FUNCTION_1ARG(FTan, RealFunction, std::tan(a.number()))
WASP_REAL_FUNCTION_1ARG(FATan, RealFunction, std::atan(a.number()))
WASP_REAL_FUNCTION_1ARG(FATanh, RealFunction, std::atanh(a.number()))

WASP_REAL_FUNCTION_1ARG(FSinh, RealFunction, std::sinh(a.number()))
WASP_REAL_FUNCTION_1ARG(FCosh, RealFunction, std::cosh(a.number()))
WASP_REAL_FUNCTION_1ARG(FTanh, RealFunction, std::tanh(a.number()))

WASP_REAL_FUNCTION_1ARG(FFloor, RealFunction, std::floor(a.number()))
WASP_REAL_FUNCTION_1ARG(FCeil, RealFunction, std::ceil(a.number()))
WASP_REAL_FUNCTION_1ARG(FExp, RealFunction, std::exp(a.number()))
WASP_REAL_FUNCTION_1ARG(FLog, RealFunction, std::log(a.number()))
WASP_REAL_FUNCTION_1ARG(FLg, RealFunction, std::log2(a.number()))
WASP_REAL_FUNCTION_1ARG(FLog10, RealFunction, std::log10(a.number()))
WASP_REAL_FUNCTION_1ARG(FSec, RealFunction, 1.0 / std::sin(a.number()))
WASP_REAL_FUNCTION_1ARG(FCsc, RealFunction, 1.0 / std::cos(a.number()))
WASP_REAL_FUNCTION_1ARG(FCot, RealFunction, 1.0 / std::tan(a.number()))
WASP_REAL_FUNCTION_1ARG(FSqrt, RealFunction, std::sqrt(a.number()))
WASP_REAL_FUNCTION_1ARG(FDeg2Rad,
                        RealFunction,
                        2.0 * std::acos(0.0) / 180.0 * a.number())
WASP_REAL_FUNCTION_1ARG(FRad2Deg,
                        RealFunction,
                        180.0 / (2.0 * std::acos(0.0)) * a.number())

WASP_REAL_FUNCTION_1ARG(FGrad2Deg, RealFunction, (10.0 / 9.0) * a.number())
WASP_REAL_FUNCTION_1ARG(FDeg2Grad, RealFunction, (9.0 / 10.0) * a.number())
WASP_REAL_FUNCTION_1ARG(FRound, RealFunction, std::floor(0.5 + a.number()))
WASP_REAL_FUNCTION_1ARG(FAbs, RealFunction, std::abs(a.number()))

#define WASP_REAL_FUNCTION_2ARG(NAME, XTENS, CALL)                           \
    class WASP_PUBLIC NAME : public XTENS                                    \
    {                                                                        \
      public:                                                                \
        typedef std::vector<Result> Args;                                    \
        virtual double                                                       \
        real(const Args& args, std::ostream& err, bool* ok = nullptr) const  \
        {                                                                    \
            bool l_ok = true;                                                \
            if (args.size() != 2)                                            \
            {                                                                \
                l_ok = false;                                                \
                err << "function expects 2 argument, given " << args.size()  \
                    << ".";                                                  \
            }                                                                \
            else if (!args.front().is_number() || !args.front().is_number()) \
            {                                                                \
                l_ok = false;                                                \
                err << "function expect numbers";                            \
            }                                                                \
            if (ok != nullptr)                                               \
                *ok        = l_ok;                                           \
            const auto& a1 = args.front();                                   \
            const auto& a2 = args.back();                                    \
            return CALL;                                                     \
        }                                                                    \
    };

WASP_REAL_FUNCTION_2ARG(FATan2,
                        RealFunction,
                        std::atan2(a1.number(), a2.number()))
WASP_REAL_FUNCTION_2ARG(FPow, RealFunction, std::pow(a1.number(), a2.number()))
WASP_REAL_FUNCTION_2ARG(
    FRoundn,
    RealFunction,
    std::floor((a1.number() * std::pow(10, std::floor(a2.number())) + 0.5)) /
        std::pow(10, std::floor(a2.number())))
WASP_REAL_FUNCTION_2ARG(FMin, RealFunction, std::min(a1.number(), a2.number()))
WASP_REAL_FUNCTION_2ARG(FMax, RealFunction, std::max(a1.number(), a2.number()))

#define WASP_INTEGER_FUNCTION_2ARG(NAME, XTENS, CALL)                          \
    class WASP_PUBLIC NAME : public XTENS                                      \
    {                                                                          \
      public:                                                                  \
        typedef std::vector<Result> Args;                                      \
        virtual int                                                            \
        integer(const Args& args, std::ostream& err, bool* ok = nullptr) const \
        {                                                                      \
            bool l_ok = true;                                                  \
            if (args.size() != 2)                                              \
            {                                                                  \
                l_ok = false;                                                  \
                err << "function expects 2 argument, given " << args.size()    \
                    << ".";                                                    \
            }                                                                  \
            else if (!args.front().is_number() || !args.front().is_number())   \
            {                                                                  \
                l_ok = false;                                                  \
                err << "function expect numbers";                              \
            }                                                                  \
            if (ok != nullptr)                                                 \
                *ok        = l_ok;                                             \
            const auto& a1 = args.front();                                     \
            const auto& a2 = args.back();                                      \
            return CALL;                                                       \
        }                                                                      \
    };
WASP_INTEGER_FUNCTION_2ARG(FMod, IntegerFunction, a1.integer() % a2.integer())

template<class T>
inline Result& Result::evaluate(const T& tree_view, Context& context)
{
    // if an ambiguous 'value' node
    // determine operation via the token's type
    size_t type = tree_view.type();
    if (type == wasp::VALUE)
        type = tree_view.token_type();

    // TODO - capture OK values from to_type conversions
    // and present the RESULT as an WEC_ERROR if OK=false
    switch (type)
    {
        default:
        case wasp::UNKNOWN:
            m_type   = Context::Type::WEC_ERROR;
            string() = error_msg(tree_view, "unable to interpret");
            break;
        case wasp::INTEGER:
            m_type        = Context::Type::INTEGER;
            m_value.m_int = tree_view.to_int();
            break;
        case wasp::REAL:
            m_type         = Context::Type::REAL;
            m_value.m_real = tree_view.to_double();
            break;
        case wasp::PARENTHESIS:
            evaluate(tree_view.child_at(1), context);
            break;
        case wasp::UNARY_MINUS:
            evaluate(tree_view.child_at(1), context);
            unary_minus();
            break;
        case wasp::UNARY_NOT:
            evaluate(tree_view.child_at(1), context);
            unary_not();
            break;
        case wasp::WASP_AND:
        {
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            and_expr(right_op);
            break;
        }
        case wasp::WASP_OR:
        {
            evaluate(tree_view.child_at(0), context);
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            or_expr(right_op);
            break;
        }
        case wasp::LT:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            less(right_op);
            break;
        }
        case wasp::LTE:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            less_or_equal(right_op);
            break;
        }
        case wasp::GT:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            greater(right_op);
            break;
        }
        case wasp::GTE:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            greater_or_equal(right_op);
            break;
        }
        case wasp::EQ:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            equal(right_op);
            break;
        }
        case wasp::NEQ:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            not_equal(right_op);
            break;
        }
        case wasp::PLUS:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            plus(right_op);
            break;
        }
        case wasp::MINUS:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            minus(right_op);
            break;
        }
        case wasp::MULTIPLY:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            mult(right_op);
            break;
        }
        case wasp::EXPONENT:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            pow(right_op);
            break;
        }
        case wasp::DIVIDE:
        {
            // evaluate this result as the left operation
            Result right_op;
            if (!binary_recurse(tree_view.child_at(0), right_op,
                                tree_view.child_at(2), context))
                break;
            div(right_op);
            break;
        }
        case wasp::QUOTED_STRING:
            // check if quoted string is actually quoted variable
            wasp_tagged_line("dealing with quoted string '"
                             << tree_view.to_string()
                             << "' determining if variable ?" << std::boolalpha
                             << (context.type(tree_view.to_string()) !=
                                 Context::Type::UNDEFINED));

            if (context.type(tree_view.to_string()) == Context::Type::UNDEFINED)
            {
                m_type   = Context::Type::STRING;
                string() = tree_view.to_string();
                break;
            }  // else quoted string falls through to variable logic below
        case wasp::STRING:
        {
            std::string name = tree_view.to_string();  // removes quotes if any
                                                       // from the fall-through
                                                       // logic above
            auto var_type = context.type(name);
            if (var_type == Context::Type::UNDEFINED)
            {
                wasp_tagged_line("undefined type error");
                m_type   = Context::Type::WEC_ERROR;
                string() = error_msg(tree_view, "is not a known variable.");
            }
            else
            {
                m_type = var_type;
                switch (m_type)
                {  // switch on current Result's type
                    case Context::Type::BOOLEAN:
                        m_value.m_bool = context.boolean(name);
                        break;
                    case Context::Type::INTEGER:
                        m_value.m_int = context.integer(name);
                        break;
                    case Context::Type::REAL:
                        m_value.m_real = context.real(name);
                        break;
                    case Context::Type::STRING:
                        string() = context.string(name);
                        break;
                    default:
                        // not implemented
                        break;
                }
            }
            break;
        }
        case wasp::OBJECT:
        {
            const std::string var_name = tree_view.name();
            if (!context.exists(var_name))
            {
                wasp_tagged_line("undefined type error");
                m_type   = Context::Type::WEC_ERROR;
                string() = error_msg(tree_view, "is not a known variable.");
            }
            wasp_check(tree_view.child_count() > 2);
            const auto& index_view = tree_view.child_at(2);
            if (evaluate(index_view, context).m_type ==
                Context::Type::WEC_ERROR)
            {
                break;
            }
            if (!is_number())
            {
                wasp_tagged_line("illegal index type error");
                m_type = Context::Type::WEC_ERROR;
                string() =
                    error_msg(index_view, "is not an integral value so it "
                                          "cannot be used as an index.");
                break;
            }
            size_t        index    = integer();
            Context::Type var_type = context.type(var_name, index);
            if (var_type == Context::Type::UNDEFINED)
            {
                wasp_tagged_line("undefined type error");
                m_type   = Context::Type::WEC_ERROR;
                string() = error_msg(tree_view,
                                     "is undefined at index " +
                                         std::to_string(index) + ".");
            }
            // name [ index ]  - 4 children
            else if (tree_view.child_count() == 4)
            {
                m_type = var_type;

                switch (m_type)
                {  // switch on current Result's type
                    case Context::Type::BOOLEAN:
                        m_value.m_bool = context.boolean(var_name, index);
                        break;
                    case Context::Type::INTEGER:
                        m_value.m_int = context.integer(var_name, index);
                        break;
                    case Context::Type::REAL:
                        m_value.m_real = context.real(var_name, index);
                        break;
                    case Context::Type::STRING:
                        string() = context.string(var_name, index);
                        break;
                    default:
                        wasp_not_implemented(
                            "unknown index variable type value acquisition");
                        break;
                }

            }  // end of name [ index ]
            // name [ index ] = value
            else if (tree_view.child_count() == 6)
            {
                Result      value;
                const auto& value_view = tree_view.child_at(5);
                value.evaluate(value_view, context);

                m_type = var_type;

                switch (value.m_type)
                {  // switch on current value's type
                    case Context::Type::BOOLEAN:
                        m_value.m_bool = value.boolean();
                        context.store(var_name, index, boolean());
                        break;
                    case Context::Type::INTEGER:
                        m_value.m_int = value.integer();
                        context.store(var_name, index, integer());
                        break;
                    case Context::Type::REAL:
                        m_value.m_real = value.real();
                        context.store(var_name, index, real());
                        break;
                    case Context::Type::STRING:
                        string() = value.string();
                        context.store(var_name, index, string());
                        break;
                    case Context::Type::WEC_ERROR:
                        m_type   = value.m_type;
                        string() = value.string();
                        break;
                    default:
                        wasp_tagged_line("unknown object error");
                        m_type   = Context::Type::WEC_ERROR;
                        string() = error_msg(tree_view,
                                             "is an unknown object scenario.");
                        break;
                }
            }
            else
            {
                wasp_tagged_line("unknown object reference pattern");
                m_type   = Context::Type::WEC_ERROR;
                string() = error_msg(
                    tree_view, "is not a known object reference pattern.");
                break;
            }
            break;
        }
        case wasp::FUNCTION:
        {
            wasp_check(tree_view.child_count() > 0);
            const auto& name_view     = tree_view.child_at(0);
            std::string function_name = name_view.data();
            wasp_check(tree_view.child_count() > 1);
            // reserved function with special
            if (function_name == "defined")
            {
                if (tree_view.child_count() < 4)
                {
                    m_type   = Context::Type::WEC_ERROR;
                    string() = error_msg(
                        tree_view,
                        "reserved function 'defined' requires an argument!");
                    break;
                }
                bool variable_defined = true;
                for (size_t c = 2, count = tree_view.child_count() - 1;
                     c < count; ++c)
                {
                    const auto& child_view = tree_view.child_at(c);
                    if (child_view.is_decorative() ||
                        child_view.type() == wasp::WASP_COMMA)
                        continue;
                    if (!context.exists(child_view.to_string()))
                    {
                        variable_defined = false;
                        break;
                    }
                }

                m_type         = Context::Type::BOOLEAN;
                m_value.m_bool = variable_defined;
                break;
            }
            else if (function_name == "size")
            {
                if (tree_view.child_count() != 4)
                {
                    m_type   = Context::Type::WEC_ERROR;
                    string() = error_msg(
                        tree_view,
                        "reserved function 'size' requires a single argument!");
                    break;
                }
                const auto& child_view = tree_view.child_at(2);
                m_type                 = Context::Type::INTEGER;
                m_value.m_int          = context.size(child_view.to_string());
                break;
            }
            else if (function_name == "if")
            {
                // 'if' '(' a1 ',' a2 ',' a3 ')'
                if (tree_view.child_count() != 8)
                {
                    m_type = Context::Type::WEC_ERROR;
                    string() =
                        error_msg(tree_view, "reserved function 'if' requires "
                                             "a 3 argument. if ( condition, if "
                                             "true, if false).");
                    break;
                }
                const auto& a1 = tree_view.child_at(2);
                if (evaluate(a1, context).is_error())
                {
                    break;
                }
                if (to_bool())
                {
                    const auto& a2 = tree_view.child_at(4);
                    evaluate(a2, context);
                }
                else
                {
                    const auto& a3 = tree_view.child_at(6);
                    evaluate(a3, context);
                }
                break;
            }
            // functions are of the form
            // name '(' [arg1 [',' argn]] ')'
            // we can traverse range [2,c-1]
            std::vector<Result> function_args;
            bool                function_args_error = false;
            for (size_t c = 2, count = tree_view.child_count() - 1; c < count;
                 ++c)
            {
                const auto& child_view = tree_view.child_at(c);
                if (child_view.is_decorative() ||
                    child_view.type() == wasp::WASP_COMMA)
                    continue;
                function_args.push_back(Result());
                function_args.back().evaluate(child_view, context);
                if (function_args.back().is_error())
                {
                    function_args_error = true;
                    m_type              = Context::Type::WEC_ERROR;
                    string()            = function_args.back().string();
                    break;
                }
            }
            // function arguments contain error
            if (function_args_error)
            {
                break;
            }
            Function* function = context.function(function_name);
            if (function == nullptr)
            {
                m_type   = Context::Type::WEC_ERROR;
                string() = error_msg(
                    tree_view, "function " + function_name + " does not exist");
                break;
            }

            auto f_type = function->type();
            m_type      = f_type;
            std::stringstream errs;
            bool              eval_ok = true;
            if (f_type == Context::Type::INTEGER)
            {
                m_value.m_int =
                    function->integer(function_args, errs, &eval_ok);
            }
            else if (f_type == Context::Type::REAL)
            {
                m_value.m_real = function->real(function_args, errs, &eval_ok);
            }
            else if (f_type == Context::Type::STRING)
            {
                string() = function->string(function_args, errs, &eval_ok);
            }
            else if (f_type == Context::Type::BOOLEAN)
            {
                m_value.m_bool =
                    function->boolean(function_args, errs, &eval_ok);
            }
            else
            {
                wasp_not_implemented("unknown function return type evaluation");
            }

            if (eval_ok == false)
            {
                m_type = Context::Type::WEC_ERROR;
                string() =
                    error_msg(tree_view, "unable to interpret, " + errs.str());
            }
            break;
        }  // end of function
        case wasp::KEYED_VALUE:
        {
            std::string variable_name = tree_view.name();
            evaluate(tree_view.child_at(2), context);
            auto var_type     = context.type(variable_name);
            bool new_variable = var_type == Context::Type::UNDEFINED;
            if (new_variable == false)
            {
                switch (m_type)
                {  // switch on current Result's type
                    case Context::Type::BOOLEAN:
                        context.store(variable_name, boolean());
                        break;
                    case Context::Type::INTEGER:
                        context.store(variable_name, integer());
                        break;
                    case Context::Type::REAL:
                        context.store(variable_name, real());
                        break;
                    case Context::Type::STRING:
                        context.store(variable_name, string());
                        break;
                    default:
                        // not implemented
                        break;
                }
            }  // end of new variable store
            else
            {
                switch (m_type)
                {  // switch on current Result's type
                    case Context::Type::BOOLEAN:
                        context.store(variable_name, boolean());
                        break;
                    case Context::Type::INTEGER:
                        context.store(variable_name, integer());
                        break;
                    case Context::Type::REAL:
                        context.store(variable_name, real());
                        break;
                    case Context::Type::STRING:
                        context.store(variable_name, string());
                        break;
                    default:
                        // not implemented
                        break;
                }
            }
            break;
        }
        case wasp::DOCUMENT_ROOT:
            // evaluate all children, storing only last result
            // TODO - implement
            for (size_t i = 0; i < tree_view.child_count(); ++i)
            {
                evaluate(tree_view.child_at(i), context);
            }
            break;
    }  // end of switch

    return *this;
}

}  // end of namespace

#endif
