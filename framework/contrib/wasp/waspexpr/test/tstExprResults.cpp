#include "waspexpr/ExprInterpreter.h"
#include "waspexpr/ExprContext.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <cmath>
using namespace wasp;

template<typename T>
struct ScalarExprTest
{
    std::string tst;
    T           expected;
    ScalarExprTest(const std::string& tst, T expected)
        : tst(tst), expected(expected)
    {
    }
};
template<typename V1, typename V2, typename T>
struct VariableExprTest
{
    std::string tst;
    V1          v1;
    V2          v2;
    T           expected;
    VariableExprTest(const std::string& tst, V1 v1, V2 v2, T expected)
        : tst(tst), v1(v1), v2(v2), expected(expected)
    {
    }
};
TEST(ExprInterpreter, defined_variables)
{
    std::vector<ScalarExprTest<bool>> tests = {
        {"defined(pi)", true},
        {"defined(e)", true},
        {"defined(pi,e)", true},
        {"defined(ted)", false},
    };
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_variables();
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_TRUE(result.is_bool());
        if (result.boolean())
        {
            std::stringstream str;
            result.format(str);
            ASSERT_EQ("1", str.str());
        }
        else
        {
            std::stringstream str;
            result.format(str);
            ASSERT_EQ("0", str.str());
        }
    }
    {
        std::stringstream input;
        input << "defined()";
        ExprInterpreter<> interpreter;
        Context           context;
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_TRUE(result.is_error());
        ASSERT_EQ("***Error : defined at line 1 and column 1 - reserved "
                  "function 'defined' requires an argument!\n",
                  result.string());
    }
}
TEST(ExprInterpreter, test_div_string)
{
    std::vector<ScalarExprTest<double>> tests = {
        {"'10.5'/'1'", 10.5},
        {"10.5/'1'", 10.5},
        {"'10.5'/1", 10.5},
        {"'10.5'/1.0", 10.5},
    };
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_TRUE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.expected, result.real());
    }
}

