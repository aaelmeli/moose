#include "waspdakota/ParametersFile.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
using namespace wasp;

TEST(Dakota, param1)
{
    std::stringstream input;
    input << R"I(                                         14 variables
             0        ddriv_1
             1        ddriv_2
             5        ddsiv_1
             7        ddsiv_2
3.400000000000000e+00 ddsrv_1
7.700000000000000e+00 ddsrv_2
2.666191841707838e+02 TF1n
6.574953891737252e+02 TF2n
            11        dsriv_1
            10        dsriv_2
             4        dssiv_1
             1        dssiv_2
1.200000000000000e+00 dssrv_1
8.800000000000001e+00 dssrv_2
             3 functions
             1 ASV_1:response_fn_1
             1 ASV_2:response_fn_2
             1 ASV_3:response_fn_3
             2 derivative_variables
             7 DVV_1:TF1n
             8 DVV_2:TF2n
             0 analysis_components
             1 eval_id
)I" << std::endl;
    ParametersFile params;
    ASSERT_TRUE(params.load(input,std::cerr));

    { // test variable

    std::vector<std::string> label = {"ddriv_1",
                                      "ddriv_2",
                                      "ddsiv_1",
                                      "ddsiv_2",
                                      "ddsrv_1",
                                      "ddsrv_2",
                                      "TF1n",
                                      "TF2n",
                                      "dsriv_1",
                                      "dsriv_2",
                                      "dssiv_1",
                                      "dssiv_2",
                                      "dssrv_1",
                                      "dssrv_2"};
    std::vector<std::string> value = {"0",
                                      "1",
                                      "5",
                                      "7",
                                      "3.400000000000000e+00",
                                      "7.700000000000000e+00",
                                      "2.666191841707838e+02",
                                      "6.574953891737252e+02",
                                      "11",
                                      "10",
                                      "4",
                                      "1",
                                      "1.200000000000000e+00",
                                      "8.800000000000001e+00"};

    ASSERT_EQ(label.size(), params.variable_count());
    ASSERT_EQ(label.size(), value.size());
    for (size_t i = 0; i < label.size(); ++i)
    {
        SCOPED_TRACE(i);
        SCOPED_TRACE(label[i]);
        ASSERT_EQ(label[i], params.variable_label(i));
        ASSERT_EQ(value[i], params.variable_value(i));
    }
    } // end of variable testing
    { // test function

    std::vector<std::string> label = {"response_fn_1",
                                      "response_fn_2",
                                      "response_fn_3"};
    std::vector<std::string> value = {"1",
                                      "1",
                                      "1"};

    ASSERT_EQ(label.size(), params.function_count());
    ASSERT_EQ(label.size(), value.size());
    for (size_t i = 0; i < label.size(); ++i)
    {
        SCOPED_TRACE(i);
        SCOPED_TRACE(label[i]);
        ASSERT_EQ(label[i], params.function_label(i));
        ASSERT_EQ(value[i], params.function_value(i));
    }
    } // end of function testing

    { // test derivative variables

    std::vector<std::string> label = {"TF1n",
                                      "TF2n"};
    std::vector<std::string> value = {"7",
                                      "8"};

    ASSERT_EQ(label.size(), params.derivative_variable_count());
    ASSERT_EQ(label.size(), value.size());
    for (size_t i = 0; i < label.size(); ++i)
    {
        SCOPED_TRACE(i);
        SCOPED_TRACE(label[i]);
        ASSERT_EQ(label[i], params.derivative_variable_label(i));
        ASSERT_EQ(value[i], params.derivative_variable_value(i));
    }
    } // end of derivative testing

    { // test analysis component variables
    ASSERT_EQ(0, params.analysis_component_count());
    } // end of derivative testing
    {
    ASSERT_EQ("1", params.eval_id());
    }
}

TEST(Dakota, unknown_block)
{
    std::stringstream input;
    input << R"I(                                         1 variables
             0        ddriv_1
             0 fowbles
             1 eval_id
)I" << std::endl;
    ParametersFile params;
    std::stringstream errors;
    ASSERT_FALSE(params.load(input,errors));
    std::stringstream expected;
    expected<<"***Error: line 3, fowbles is an unknown parameter block!\n"
              "\t Expecting functions, derivative_variables, or analysis_components.\n";
    ASSERT_EQ(expected.str(), errors.str());
}

TEST(Dakota, nagative_count_block)
{
    std::stringstream input;
    input << R"I(                                         -1 variables
             0        ddriv_1
             0 fowbles
             1 eval_id
)I" << std::endl;
    ParametersFile params;
    std::stringstream errors;
    ASSERT_FALSE(params.load(input,errors));
    std::stringstream expected;
    expected<<"***Error: line 1, variable count must be non-negative, but was provided as -1\n";
    ASSERT_EQ(expected.str(), errors.str());
}
