#ifndef WASPCORE_FORMAT_H
#define WASPCORE_FORMAT_H
#include "waspcore/wasp_bug.h"
#include "waspcore/decl.h"
#include <ctype.h>
#include <cmath>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>  // windows exponent configure

namespace wasp
{
/**
 * @brief The Format class is a static class to privatize implementation details
 */
class WASP_PUBLIC Format
{
  public:
    /**
     * @brief format variadic template function for type safe printf/Java.Format
     * like capability
     * @param out - the output stream to emit the formatted result
     * @param err - the error stream if an error occurs
     * @param s - the format string
     * @param value the argument to print
     * @param args the additional arguments to print
     * NOTE : format(std::cout, "%%4.8f=|%4.8f|",3.14159265)  => "%4.1f=| 3.1|"
     */
    template<typename T>
    static bool
    fmt(std::ostream& out, std::ostream& err, const char* s, const T& value)
    {
// if Windows, only output two-digits of exponent if possible like linux
#ifdef _WIN32
#if (defined(_MSC_VER) && _MSC_VER < 1900) || (defined(__GNUC__))
        unsigned int old_exponent_format;
        old_exponent_format = _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
#endif
        int         width              = out.width();
        int         prec               = out.precision();
        static char conversion_types[] = "dfsge";
        bool        error_occurred     = false;
        while (*s && error_occurred == false)
        {
            // are we encountering a possible format statement
            // %[flags][width][.precision]conversion is lexed and parsed where
            //   flags : justification, sign, padding and parenthesis
            //   '-' The result will be left-justified
            //   '+' The result will always include a sign
            //   ' ' The result will include a leading space for positive values
            //   '0' The result will be zero-padded
            //   '(' The result will enclose negative numbers in parentheses
            //   width : [1-9][0-9]* (cannot start with 0)
            bool include_sign = false;  // true when '%+...' is specified
            bool include_lead_space_for_positive =
                false;  // true when '% ...' is specified
            bool include_parenthesis_for_negative =
                false;  // true when '%(...' is specified
            if (*s == '%')
            {
                // Check for escaped formatting '%%'
                const char* sl = s + 1;  // string lookahead char
                if (*sl == '%')
                {
                    out << *sl;  // emit escaped '%'
                    s = sl + 1;  // move beyond
                    continue;
                }
                while (*sl)
                {
                    // check for flag
                    if (*sl == '-')
                    {  // %-..., left justified
                        out << std::left;
                        ++sl;
                    }
                    else if (*sl == '+')
                    {  // %+..., include plus for positive numbers
                        include_sign = true;
                        ++sl;
                    }
                    else if (*sl == ' ')
                    {  // include leading space for positive numbers
                        include_lead_space_for_positive = true;
                        ++sl;
                    }
                    else if (*sl == '0')
                    {  // pad space with zeros
                        out.fill('0');
                        out << std::internal;
                        ++sl;
                    }
                    else if (*sl == '(')
                    {  // surround negative numbers with parenthesis
                        include_parenthesis_for_negative = true;
                        ++sl;
                    }
                    else if (*sl == '.')  // check for %.'precision''conversion'
                    {                     // no width, just precision
                        ++sl;             // move beyond '.'
                        const char* p =
                            sl;  // cache location of the start of the precision
                        // isdigit returns non-zero when character is a digit
                        while (sl && isdigit(*sl))
                        {
                            ++sl;  // move forward
                        }
                        // check for '%.conversion'| '%.NULL' - missing
                        // precision
                        if (p == sl)
                        {  // if no difference in character pointers, error
                            err << "***Error : missing precision format "
                                   "statement at index "
                                << (sl - p);
                            error_occurred = true;
                            break;
                        }
                        // the precision should be terminated by the type
                        // 'conversion'
                        // check that the 'conversion' is supported
                        if (std::strchr(conversion_types, *sl) == nullptr)
                        {
                            err << "***Error : unsupported format conversion "
                                   "type '"
                                << *sl << "', must be one of "
                                << conversion_types;
                            error_occurred = true;
                            break;
                        }
                        // capture the precision
                        std::stringstream prec_substr(std::string(p, sl));
                        prec_substr >> prec;
                        wasp_check(!prec_substr.bad() || !prec_substr.fail());
                    }
                    else if (isdigit(*sl))  // check for width
                    {
                        const char* w = sl;  // cache beginning of width substr
                        ++sl;
                        while (isdigit(*sl))
                        {
                            ++sl;
                        }
                        // capture the precision
                        std::string       substr(w, sl);
                        std::stringstream width_substr(substr);
                        width_substr >> width;
                        wasp_check(!width_substr.bad() || !width_substr.fail());
                    }
                    else if (std::strchr(conversion_types, *sl) != nullptr)
                    {
                        // a format expression has been consumed, emit the value
                        error_occurred = !fmt_emit(
                            out, err, *sl, value, width, prec,
                            include_parenthesis_for_negative,
                            include_lead_space_for_positive, include_sign);
                        break;  // break from lookahead loop
                    }
                    else
                    {
                        err << "***Error : '" << *sl
                            << "' is an unknown format element";
                        error_occurred = true;
                        break;
                    }
                }        // end of format lookahead loop
                s = sl;  // move forward the lookahead amount
                // ensure we concluded on a conversion type
                if (std::strchr(conversion_types, *sl) == nullptr)
                {
                    err << " - format type conversion is missing.";
                    error_occurred = true;
                    break;
                }
                ++s;
            }
            else
            {
                // emit intermediate characters
                out << *s++;
            }
        }  // eo while loop
#ifdef _WIN32
#if (defined(_MSC_VER) && _MSC_VER < 1900) || (defined(__GNUC__))
        _set_output_format(old_exponent_format);
#endif
#endif
        return error_occurred == false;
    }  // end of format