TEST(ExprInterpreter, right_operator_undefined_variables)
{
    std::vector<std::string> ops = {
        "< ", "> ", "<=", ">=", "==", "!=", "&&", "||", "+ ", "- ", "* ", "/ "};
    for (std::string op : ops)
    {
        SCOPED_TRACE(op);
        std::stringstream input;
        input << "pi+4 " << op << "y";
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_variables();
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_TRUE(result.is_error());
        ASSERT_EQ("***Error : value (y) at line 1 and column 8 - is not a "
                  "known variable.\n",
                  result.string());
    }
}
TEST(ExprInterpreter, left_operator_undefined_variables)
{
    std::vector<std::string> ops = {
        "< ", "> ", "<=", ">=", "==", "!=", "&&", "||", "+ ", "- ", "* ", "/ "};
    for (std::string op : ops)
    {
        SCOPED_TRACE(op);
        std::stringstream input;
        input << " y+4 " << op << "pi";
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_variables();
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_TRUE(result.is_error());
        ASSERT_EQ("***Error : value (y) at line 1 and column 2 - is not a "
                  "known variable.\n",
                  result.string());
    }
}
TEST(ExprInterpreter, vector_quoted_name)
{
    std::vector<int>                 data  = {1, 9, 8};
    std::vector<ScalarExprTest<int>> tests = {
        {"'my data'[0]", 1},
        {"'my data'[1]", 9},
        {"'my data'[2]", 8},
        {"'my data'['my data'[0]]", 9},
        {"'my data'[2]=7", 7},
        {"size('my data')", 3},
        {"if(size('my data')==3,1,0)", 1},
        {"if(size('my data')!=3,10,49)", 49},
        {"if(size('my data')==4,10,50)", 50},
        {"if(size('my data')==4,'my data'[0]=10,'my data'[0]+3)",
         4},  // if_true never evaluated
        {"if(3.gt.size('my data')-1,size('my data')-1,99)", 2},
        {"if(2.gt.size('my data')-1,size('my data')-1,99)", 99}};
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.store_ref("my data", data);
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_error());
        ASSERT_TRUE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.expected, result.integer());
    }
    std::vector<int> expected = {1, 9, 7};
    ASSERT_EQ(expected, data);
}
TEST(ExprInterpreter, vector_int_variables)
{
    std::vector<int>                 data  = {1, 9, 8};
    std::vector<ScalarExprTest<int>> tests = {
        {"data[0]", 1},
        {"data[1]", 9},
        {"data[2]", 8},
        {"data[data[0]]", 9},
        {"data[2]=7", 7},
        {"size(data)", 3},
        {"if(size(data)==3,1,0)", 1},
        {"if(size(data)!=3,10,49)", 49},
        {"if(size(data)==4,10,50)", 50},
        {"if(size(data)==4,data[0]=10,data[0]+3)",
         4},  // if_true never evaluated
        {"if(3.gt.size(data)-1,size(data)-1,99)", 2},
        {"if(2.gt.size(data)-1,size(data)-1,99)", 99}};
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.store_ref("data", data);
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_error());
        ASSERT_TRUE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.expected, result.integer());
    }
    std::vector<int> expected = {1, 9, 7};
    ASSERT_EQ(expected, data);
}
TEST(ExprInterpreter, vector_real_variables)
{
    std::vector<double>                 data  = {1.1, 9.2, 8.3};
    std::vector<ScalarExprTest<double>> tests = {
        {"data[0]", 1.1},     {"data[1]", 9.2},   {"data[2]", 8.3},
        {"data[2]=7.8", 7.8}, {"data[2]=7.0", 7}, {"data[0]=pi", 3.14159265359},
    };
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_variables();  // needed for pi
        context.store_ref("data", data);
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_TRUE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.expected, result.real());
    }
    std::vector<double> expected = {3.14159265359, 9.2, 7.0};
    ASSERT_EQ(expected, data);

    {  // test the unknown variable reference
        std::stringstream input;
        input << "data[0]=unknown";
        ExprInterpreter<> interpreter;
        Context           context;
        context.store_ref("data", data);
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_TRUE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ("***Error : value (unknown) at line 1 and column 9 - is not "
                  "a known variable.\n",
                  result.string());
    }
    {  // test the unknown variable reference
        std::stringstream input;
        input << "data[unknown]=3";
        ExprInterpreter<> interpreter;
        Context           context;
        context.store_ref("data", data);
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_TRUE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ("***Error : value (unknown) at line 1 and column 6 - is not "
                  "a known variable.\n",
                  result.string());
    }
}
TEST(ExprInterpreter, default_variables)
{
    {
        std::vector<ScalarExprTest<double>> tests = {{"pi", 3.14159265359},
                                                     {"e", 2.718281828459}};
        ASSERT_FALSE(tests.empty());
        for (auto& t : tests)
        {
            SCOPED_TRACE(t.tst);
            std::stringstream input;
            input << t.tst;
            ExprInterpreter<> interpreter;
            Context           context;
            context.add_default_variables();
            ASSERT_TRUE(interpreter.parse(input));

            auto result = interpreter.evaluate(context);
            ASSERT_FALSE(result.is_integer());
            ASSERT_TRUE(result.is_number());
            ASSERT_TRUE(result.is_real());
            ASSERT_FALSE(result.is_string());
            ASSERT_FALSE(result.is_bool());
            ASSERT_NEAR(t.expected, result.real(), 1e-8);
        }
    }
    {
        std::vector<ScalarExprTest<std::string>> tests = {{"nl", "\n"}};
        ASSERT_FALSE(tests.empty());
        for (auto& t : tests)
        {
            SCOPED_TRACE(t.tst);
            std::stringstream input;
            input << t.tst;
            ExprInterpreter<> interpreter;
            Context           context;
            context.add_default_variables();
            ASSERT_TRUE(interpreter.parse(input));

            auto result = interpreter.evaluate(context);
            ASSERT_FALSE(result.is_integer());
            ASSERT_FALSE(result.is_number());
            ASSERT_FALSE(result.is_real());
            ASSERT_TRUE(result.is_string());
            ASSERT_FALSE(result.is_bool());
            ASSERT_EQ(t.expected, result.string());
        }
    }
}
TEST(ExprInterpreter, default_real_functions)
{
    std::vector<ScalarExprTest<double>> tests = {
        {"sin(pi)", 0.0},
        {"sin(pi/2.0)", 1.0},
        {"sin(1.0)", 0.8414709848},
        {"sin(1)", 0.8414709848},
        {"cos(pi)", -1},
        {"cos(1)", 0.54030230586},
        {"cos(0.0)", 1},
        {"cos(pi/2)", 0.0},
        {"tan(0)", 0.0},
        {"tan(1)", 1.55740772465},
        {"tan(pi/6)", 0.57735026919},
        {"tan(-pi/3)", -1.73205080757},
        {"asin(1)", 1.57079633},
        {"acos(1)", 0.0},
        {"atan(1)", 0.785398163},
        {"floor(1.0)", 1.0},
        {"floor(1.1)", 1.0},
        {"floor(-1.0)", -1.0},
        {"floor(-1.1)", -2.0},
        {"ceil(1.0)", 1.0},
        {"ceil(1.1)", 2.0},
        {"ceil(-1.0)", -1.0},
        {"ceil(-1.1)", -1.0},
        {"exp(0.0)", 1.0},
        {"exp(1.0)", 2.71828182845904523},
        {"log(2.7182818284590451)", 1.0},
        {"ln(2.7182818284590451)", 1.0},
        {"log10(10.0)", 1.0},
        {"lg(2)", 1.0},
        {"lg(8)", 3.0},
        {"lg(9)", 3.16992500144},
        {"sqrt(4)", 2.0},
        {"sqrt(2)", 1.41421356},
        {"sec  (1) + csc  (1) + tanh (1) + cot  (1)", 4.44289759535},
        {"sec  (1) + cosec  (1) + tanh (1) + cot  (1)", 4.44289759535},
        {"sin  (1) + sinh (1) + sqrt (1) + tan  (1)", 4.57407990311},
        {"atan2(1,0)", 3.14159265359 / 2.0},
        {"pow(2,3)", 8.0},
        {"asinh(0)", 0.0},
        {"atanh(0)", 0.0},
        {"acosh(1)", 0.0},
        {"acosh(4)", 2.0634370688955},
        {"atanh(.8)", 1.0986122886},
        {"atanh(-.8)", -1.0986122886},
        {"asinh(1)", 0.881373587019},
        {"acosh(pi)", 1.81152627246},
        {"asinh(-1)", -0.881373587019},
        {"sin(deg2rad(30))", 0.5},
        {"rad2deg(0.5)", 28.647889756541161},
        {"grad2deg(1)", 1 * 10.0 / 9},
        {"deg2grad(1)", 1 * 9.0 / 10},
        {"round(1.23)", 1.0},
        {"round(1.5)", 2.0},
        {"round(1.49999)", 1.0},
        {"round(1.9)", 2.0},
        {"round(-1.9)", -2.0},
        {"round(-1.4999)", -1.0},
        {"round(-0.4999)", -0.0},
        {"abs(-0.4999)", 0.4999},
        {"abs(-12)", 12},
        {"abs(12)", 12},
        {"roundn(pi,2)", 3.14},
        {"roundn(pi,1)", 3.1},
        {"roundn(pi,3)", 3.142},
        {"roundn(pi,4)", 3.1416},
        {"min(1.1, 2)", 1.1},
        {"min(-1.1, -2)", -2},
        {"min(-1.1, -1.1)", -1.1},
        {"max(1.1, 2)", 2},
        {"max(-1.1, -2)", -1.1},
        {"max(1.1, 1.1)", 1.1},
    };
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_functions();
        context.add_default_variables();
        ASSERT_TRUE(interpreter.parse(input));
        auto result = interpreter.evaluate(context);
        if (result.is_error())
            std::cout << result.string() << std::endl;
        ASSERT_FALSE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_TRUE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_NEAR(t.expected, result.real(), 1e-8);
    }
}
TEST(ExprInterpreter, default_integer_functions)
{
    std::vector<ScalarExprTest<int>> tests = {
        {"mod(12,3)", 0}, {"mod(12,5)", 2},
    };
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_functions();
        context.add_default_variables();
        ASSERT_TRUE(interpreter.parse(input));
        auto result = interpreter.evaluate(context);
        if (result.is_error())
            std::cout << result.string() << std::endl;
        ASSERT_FALSE(result.is_error());
        ASSERT_TRUE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.expected, result.integer());
    }
}
TEST(ExprInterpreter, scalar_boolean)
{
    std::vector<ScalarExprTest<bool>> tests = {

        // same negative numbers
        {"-1000000 <  -1000000", false},
        {"-1000000 <= -1000000", true},
        {"-1000000 >  -1000000", false},
        {"-1000000 >= -1000000", true},
        {"-1000000 == -1000000", true},
        {"-1000000 != -1000000", false},
        {"-1000000 || -1000000", true},
        {"-1000000 && -1000000", true}

        // same zero integer
        ,
        {"0 <  0", false},
        {"0 <= 0", true},
        {"0 >  0", false},
        {"0 >= 0", true},
        {"0 == 0", true},
        {"0 != 0", false},
        {"0 || 0", false},
        {"0 && 0", false}

        // same positive integer
        ,
        {"1000000 <  1000000", false},
        {"1000000 <= 1000000", true},
        {"1000000 >  1000000", false},
        {"1000000 >= 1000000", true},
        {"1000000 == 1000000", true},
        {"1000000 != 1000000", false}

        // positive different integers
        ,
        {"2 >  3", false},
        {"2 >= 3", false},
        {"2 <  3", true},
        {"2 <= 3", true},
        {"2 == 3", false},
        {"2 != 3", true},
        {"2 || 3", true},
        {"2 && 3", true}

        // negative different integers
        ,
        {"-2 >  -3", true},
        {"-2 >= -3", true},
        {"-2 <  -3", false},
        {"-2 <= -3", false},
        {"-2 == -3", false},
        {"-2 != -3", true},
        {"-2 || -3", true},
        {"-2 && -3", true}

        // mixed sign different integer
        ,
        {"2 >  -3", true},
        {"2 >= -3", true},
        {"2 <  -3", false},
        {"2 <= -3", false},
        {"2 == -3", false},
        {"2 != -3", true},
        {"2 || -3", true},
        {"2 && -3", true}

        // same zero real number
        ,
        {"0.0 <  0.0", false},
        {"0.0 <= 0.0", true},
        {"0.0 >  0.0", false},
        {"0.0 >= 0.0", true},
        {"0.0 == 0.0", true},
        {"0.0 != 0.0", false},
        {"0.0 || 0.0", false},
        {"0.0 && 0.0", false}

        // positive same reals
        ,
        {"3.33 <  3.33", false},
        {"3.33 <= 3.33", true},
        {"3.33 >  3.33", false},
        {"3.33 >= 3.33", true},
        {"3.33 == 3.33", true},
        {"3.33 != 3.33", false},
        {"3.33 || 3.33", true},
        {"3.33 && 3.33", true}

        // positive different reals
        ,
        {"0.33 <  3.33", true},
        {"0.33 <= 3.33", true},
        {"0.33 >  3.33", false},
        {"0.33 >= 3.33", false},
        {"0.33 == 3.33", false},
        {"0.33 != 3.33", true},
        {"0.33 || 3.33", true},
        {"0.33 && 3.33", true}

        // negative same reals
        ,
        {"-3.33 <  -3.33", false},
        {"-3.33 <= -3.33", true},
        {"-3.33 >  -3.33", false},
        {"-3.33 >= -3.33", true},
        {"-3.33 == -3.33", true},
        {"-3.33 != -3.33", false},
        {"-3.33 || -3.33", true},
        {"-3.33 && -3.33", true}

        // negative different reals
        ,
        {"-0.33 <  -3.33", false},
        {"-0.33 <= -3.33", false},
        {"-0.33 >  -3.33", true},
        {"-0.33 >= -3.33", true},
        {"-0.33 == -3.33", false},
        {"-0.33 != -3.33", true},
        {"-0.33 || -3.33", true},
        {"-0.33 && -3.33", true}

        // boolean algebra tables
        ,
        {"1==1 && 2==2", true}  // true && true
        ,
        {"1==1 && 2==1", false}  // true && false
        ,
        {"1==2 && 0==2", false}  // false && false
        ,
        {"1==2 && 2==2", false}  // false and true

        // cont'd
        ,
        {"1==1 || 2==2", true},
        {"1==1 || 2==1", true},
        {"1==2 || 0==2", false},
        {"1==2 || 2==2", true}

    };

    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        ASSERT_EQ(true, interpreter.parse(input));

        auto result = interpreter.evaluate();
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_TRUE(result.is_bool());
        ASSERT_EQ(t.expected, result.boolean());
    }
}
TEST(ExprInterpreter, integer_variable_boolean)
{
    std::vector<VariableExprTest<int, int, bool>> tests = {

        // same negative numbers
        {"x < y", -1000000, -1000000, false},
        {"x <= y", -1000000, -1000000, true},
        {"x >  y", -1000000, -1000000, false},
        {"x >= y", -1000000, -1000000, true},
        {"x == y", -1000000, -1000000, true},
        {"x != y", -1000000, -1000000, false},
        {"x || y", -1000000, -1000000, true},
        {"x && y", -1000000, -1000000, true}

        // same zero integer
        ,
        {"x <  y", 0, 0, false},
        {"x <= y", 0, 0, true},
        {"x >  y", 0, 0, false},
        {"x >= y", 0, 0, true},
        {"x == y", 0, 0, true},
        {"x != y", 0, 0, false},
        {"x || y", 0, 0, false},
        {"x && y", 0, 0, false}

        // same positive integer
        ,
        {"x <  y", 1000000, 1000000, false},
        {"x <= y", 1000000, 1000000, true},
        {"x >  y", 1000000, 1000000, false},
        {"x >= y", 1000000, 1000000, true},
        {"x == y", 1000000, 1000000, true},
        {"x != y", 1000000, 1000000, false}

        // positive different integers
        ,
        {"x >  y", 2, 3, false},
        {"x >= y", 2, 3, false},
        {"x <  y", 2, 3, true},
        {"x <= y", 2, 3, true},
        {"x == y", 2, 3, false},
        {"x != y", 2, 3, true},
        {"x || y", 2, 3, true},
        {"x && y", 2, 3, true}

        // negative different integers
        ,
        {"x >  y", -2, -3, true},
        {"x >= y", -2, -3, true},
        {"x <  y", -2, -3, false},
        {"x <= y", -2, -3, false},
        {"x == y", -2, -3, false},
        {"x != y", -2, -3, true},
        {"x || y", -2, -3, true},
        {"x && y", -2, -3, true}

        // mixed sign different integer
        ,
        {"x >  y", 2, -3, true},
        {"x >= y", 2, -3, true},
        {"x <  y", 2, -3, false},
        {"x <= y", 2, -3, false},
        {"x == y", 2, -3, false},
        {"x != y", 2, -3, true},
        {"x || y", 2, -3, true},
        {"x && y", 2, -3, true}

        // boolean algebra tables
        ,
        {"x==x && y==y", 1, 2, true}  // true && true
        ,
        {"x==x && y==x", 1, 2, false}  // true && false
        ,
        {"x==y && 0==y", 1, 2, false}  // false && false
        ,
        {"x==y && y==y", 1, 2, false}  // false and true

        // cont'd
        ,
        {"x==x || y==y", 1, 2, true},
        {"x==x || y==x", 1, 2, true},
        {"x==y || 0==y", 1, 2, false},
        {"x==y || y==y", 1, 2, true}};

    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.store("x", t.v1);
        context.store("y", t.v2);
        ASSERT_EQ(true, interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_TRUE(result.is_bool());
        ASSERT_EQ(t.expected, result.boolean());
    }
}

