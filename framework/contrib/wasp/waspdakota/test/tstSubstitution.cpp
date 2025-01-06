/*
 * File:   tstSubstitute.cpp
 * Author: Robert A. Lefebvre
 */

#include <cstdlib>
#include <cstdio>  // std::remove(file_name)
#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "waspcore/Object.h"
#include "waspcore/utils.h"
#include "waspdakota/ParametersFile.h"

#include "waspdakota/test/Paths.h"

#include "gtest/gtest.h"

using namespace std;
using namespace wasp;

struct test
{
    std::string template_path;
    std::string param_path;
    std::string expected_path;
    test(const std::string& t, const std::string& p, const std::string& e)
        : template_path(t), param_path(p), expected_path(e)
    {
    }
    test(const test& orig)
        : template_path(orig.template_path)
        , param_path(orig.param_path)
        , expected_path(orig.expected_path)
    {
    }
};

void test_template(const test& t)
{
    std::string tp = SOURCE_DIR + "/data/" + t.template_path;
    std::string dd = SOURCE_DIR + "/data/";
    std::string bd = SOURCE_DIR + "/data/";
    SCOPED_TRACE(tp);
    std::string params = t.param_path.empty() ? "" : dd + t.param_path;
    SCOPED_TRACE(params);
    SCOPED_TRACE(bd + t.expected_path);
    std::stringstream expected_result;
    std::stringstream result;
    std::stringstream errors;
    bool expanded = wasp::substitute_template(result, errors, errors, tp,
                                              params, false, false,
                                              "<",">",".");

    if (errors.rdbuf()->in_avail() > 0)
        std::cout << errors.str() << std::endl;
    ASSERT_TRUE(expanded);
    ASSERT_TRUE(load_file(bd + t.expected_path, expected_result));
    ASSERT_EQ(expected_result.str(), result.str());
}


TEST(Dakota, substitute)
{
    SCOPED_TRACE("simple.tmpl");
    test_template(test("simple.tmpl"  // tmpl
                       ,
                       "params.in.1"  // parameters file
                       ,
                       "simple.result.txt"  // result
                       ));
}
