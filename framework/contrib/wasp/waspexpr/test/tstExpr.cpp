#include "waspexpr/ExprInterpreter.h"
#include "waspexpr/ExprContext.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
using namespace wasp;

// TODO elaborate this test suite

TEST(ExprInterpreter, addition)
{
    std::stringstream input;
    input << "8+9" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::PLUS, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::PLUS, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(17, result.integer());
}
TEST(ExprInterpreter, substraction)
{
    std::stringstream input;
    input << "8 - 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::MINUS, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::MINUS, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(5, result.integer());
}
TEST(ExprInterpreter, and_expr)
{
    std::stringstream input;
    input << "1==1 && 2==2" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::WASP_AND, op.type());

    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, or_expr)
{
    std::stringstream input;
    input << "1==1 || 1==2" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::WASP_OR, op.type());
    Context context;
    auto    result = interpreter.evaluate(context);
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, assign)
{
    std::stringstream input;
    input << "x=2" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::KEYED_VALUE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::DECL, op.child_at(0).type());
    ASSERT_EQ(wasp::ASSIGN, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    Context context;
    ASSERT_FALSE(context.exists("x"));
    auto result = interpreter.evaluate(context);
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(2, result.integer());
    ASSERT_TRUE(context.exists("x"));
    ASSERT_FALSE(context.exists("y"));
    bool ok = false;
    ASSERT_EQ(2, context.integer("x", &ok));
    ASSERT_TRUE(ok);
    ASSERT_EQ(2, context.real("x", &ok));
    ASSERT_TRUE(ok);
    ASSERT_TRUE(context.boolean("x", &ok));
    ASSERT_TRUE(ok);
}
TEST(ExprInterpreter, assign_existing)
{
    std::stringstream input;
    input << "x=2+5" << std::endl;
    ExprInterpreter<> interpreter;
    int               x = 3;
    Context           context;
    context.store_ref("x", x);
    ASSERT_TRUE(context.exists("x"));
    ASSERT_EQ(3, context.integer("x"));
    ASSERT_EQ(true, interpreter.parse(input));
    auto result = interpreter.evaluate(context);
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(7, result.integer());
    ASSERT_EQ(7, x);  // ensure x was updated
    ASSERT_TRUE(context.exists("x"));
    bool ok = false;
    ASSERT_EQ(7, context.integer("x", &ok));
    ASSERT_TRUE(ok);
    ASSERT_EQ(7, context.real("x", &ok));
    ASSERT_TRUE(ok);
    ASSERT_TRUE(context.boolean("x", &ok));
    ASSERT_TRUE(ok);
}
TEST(ExprInterpreter, exponent)
{
    std::stringstream input;
    input << "3^2" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::EXPONENT, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::EXPONENT, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(9, result.integer());
}
TEST(ExprInterpreter, mult)
{
    std::stringstream input;
    input << "8 * 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::MULTIPLY, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::MULTIPLY, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(24, result.integer());
}
TEST(ExprInterpreter, div)
{
    std::stringstream input;
    input << "8 / 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::DIVIDE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::DIVIDE, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    // integer is not closed under division
    // reflect this reality in the result
    ASSERT_FALSE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_TRUE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_NEAR(2.6666667, result.real(), 1e-7);
}
TEST(ExprInterpreter, equal)
{
    std::stringstream input;
    input << "8 == 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::EQ, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::EQ, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, equal_alternate)
{
    std::stringstream input;
    input << "8 .eq. 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::EQ, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::EQ, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, not_equal)
{
    std::stringstream input;
    input << "8 != 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::NEQ, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::NEQ, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, not_equal_alternate)
{
    std::stringstream input;
    input << "8 .neq. 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::NEQ, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::NEQ, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, less_than)
{
    std::stringstream input;
    input << "8 < 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::LT, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::LT, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, less_than_alternate)
{
    std::stringstream input;
    input << "8 .lt. 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::LT, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::LT, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, less_than_eq)
{
    std::stringstream input;
    input << "8 <= 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::LTE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::LTE, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, less_than_eq_alternate)
{
    std::stringstream input;
    input << "8 .lte. 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::LTE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::LTE, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, greater_than)
{
    std::stringstream input;
    input << "8 > 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::GT, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::GT, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, greater_than_alternate)
{
    std::stringstream input;
    input << "8 .gt. 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::GT, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::GT, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, greater_than_eq)
{
    std::stringstream input;
    input << "8 >= 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::GTE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::GTE, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, greater_than_eq_alternate)
{
    std::stringstream input;
    input << "8 .gte. 3" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::GTE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::VALUE, op.child_at(0).type());
    ASSERT_EQ(wasp::GTE, op.child_at(1).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, parenthesis)
{
    std::stringstream input;
    input << " (8 > 3) " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(8, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::PARENTHESIS, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::LPAREN, op.child_at(0).type());
    ASSERT_EQ(wasp::GT, op.child_at(1).type());
    ASSERT_EQ(wasp::RPAREN, op.child_at(2).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_TRUE(result.boolean());
}
TEST(ExprInterpreter, unary_not)
{
    std::stringstream input;
    input << " !(8 > 3) " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(10, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::UNARY_NOT, op.type());
    ASSERT_EQ(2, op.child_count());
    ASSERT_EQ(wasp::BANG, op.child_at(0).type());
    ASSERT_EQ(wasp::PARENTHESIS, op.child_at(1).type());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_TRUE(result.is_bool());
    ASSERT_FALSE(result.boolean());
}
TEST(ExprInterpreter, unary_minus)
{
    std::stringstream input;
    input << " -(8 > 3) " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(10, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::UNARY_MINUS, op.type());
    ASSERT_EQ(2, op.child_count());
    ASSERT_EQ(wasp::MINUS, op.child_at(0).type());
    ASSERT_EQ(wasp::PARENTHESIS, op.child_at(1).type());
}
TEST(ExprInterpreter, string)
{
    std::stringstream input;
    input << " 'my string' " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(2, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::VALUE, op.type());
    ASSERT_EQ(0, op.child_count());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_TRUE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ("my string", result.string());
}
TEST(ExprInterpreter, variable_ref)
{
    std::stringstream input;
    input << " x " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(2, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::VALUE, op.type());
    ASSERT_EQ(0, op.child_count());
    double  x = 3.14;
    Context context;
    context.store("x", x);
    auto result = interpreter.evaluate(context);
    ASSERT_FALSE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_TRUE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(x, result.real());
}
TEST(ExprInterpreter, assign_variable_ref)
{
    std::stringstream input;
    input << " y=x " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::KEYED_VALUE, op.type());
    ASSERT_EQ(3, op.child_count());
    double  x = 3.14;
    Context context;
    context.store("x", x);
    auto result = interpreter.evaluate(context);
    ASSERT_FALSE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_TRUE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(x, result.real());
    ASSERT_TRUE(context.exists("y"));
    ASSERT_EQ(x, context.real("y"));
}
TEST(ExprInterpreter, variable_ref_undefined)
{
    std::stringstream input;
    input << " x " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input, 10, 4));
    ASSERT_EQ(2, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::VALUE, op.type());
    ASSERT_EQ(0, op.child_count());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_FALSE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_TRUE(result.is_error());
    ASSERT_EQ("***Error : value (x) at line 10 and column 5 - is not a known "
              "variable.\n",
              result.string());
}
TEST(ExprInterpreter, pos_scalar)
{
    std::stringstream input;
    input << " 9 " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(2, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::VALUE, op.type());
    ASSERT_EQ(0, op.child_count());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(9, result.integer());
}
TEST(ExprInterpreter, neg_scalar)
{
    std::stringstream input;
    input << " -9 " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::UNARY_MINUS, op.type());
    ASSERT_EQ(2, op.child_count());
    ASSERT_EQ(wasp::MINUS, op.child_at(0).type());
    ASSERT_EQ(wasp::VALUE, op.child_at(1).type());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(-9, result.integer());
}
TEST(ExprInterpreter, basic_combined_integer)
{
    std::stringstream input;
    // exclude division because integer division is not closed
    input << "-(1+2-3*8-2^3)" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(22, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::UNARY_MINUS, op.type());
    ASSERT_EQ(2, op.child_count());
    auto result = interpreter.evaluate();
    ASSERT_TRUE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_FALSE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(29, result.integer());
}
TEST(ExprInterpreter, basic_combined_integer_division)
{
    std::stringstream input;
    // exclude division because integer division is not closed
    input << "-(1+2-3*8/2^3)" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(22, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::UNARY_MINUS, op.type());
    ASSERT_EQ(2, op.child_count());
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_TRUE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_EQ(0.0, result.real());
}
TEST(ExprInterpreter, basic_combined_real)
{
    std::stringstream input;
    input << "1+2/3.0*4/-(2.0^3)" << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    auto result = interpreter.evaluate();
    ASSERT_FALSE(result.is_integer());
    ASSERT_TRUE(result.is_number());
    ASSERT_TRUE(result.is_real());
    ASSERT_FALSE(result.is_string());
    ASSERT_FALSE(result.is_error());
    ASSERT_NEAR(2 / 3.0, result.real(), 1e-12);
}
TEST(ExprInterpreter, combined)
{
    std::stringstream input;
    input << " y = !(1+2 > -3*4/5^6)  " << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(27, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::KEYED_VALUE, op.type());
    ASSERT_EQ(3, op.child_count());
    ASSERT_EQ(wasp::DECL, op.child_at(0).type());
    ASSERT_EQ(wasp::ASSIGN, op.child_at(1).type());
    ASSERT_EQ(wasp::UNARY_NOT, op.child_at(2).type());
}
TEST(ExprInterpreter, expression)
{
    std::stringstream input;
    input << R"INPUT(
function = A*c^2*(1-c)^2+B*(c^2+6*(1-c)*(gr0^2+gr1^2+gr2^2+gr3^2)
             -4*(2-c)*(gr0^3+gr1^3+gr2^3+gr3^3)
             +3*(gr0^2+gr1^2+gr2^2+gr3^2)^2)
)INPUT";
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(146, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
}

TEST(ExprInterpreter, functions)
{
    std::vector<std::string> function_strings = {"sin(x)",
                                                 "cos(x)",
                                                 "tan(x)",
                                                 "asin(x)",
                                                 "acos(x)",
                                                 "atan(x)",
                                                 "atan2(y, x)",
                                                 "sec(x)",
                                                 "cosec(x)",
                                                 "cot(x)",
                                                 "sinh(x)",
                                                 "cosh(x)",
                                                 "tanh(x)",
                                                 "asinh(x)",
                                                 "acosh(x)",
                                                 "atanh(x)",
                                                 "ln(x) ",
                                                 "log(x)",
                                                 "lg(x)",
                                                 "exp(x)",
                                                 "pow(x)",
                                                 "avg(x1,x2)",
                                                 "avg(x1,x2,x3)",
                                                 "min(x1,x2)",
                                                 "min(x1,x2,x3)",
                                                 "max(x1,x2)",
                                                 "max(x1,x2,x3)",
                                                 "round(x)",
                                                 "round(x, p)",
                                                 "floor(x)",
                                                 "ceil(x)",
                                                 "if(cond, trueval, falseval)",
                                                 "str(x)",
                                                 "abs(x)",
                                                 "rand()",
                                                 "mod(x,y)",
                                                 "sqrt(x)",
                                                 "get(name,index)",
                                                 "get(name,child_name)",
                                                 "length(attribute)",
                                                 "left(string, count)",
                                                 "right(string, count)",
                                                 "find(string, substr)",
                                                 "replace(string,x,y)",
                                                 "exists(filepath)",
                                                 "current_date()",
                                                 "date('mm-dd-yyyy')",
                                                 "binom(n, i)"};
    for (const std::string& f : function_strings)
    {
        SCOPED_TRACE(f);
        std::stringstream input;
        input << f;
        ExprInterpreter<> interpreter;
        ASSERT_EQ(true, interpreter.parse(input));
        ASSERT_LT(1, interpreter.node_count());
        auto document = interpreter.root();
        ASSERT_EQ(1, document.child_count());
        ASSERT_EQ(wasp::FUNCTION, document.child_at(0).type());
    }
}

TEST(ExprInterpreter, variable_indexing)
{
    std::vector<std::string> variable_strings = {"array[i]", "obj['fred']",
                                                 "array[1]"};
    for (const std::string& f : variable_strings)
    {
        SCOPED_TRACE(f);
        std::stringstream input;
        input << f;
        ExprInterpreter<> interpreter;
        ASSERT_EQ(true, interpreter.parse(input));
        ASSERT_LT(1, interpreter.node_count());
        auto document = interpreter.root();
        ASSERT_EQ(1, document.child_count());
        ASSERT_EQ(wasp::OBJECT, document.child_at(0).type());
    }
}

TEST(ExprInterpreter, if_real_gt_size)
{
    std::stringstream input;
    input << "if(i+7.gt.size('something')-1,i+7,size('something')-1)"
          << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(33, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::FUNCTION, op.type());
    ASSERT_EQ(8, op.child_count());
    ASSERT_EQ(wasp::DECL, op.child_at(0).type());
    ASSERT_EQ(wasp::LPAREN, op.child_at(1).type());
    ASSERT_EQ(wasp::GT, op.child_at(2).type());
    ASSERT_EQ(wasp::WASP_COMMA, op.child_at(3).type());
    ASSERT_EQ(wasp::PLUS, op.child_at(4).type());
    ASSERT_EQ(wasp::WASP_COMMA, op.child_at(5).type());
    ASSERT_EQ(wasp::MINUS, op.child_at(6).type());
    ASSERT_EQ(wasp::RPAREN, op.child_at(7).type());
}

TEST(ExprInterpreter, if_real_eq_size)
{
    std::stringstream input;
    input << "if(i+7.eq.size('something')-1,i+7,size('something')-1)"
          << std::endl;
    ExprInterpreter<> interpreter;
    ASSERT_EQ(true, interpreter.parse(input));
    ASSERT_EQ(33, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto op = document.child_at(0);
    ASSERT_EQ(wasp::FUNCTION, op.type());
    ASSERT_EQ(8, op.child_count());
    ASSERT_EQ(wasp::DECL, op.child_at(0).type());
    ASSERT_EQ(wasp::LPAREN, op.child_at(1).type());
    ASSERT_EQ(wasp::EQ, op.child_at(2).type());
    ASSERT_EQ(wasp::WASP_COMMA, op.child_at(3).type());
    ASSERT_EQ(wasp::PLUS, op.child_at(4).type());
    ASSERT_EQ(wasp::WASP_COMMA, op.child_at(5).type());
    ASSERT_EQ(wasp::MINUS, op.child_at(6).type());
    ASSERT_EQ(wasp::RPAREN, op.child_at(7).type());
}