TEST(ExprInterpreter, real_variable_boolean)
{
    std::vector<VariableExprTest<double, double, bool>> tests = {
        // same zero real number
        {"x <  y", 0.0, 0.0, false},
        {"x <= y", 0.0, 0.0, true},
        {"x >  y", 0.0, 0.0, false},
        {"x >= y", 0.0, 0.0, true},
        {"x == y", 0.0, 0.0, true},
        {"x != y", 0.0, 0.0, false},
        {"x || y", 0.0, 0.0, false},
        {"x && y", 0.0, 0.0, false}

        // positive same reals
        ,
        {"x <  y", 3.33, 3.33, false},
        {"x <= y", 3.33, 3.33, true},
        {"x >  y", 3.33, 3.33, false},
        {"x >= y", 3.33, 3.33, true},
        {"x == y", 3.33, 3.33, true},
        {"x != y", 3.33, 3.33, false},
        {"x || y", 3.33, 3.33, true},
        {"x && y", 3.33, 3.33, true}

        // positive different reals
        ,
        {"x <  y", 0.33, 3.33, true},
        {"x <= y", 0.33, 3.33, true},
        {"x >  y", 0.33, 3.33, false},
        {"x >= y", 0.33, 3.33, false},
        {"x == y", 0.33, 3.33, false},
        {"x != y", 0.33, 3.33, true},
        {"x || y", 0.33, 3.33, true},
        {"x && y", 0.33, 3.33, true}

        // negative same reals
        ,
        {"x <  y", -3.33, -3.33, false},
        {"x <= y", -3.33, -3.33, true},
        {"x >  y", -3.33, -3.33, false},
        {"x >= y", -3.33, -3.33, true},
        {"x == y", -3.33, -3.33, true},
        {"x != y", -3.33, -3.33, false},
        {"x || y", -3.33, -3.33, true},
        {"x && y", -3.33, -3.33, true}

        // negative different reals
        ,
        {"x <  y", -0.33, -3.33, false},
        {"x <= y", -0.33, -3.33, false},
        {"x >  y", -0.33, -3.33, true},
        {"x >= y", -0.33, -3.33, true},
        {"x == y", -0.33, -3.33, false},
        {"x != y", -0.33, -3.33, true},
        {"x || y", -0.33, -3.33, true},
        {"x && y", -0.33, -3.33, true}};
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.store("x", t.v1);
        context.store("y", t.v2);
        ASSERT_EQ(true, interpreter.parse(input));

        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_TRUE(result.is_bool());
        ASSERT_EQ(t.expected, result.boolean());
    }
}
TEST(ExprInterpreter, mixed_numeric_variable_boolean)
{
    std::vector<VariableExprTest<int, double, bool>> tests = {
        // same zero number
        {"x <  y", 0, 0.0, false},
        {"x <= y", 0, 0.0, true},
        {"x >  y", 0, 0.0, false},
        {"x >= y", 0, 0.0, true},
        {"x == y", 0, 0.0, true},
        {"x != y", 0, 0.0, false},
        {"x || y", 0, 0.0, false},
        {"x  && y", 0, 0.0, false}

        // positive same number
        ,
        {"x <  y", 3, 3.00, false},
        {"x <= y", 3, 3.00, true},
        {"x >  y", 3, 3.00, false},
        {"x >= y", 3, 3.00, true},
        {"x == y", 3, 3.00, true},
        {"x != y", 3, 3.00, false},
        {"x || y", 3, 3.00, true},
        {"x &&  y", 3, 3.00, true}

        // positive different numbers
        ,
        {"x <  y", 0, 3.33, true},
        {"x <= y", 0, 3.33, true},
        {"x >  y", 0, 3.33, false},
        {"x >= y", 0, 3.33, false},
        {"x == y", 0, 3.33, false},
        {"x != y", 0, 3.33, true},
        {"x || y", 0, 3.33, true},
        {"x  &&  y", 0, 3.33, false}

        // negative same number
        ,
        {"x <  y", -3, -3.00, false},
        {"x <= y", -3, -3.00, true},
        {"x >  y", -3, -3.00, false},
        {"x >= y", -3, -3.00, true},
        {"x == y", -3, -3.00, true},
        {"x != y", -3, -3.00, false},
        {"x || y", -3, -3.00, true},
        {"x&&y", -3, -3.00, true}

        // negative different numbers
        ,
        {"x <  y", -0, -3.33, false},
        {"x <= y", -0, -3.33, false},
        {"x >  y", -0, -3.33, true},
        {"x >= y", -0, -3.33, true},
        {"x == y", -0, -3.33, false},
        {"x != y", -0, -3.33, true},
        {"x || y", -0, -3.33, true},
        {"x && y", -0, -3.33, false}

        // expressions
        ,
        {"x/2.0 == y", 1, 0.5, true}};
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.store("x", t.v1);
        context.store("y", t.v2);
        ASSERT_EQ(true, interpreter.parse(input));

        auto result = interpreter.evaluate(context);

        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_TRUE(result.is_bool());
        ASSERT_EQ(t.expected, result.boolean());
    }
}
TEST(ExprInterpreter, scalar_integer)
{
    for (int i = -2500, count = 0; count < 5000; ++count, i = -2500 + count)
    {
        SCOPED_TRACE(i);
        std::stringstream input;
        input << "result=" << i
              << "^2";  // exponentiation is higher precendence than unary minus
        ExprInterpreter<> interpreter;
        ASSERT_EQ(true, interpreter.parse(input));
        Context context;
        auto    result = interpreter.evaluate(context);
        ASSERT_TRUE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        int expected = i > 0 ? std::pow(i, 2) : -std::pow(i, 2);
        ASSERT_EQ(expected, result.integer());
        ASSERT_EQ(expected, context.integer("result"));
    }
}

