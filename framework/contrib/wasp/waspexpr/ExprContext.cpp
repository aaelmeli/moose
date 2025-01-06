#include "waspexpr/ExprContext.h"

namespace wasp
{
bool Result::format(std::ostream& out) const
{
    if (is_real())
    {
        out << real();
    }
    else if (is_integer())
    {
        out << integer();
    }
    else if (is_bool())
    {
        out << boolean();
    }
    else if (is_string() || is_error())
    {
        out << string();
    }
    else
    {
        return false;
    }
    return true;
}

bool Result::format(std::ostream&      out,
                    const std::string& fmt,
                    std::ostream&      err) const
{
    bool result = true;
    if (is_real())
    {
        result = wasp::Format::fmt(out, err, fmt.c_str(), real());
    }
    else if (is_integer())
    {
        result = wasp::Format::fmt(out, err, fmt.c_str(), integer());
    }
    else if (is_bool())
    {
        out << boolean();
    }
    else if (is_string() || is_error())
    {
        result = wasp::Format::fmt(out, err, fmt.c_str(), string());
    }
    else
    {
        result = false;
    }
    return result;
}

void Context::clear()
{
    for (auto v : m_variables)
        delete v.second;
    m_variables.clear();
    for (auto f : m_functions)
        delete f.second;
    m_functions.clear();
}

Context& Context::add_default_variables()
{
    store("e", 2.7182818284590452353602874713527);
    store("pi", 3.14159265359);
    store("nl", "\n");
    return *this;
}

Context& Context::add_default_functions()
{
    wasp_check(function_exists("sin") == false);
    add_function("sin", new FSin());
    wasp_check(function_exists("asin") == false);
    add_function("asin", new FASin());
    wasp_check(function_exists("asinh") == false);
    add_function("asinh", new FASinh());
    wasp_check(function_exists("cos") == false);
    add_function("cos", new FCos());
    wasp_check(function_exists("acos") == false);
    add_function("acos", new FACos());
    wasp_check(function_exists("acosh") == false);
    add_function("acosh", new FACosh());
    wasp_check(function_exists("tan") == false);
    add_function("tan", new FTan());
    wasp_check(function_exists("atan2") == false);
    add_function("atan2", new FATan2());
    wasp_check(function_exists("atan") == false);
    add_function("atan", new FATan());
    wasp_check(function_exists("atanh") == false);
    add_function("atanh", new FATanh());
    wasp_check(function_exists("sinh") == false);
    add_function("sinh", new FSinh());
    wasp_check(function_exists("cosh") == false);
    add_function("cosh", new FCosh());
    wasp_check(function_exists("Tanh") == false);
    add_function("tanh", new FTanh());

    wasp_check(function_exists("floor") == false);
    add_function("floor", new FFloor());
    wasp_check(function_exists("ceil") == false);
    add_function("ceil", new FCeil());
    wasp_check(function_exists("exp") == false);
    add_function("exp", new FExp());
    wasp_check(function_exists("log") == false);
    add_function("log", new FLog());
    wasp_check(function_exists("lg") == false);
    add_function("lg", new FLg());
    wasp_check(function_exists("log10") == false);
    add_function("log10", new FLog10());
    wasp_check(function_exists("sec") == false);
    add_function("sec", new FSec());
    wasp_check(function_exists("csc") == false);
    add_function("csc", new FCsc());
    wasp_check(function_exists("cot") == false);
    add_function("cot", new FCot());
    wasp_check(function_exists("sqrt") == false);
    add_function("sqrt", new FSqrt());
    wasp_check(function_exists("pow") == false);
    add_function("pow", new FPow());

    wasp_check(function_exists("deg2rad") == false);
    add_function("deg2rad", new FDeg2Rad());
    wasp_check(function_exists("rad2deg") == false);
    add_function("rad2deg", new FRad2Deg());

    wasp_check(function_exists("grad2deg") == false);
    add_function("grad2deg", new FGrad2Deg());
    wasp_check(function_exists("deg2grad") == false);
    add_function("deg2grad", new FDeg2Grad());

    wasp_check(function_exists("round") == false);
    add_function("round", new FRound());
    wasp_check(function_exists("roundn") == false);
    add_function("roundn", new FRoundn());
    wasp_check(function_exists("min") == false);
    add_function("min", new FMin());
    wasp_check(function_exists("max") == false);
    add_function("max", new FMax());
    wasp_check(function_exists("abs") == false);
    add_function("abs", new FAbs());
    wasp_check(function_exists("mod") == false);
    add_function("mod", new FMod());
    wasp_check(function_exists("fmt") == false);
    add_function("fmt", new FFmt());

    // popular variants
    wasp_check(function_exists("cosec") == false);
    add_function("cosec", new FCsc());
    wasp_check(function_exists("ln") == false);
    add_function("ln", new FLog());
    return *this;
}

bool Context::function_exists(const std::string& name) const
{
    return m_functions.find(name) != m_functions.end();
}

}  // end of namespace