  private:
    template<typename T>
    static bool fmt_emit(std::ostream& out,
                         std::ostream& err,
                         char          format,
                         const T&      value,
                         int           width,
                         int           prec,
                         bool          include_parenthesis_for_negative,
                         bool          include_lead_space_for_positive,
                         bool          include_sign)
    {
        switch (format)
        {
            case 'e':
                out << std::scientific;
                break;
            case 'f':
                out << std::fixed;
                break;
            case 'g':
            {
                if (std::is_fundamental<T>::value == false)
                {
                    err << "format argument is not a fundamental type";
                    return false;
                }
                int magnitude = std::floor(std::log10(value));
                if (magnitude < -4 && prec >= std::abs(magnitude))
                {
                    out << std::scientific;
                    --prec;
                }
                else if (magnitude <= -4)
                {
                    out << std::fixed;
                    prec += 3;
                }
                else
                {
                    out << std::fixed;
                    if (prec > 0)
                        prec -= magnitude + 1;
                }
                break;
            }
            case 's':
                // nothing special to see here ... yet
                break;
            case 'd':
                out << std::fixed;
                prec = 0;  // float -> integer
                break;
        }

        if (include_parenthesis_for_negative && value < 0)
        {
            width -= 3;  // account for ()
            if (width < 0)
                width = 0;
            out << std::setw(width) << std::setprecision(prec);
            out << "(" << -value << ")";
        }
        else if (include_sign && value > 0)
        {
            width -= 2;  // account for '+'
            if (width < 0)
                width = 0;
            out << std::setw(width) << std::setprecision(prec);
            out << '+' << value;
        }
        else if (include_lead_space_for_positive && value > 0)
        {
            // only adjust for leading ws if no width already
            width -= 2;
            if (width < 0)
                width = 0;
            out << std::setw(width) << std::setprecision(prec);
            out << " " << value;
        }
        else
        {
            out << std::setw(width) << std::setprecision(prec);
            out << value;
        }
        return true;
    }  // end of fmt_emit

};  // end of class Foramt

template<>
// string specialization
inline bool Format::fmt_emit<std::string>(std::ostream&      out,
                                          std::ostream&      err,
                                          char               format,
                                          const std::string& value,
                                          int                width,
                                          int                prec,
                                          bool include_parenthesis_for_negative,
                                          bool include_lead_space_for_positive,
                                          bool include_sign)
{
    switch (format)
    {
        case 'e':
        case 'f':
        case 'g':
        case 'd':
        {
            err << "invalid value type 'String' for format type '" << format
                << "'";
            return false;
        }
    }
    if (include_parenthesis_for_negative)
    {
        err << "malformed format string: flag '(' does not match the "
               "conversion 's'";
        return false;
    }
    if (include_lead_space_for_positive)
    {
        err << "malformed format string: flag ' ' does not match the "
               "conversion 's'";
        return false;
    }
    if (include_sign)
    {
        err << "malformed format string: flag '+' does not match the "
               "conversion 's'";
        return false;
    }

    out << std::setw(width) << std::setprecision(prec);
    out << value;
    return true;
}  // end of fmt_emit<string>
}  // end of namespace
#endif