TEST(ExprInterpreter, DISABLED_scalar_real)
{
    for (double i = -50.1, count = 0; count < 100; ++count, i = -50.1 + count)
    {
        SCOPED_TRACE(i);
        std::stringstream input;
        input << "result=(" << i << "-15)*3";  // exponentiation is higher
                                               // precendence than unary minus
        ExprInterpreter<> interpreter;
        std::cout << input.str() << std::endl;
        ASSERT_EQ(true, interpreter.parse(input));
        Context context;
        auto    result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_integer());
        ASSERT_TRUE(result.is_number());
        ASSERT_TRUE(result.is_real());
        ASSERT_FALSE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        float expected = (i - 15.0) * 3;
        ASSERT_NEAR(expected, result.real(), 1e-10);
        ASSERT_NEAR(expected, context.real("result"), 1e-10);
    }
}

TEST(ExprInterpreter, DISABLED_string_concat)
{
    std::vector<ScalarExprTest<std::string>> tests = {
        {"'x'+'y'", "xy"},
        {"\"x\"+\"y\"", "xy"},
        {"'x'+'y'+' z '", "xy z "},
        {"'x'+'y'+(' z ')", "xy z "},
        {"3+'y'+' z '", "3y z "},
        {"'y'+3+' z '", "y3 z "},
        {"3/'y'+' z '", "3/y z "}
        // <-- y gets evaluated as zero, evaluates as 3/0 or inf.
        ,
        {"'y'+3/' z '", "y3/ z "},
        {"3 - 'y' +' z '", "3-y z "},
        {"'y' + 3 - ' z '", "y3- z "}};
    for (auto t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        ASSERT_EQ(true, interpreter.parse(input));

        auto result = interpreter.evaluate();
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_TRUE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.expected, result.string());
    }
}

