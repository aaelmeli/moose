/*
 * File:   tstHalite.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
#include <cstdio>  // std::remove(file_name)
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "wasphalite/HaliteInterpreter.h"
#include "wasphalite/DataAccessor.h"
#include "waspcore/utils.h"
#include "waspcore/Object.h"

#include "gtest/gtest.h"
using namespace std;
using namespace wasp;

TEST(Halite, range_errors)
{
    std::vector<std::string> expected_error = {
        "no range start was specified for 'x'",
        "unable to extract range start for 'x'"  // no ',' or ';'
        ,
        "unable to extract delimited range start for 'x'"  // delimited by ','
                                                           // or ';'
        ,
        "unable to extract delimited range start for 'x'"  // delimited by ','
                                                           // or ';'
        ,
        "no range end was specified for 'x'",
        "unable to extract range end for 'x'"  // no ',' or ';'
        ,
        "unable to extract delimited range end for 'x'"  // nothing between ','
                                                         // or ';'
        ,
        "unable to extract delimited range end for 'x'"  // nothing between ','
                                                         // or ';'
        ,
        "no range stride was specified for 'x'",
        "unable to extract range stride for 'x'"  // no ',' or ';'
        ,
        "unable to extract delimited range stride for 'x'"  // delimited by ';'
    };
    std::vector<std::string> ranges = {" x=",         "x = ",      "x = ,",
                                       "x = ;",       "x = 1,",    "x = 1, ",
                                       "x = 1, ,",    "x = 1, ;",  "x = 1, 2,",
                                       "x = 1, 2, ,", "x = 1, 3,;"};
    ASSERT_EQ(expected_error.size(), ranges.size());
    for (size_t i = 0; i < expected_error.size(); ++i)
    {
        SCOPED_TRACE(expected_error[i]);
        SCOPED_TRACE(ranges[i]);
        std::vector<DefaultHaliteInterpreter::Range> imports;
        std::string                                  range = ranges[i];
        std::string                                  error;
        EXPECT_FALSE(
            DefaultHaliteInterpreter::extract_ranges(range, imports, error));
        std::cout << error << std::endl;
        ASSERT_EQ(expected_error[i], error);
    }
}

TEST(Halite, attribute_range_errors)
{
    std::vector<std::string> ranges = {
        "<x:x=>",          "<x:x = >",      "<x:x = ,>",
        "<x:x = ;>",       "<x:x = 1,>",    "<x:x = 1, >",
        "<x:x = 1, ,>",    "<x:x = 1, ;>",  "\n\n<x:x = 1, 2,>",
        "<x:x = 1, 2, ,>", "<x:x = 1, 3,;>"};
    std::vector<std::string> expected_error = {
        "1; no range start was specified for 'x'.",
        "1; no range start was specified for 'x'.",
        "1; unable to extract delimited range start for 'x'."  // delimited by
                                                               // ',' or ';'
        ,
        "1; unable to extract delimited range start for 'x'."  // delimited by
                                                               // ',' or ';'
        ,
        "1; no range end was specified for 'x'.",
        "1; no range end was specified for 'x'."  // no ',' or ';'
        ,
        "1; unable to extract delimited range end for 'x'."  // nothing between
                                                             // ',' or ';'
        ,
        "1; unable to extract delimited range end for 'x'."  // nothing between
                                                             // ',' or ';'
        ,
        "3; no range stride was specified for 'x'.",
        "1; unable to extract range stride for 'x'."  // no ',' or ';'
        ,
        "1; unable to extract delimited range stride for 'x'."  // delimited by
                                                                // ';'
    };
    ASSERT_EQ(expected_error.size(), ranges.size());
    for (size_t i = 0; i < ranges.size(); ++i)
    {
        const std::string& t = ranges[i];
        const std::string& e = expected_error[i];
        SCOPED_TRACE(t);
        SCOPED_TRACE(e);
        std::stringstream input;
        input << t;
        std::stringstream        errors;
        DefaultHaliteInterpreter interpreter(errors);
        // TODO - these should really be parse errors
        ASSERT_TRUE(interpreter.parse(input));

        std::stringstream out;
        DataAccessor      data;
        ASSERT_FALSE(interpreter.evaluate(out, data));
        ASSERT_EQ("***Error : unable to acquire attribute options on line " +
                      e + "\n",
                  errors.str());
    }
}
#include "wasphalite/test/Paths.h"
// TODO add timing for each template
struct error_test
{
    std::string template_path;
    std::string json_path;
    std::string expected_path;
    error_test(const std::string& t, const std::string& j, const std::string& e)
        : template_path(t), json_path(j), expected_path(e)
    {
    }
    error_test(const error_test& orig)
        : template_path(orig.template_path)
        , json_path(orig.json_path)
        , expected_path(orig.expected_path)
    {
    }
};

void test_template(const error_test& t)
{
    std::string tp = SOURCE_DIR + "/data/" + t.template_path;
    std::string dd = SOURCE_DIR + "/data/";
    std::string bd = BINARY_DIR + "/";
    SCOPED_TRACE(tp);
    std::string json = t.json_path.empty() ? "" : dd + t.json_path;
    SCOPED_TRACE(json);
    SCOPED_TRACE(bd + t.expected_path);
    std::stringstream expected_result;
    std::stringstream result;
    std::stringstream errors;
    bool expanded = wasp::expand_template(result, errors, errors, tp, json);
    if (errors.rdbuf()->in_avail() > 0)
        std::cout << errors.str() << std::endl;
    ASSERT_FALSE(expanded);
    ASSERT_TRUE(load_file(bd + t.expected_path, expected_result));
    ASSERT_EQ(expected_result.str(), errors.str());
}

//
// Test single template errors for their expected error messages - no data
// needed
//

TEST(Halite, file_import_file_missing)
{
    test_template(error_test("file_import_file_missing.tmpl"  // tmpl
                             ,
                             ""  // json
                             ,
                             "file_import_file_missing.result.txt"  // result
                             ));
}

TEST(Halite, unmatched_conditional_terminator)
{
    test_template(
        error_test("unmatched_conditional_terminator.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "unmatched_conditional_terminator.result.txt"  // result
                   ));
}

TEST(Halite, unmatched_conditional_elseif)
{
    test_template(
        error_test("unmatched_conditional_elseif.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "unmatched_conditional_elseif.result.txt"  // result
                   ));
}

TEST(Halite, unmatched_conditional_else)
{
    test_template(error_test("unmatched_conditional_else.tmpl"  // tmpl
                             ,
                             ""  // json
                             ,
                             "unmatched_conditional_else.result.txt"  // result
                             ));
}

TEST(Halite, undefined_attribute)
{
    test_template(error_test("undefined_attribute.tmpl"  // tmpl
                             ,
                             ""  // json
                             ,
                             "undefined_attribute.result.txt"  // result
                             ));
}

TEST(Halite, expression_evaluation_syntax_error)
{
    test_template(
        error_test("expression_evaluation_syntax_error.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "expression_evaluation_syntax_error.result.txt"  // result
                   ));
}

TEST(Halite, expression_evaluation_format_error)
{
    test_template(
        error_test("expression_evaluation_format_error.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "expression_evaluation_format_error.result.txt"  // result
                   ));
}

TEST(Halite, iterative_expression_range_error)
{
    test_template(
        error_test("iterative_expression_range_error.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "iterative_expression_range_error.result.txt"  // result
                   ));
}

TEST(Halite, scoped_expression_evaluation_error)
{
    test_template(
        error_test("scoped_expression_evaluation_error.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "scoped_expression_evaluation_error.result.txt"  // result
                   ));
}
TEST(Halite, scoped_iterative_expression_evaluation_error)
{
    test_template(error_test(
        "scoped_iterative_expression_evaluation_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "scoped_iterative_expression_evaluation_error.result.txt"  // result
        ));
}

TEST(Halite, file_repeat_error)
{
    test_template(error_test("file_repeat_error.tmpl"  // tmpl
                             ,
                             ""  // json
                             ,
                             "file_repeat_error.result.txt"  // result
                             ));
}
TEST(Halite, attribute_options_missing_delimiter_error)
{
    test_template(error_test(
        "attribute_options_missing_delimiter_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "attribute_options_missing_delimiter_error.result.txt"  // result
        ));
}
TEST(Halite, empty_attribute_substitution_error)
{
    test_template(
        error_test("empty_attribute_substitution_error.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "empty_attribute_substitution_error.result.txt"  // result
                   ));
}

//
// Test the same errors but propogated from
//
TEST(Halite, import_with_file_import_file_missing)
{
    test_template(
        error_test("import_with_file_import_file_missing.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "import_with_file_import_file_missing.result.txt"  // result
                   ));
}

TEST(Halite, import_with_unmatched_conditional_terminator)
{
    test_template(error_test(
        "import_with_unmatched_conditional_terminator.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_unmatched_conditional_terminator.result.txt"  // result
        ));
}

TEST(Halite, import_with_unmatched_conditional_elseif)
{
    test_template(error_test(
        "import_with_unmatched_conditional_elseif.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_unmatched_conditional_elseif.result.txt"  // result
        ));
}

TEST(Halite, import_with_unmatched_conditional_else)
{
    test_template(error_test(
        "import_with_unmatched_conditional_else.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_unmatched_conditional_else.result.txt"  // result
        ));
}

TEST(Halite, import_with_undefined_attribute)
{
    test_template(
        error_test("import_with_undefined_attribute.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "import_with_undefined_attribute.result.txt"  // result
                   ));
}

TEST(Halite, import_with_expression_evaluation_syntax_error)
{
    test_template(error_test(
        "import_with_expression_evaluation_syntax_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_expression_evaluation_syntax_error.result.txt"  // result
        ));
}

TEST(Halite, import_with_expression_evaluation_format_error)
{
    test_template(error_test(
        "import_with_expression_evaluation_format_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_expression_evaluation_format_error.result.txt"  // result
        ));
}

TEST(Halite, import_with_iterative_expression_range_error)
{
    test_template(error_test(
        "import_with_iterative_expression_range_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_iterative_expression_range_error.result.txt"  // result
        ));
}

TEST(Halite, import_with_scoped_expression_evaluation_error)
{
    test_template(error_test(
        "import_with_scoped_expression_evaluation_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_scoped_expression_evaluation_error.result.txt"  // result
        ));
}
TEST(Halite, import_with_scoped_iterative_expression_evaluation_error)
{
    test_template(error_test("import_with_scoped_iterative_expression_"
                             "evaluation_error.tmpl"  // tmpl
                             ,
                             ""  // json
                             ,
                             "import_with_scoped_iterative_expression_"
                             "evaluation_error.result.txt"  // result
                             ));
}

TEST(Halite, import_with_file_repeat_error)
{
    test_template(
        error_test("import_with_file_repeat_error.tmpl"  // tmpl
                   ,
                   ""  // json
                   ,
                   "import_with_file_repeat_error.result.txt"  // result
                   ));
}
TEST(Halite, import_with_attribute_options_missing_delimiter_error)
{
    test_template(error_test("import_with_attribute_options_missing_delimiter_"
                             "error.tmpl"  // tmpl
                             ,
                             ""  // json
                             ,
                             "import_with_attribute_options_missing_delimiter_"
                             "error.result.txt"  // result
                             ));
}
TEST(Halite, import_with_empty_attribute_substitution_error)
{
    test_template(error_test(
        "import_with_empty_attribute_substitution_error.tmpl"  // tmpl
        ,
        ""  // json
        ,
        "import_with_empty_attribute_substitution_error.result.txt"  // result
        ));
}