struct ScalarExprFmtTest
{
    std::string tst;
    std::string expected;
    std::string fmt;
    ScalarExprFmtTest(const std::string& tst,
                      const std::string& expected,
                      const std::string& fmt)
        : tst(tst), expected(expected), fmt(fmt)
    {
    }
};
TEST(ExprInterpreter, format)
{
    std::vector<ScalarExprFmtTest> tests = {
        {"pi", "3.14159265359", "%6.11f"},
        {"pi", "3.14159", "%6.5f"},
        {"e", "2.718281828459", "%6.12f"},
        {"e", "result=2.718281828459", "result=%6.12f"},
        {"1.0", "result=1", "result=%.0f"},
        {"1", "result=1", "result=%d"},
        {"'fred'", "result=    fred", "result=%8s"},
        {"'fred'", "result=fred    ", "result=%-8s"},
    };
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.add_default_variables();
        ASSERT_TRUE(interpreter.parse(input));

        auto result = interpreter.evaluate(context);

        ASSERT_FALSE(result.is_error());
        std::stringstream fmt_result;
        std::stringstream fmt_error;
        ASSERT_TRUE(result.format(fmt_result, t.fmt, fmt_error));
        ASSERT_EQ(t.expected, fmt_result.str());
    }
}

TEST(ExprInterpreter, format_statements)
{
    std::vector<VariableExprTest<double, std::string, bool>> tests = {
        {"fmt(x,     '%3.2f')", 3.14159, "3.14", true},
        {"fmt('x',   '%3.2f')", 3.14159, "3.14", true},
        {"fmt(x,     '%5.4f')", 1.123456789, "1.1235", true},
        {"fmt(pi,    '%6.11f')", 0, "3.14159265359", true},
        {"fmt(pi,    '%6.5f')", 0, "3.14159", true},
        {"fmt(e,     '%6.12f')", 0, "2.718281828459", true},
        {"fmt(e,     'result=%6.12f')", 0, "result=2.718281828459", true},
        {"fmt(1.0,   'result=%.0f')", 0, "result=1", true},
        {"fmt(1,     'result=%d')", 0, "result=1", true},
        {"fmt('fred','result=%8s')", 0, "result=    fred", true},
        {"fmt('fred','result=%-8s')", 0, "result=fred    ", true}};
    ASSERT_FALSE(tests.empty());
    for (auto& t : tests)
    {
        SCOPED_TRACE(t.tst);
        std::stringstream input;
        input << t.tst;
        ExprInterpreter<> interpreter;
        Context           context;
        context.store("x", t.v1);
        context.add_default_variables();
        context.add_default_functions();
        ASSERT_TRUE(interpreter.parse(input));
        auto result = interpreter.evaluate(context);
        ASSERT_FALSE(result.is_error());
        ASSERT_FALSE(result.is_integer());
        ASSERT_FALSE(result.is_number());
        ASSERT_FALSE(result.is_real());
        ASSERT_TRUE(result.is_string());
        ASSERT_FALSE(result.is_bool());
        ASSERT_EQ(t.v2, result.string());
    }
}
