#include "gtest/gtest.h"

#include "waspcore/Format.h"
using namespace wasp;

template<class T>
struct format_test
{
    std::string format;
    T           arg;
    std::string expected;
    format_test(const std::string& f, T a, const std::string& e)
    {
        format   = f;
        arg      = a;
        expected = e;
    }
};

TEST(utils, format_fixed_precision)
{
    std::vector<format_test<double>> tests = {
        {"|%f|", 3.14159265, "|3.141593|"},
        {"|%7f|", 3.14159265, "|3.141593|"},
        {"|%8f|", 3.14159265, "|3.141593|"},
        {"|%9f|", 3.14159265, "| 3.141593|"},
        {"|%10f|", 3.14159265, "|  3.141593|"},
        {"|%.0f|", 3.14159265, "|3|"},
        {"|%.1f|", 3.14159265, "|3.1|"},
        {"|%.2f|", 3.14159265, "|3.14|"},
        {"|%.3f|", 3.14159265, "|3.142|"},
        {"|%1.0f|", 3.14159265, "|3|"},
        {"|%4.1f|", 3.14159265, "| 3.1|"},
        {"|%4.8f|", 3.14159265, "|3.14159265|"},
        {"|%8.2f|", 3.14159265, "|    3.14|"},
        {"|%10.3f|", 3.14159265, "|     3.142|"},
        {"|%01.0f|", 3.14159265, "|3|"},
        {"|%04.1f|", 3.14159265, "|03.1|"},
        {"|%04.8f|", 3.14159265, "|3.14159265|"},
        {"|%08.2f|", 3.14159265, "|00003.14|"},
        {"|%010.3f|", 3.14159265, "|000003.142|"}};
    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing fixed format of " << tst.format << std::endl;
        std::stringstream out, err;
        ASSERT_TRUE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, out.str());
    }
}

TEST(utils, format_general_scientific_precision)
{
    std::vector<format_test<double>> tests = {
        {"|%g|", 3.14159265, "|3.14159|"},
        {"|%.0g|", 3.14159265, "|3|"},
        {"|%.1g|", 3.14159265, "|3|"},
        {"|%.2g|", 3.14159265, "|3.1|"},
        {"|%.3g|", 3.14159265, "|3.14|"},
        {"|%1.0g|", 3.14159265, "|3|"},
        {"|%4.1g|", 3.14159265, "|   3|"},
        {"|%4.8g|", 3.14159265, "|3.1415927|"},
        {"|%8.2g|", 3.14159265, "|     3.1|"},
        {"|%10.3g|", 3.14159265, "|      3.14|"}
#ifndef _WIN32
        ,
        {"|%01.0g|", 3.14159265, "|3|"}
#endif
        ,
        {"|%04.1g|", 3.14159265, "|0003|"},
        {"|%04.8g|", 3.14159265, "|3.1415927|"},
        {"|%08.2g|", 3.14159265, "|000003.1|"},
        {"|%010.3g|", 3.14159265, "|0000003.14|"},
        {"|%.4g|", 1e-4, "|0.0001000|"},
        {"|%10.4g|", 1e-4, "| 0.0001000|"},
        {"|%.5g|", 1e-4, "|0.00010000|"},
        {"|%.5g|", 1e-5, "|1.0000e-05|"},
        {"|%.6g|", 1e-6, "|1.00000e-06|"},
        {"|%.7g|", 1e-2, "|0.01000000|"},
        {"|%.7g|", 1e-2, "|0.01000000|"},
        {"|%10.7g|", 1e-2, "|0.01000000|"},
        {"|%10.7g|", 1e2, "|  100.0000|"}

    };
    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing general scientific format of " << tst.format
                  << std::endl;
        std::stringstream out, err;
        ASSERT_TRUE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, out.str());
    }
}

TEST(utils, format_scientific_precision)
{
    std::vector<format_test<double>> tests = {
        {"|%e|", 3.14159265, "|3.141593e+00|"},
        {"|%.1e|", 3.14159265, "|3.1e+00|"},
        {"|%.2e|", 3.14159265, "|3.14e+00|"},
        {"|%.3e|", 3.14159265, "|3.142e+00|"},
        {"|%4.1e|", 3.14159265, "|3.1e+00|"},
        {"|%4.8e|", 3.14159265, "|3.14159265e+00|"},
        {"|%8.2e|", 3.14159265, "|3.14e+00|"},
        {"|%10.3e|", 3.14159265, "| 3.142e+00|"}
#ifndef _WIN32
        ,
        {"|%.0e|", 3.14159265, "|3e+00|"},
        {"|%1.0e|", 3.14159265, "|3e+00|"},
        {"|%01.0e|", 3.14159265, "|3e+00|"}
#endif
        ,
        {"|%04.1e|", 3.14159265, "|3.1e+00|"},
        {"|%04.8e|", 3.14159265, "|3.14159265e+00|"},
        {"|%08.2e|", 3.14159265, "|3.14e+00|"},
        {"|%010.3e|", 3.14159265, "|03.142e+00|"},
        {"|%.4e|", 1e-4, "|1.0000e-04|"},
        {"|%10.4e|", 1e-4, "|1.0000e-04|"},
        {"|%.5e|", 1e-4, "|1.00000e-04|"},
        {"|%.5e|", 1e-5, "|1.00000e-05|"},
        {"|%.6e|", 1e-6, "|1.000000e-06|"},
        {"|%.7e|", 1e-2, "|1.0000000e-02|"},
        {"|%.7e|", 1e-2, "|1.0000000e-02|"},
        {"|%10.7e|", 1e-2, "|1.0000000e-02|"},
        {"|%10.7e|", 1e2, "|1.0000000e+02|"}};

    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing scientific format of " << tst.format << std::endl;
        std::stringstream out, err;
        ASSERT_TRUE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, out.str());
    }
}

TEST(utils, format_double_as_integer)
{
    std::vector<format_test<double>> tests = {{"|%d|", 3.14159265, "|3|"},
                                              {"|%d|", 1e-4, "|0|"}};

    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing double as integer format of " << tst.format
                  << std::endl;
        std::stringstream out, err;
        ASSERT_TRUE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, out.str());
    }
}

TEST(utils, format_integer)
{
    std::vector<format_test<int>> tests = {{"|%d|", 3, "|3|"},
                                           {"|%1d|", 30, "|30|"},
                                           {"|%5d|", 30, "|   30|"},
                                           {"|%-5d|", 30, "|30   |"},
                                           {"|%(5d|", -30, "| (30)|"},
                                           {"|%+5d|", 30, "|  +30|"},
                                           {"|%+5d|", -30, "|  -30|"},
                                           {"|%05d|", -30, "|-0030|"},
                                           {"%%5d=|%05d|", -30, "%5d=|-0030|"},
                                           {"|% d|", 30, "| 30|"},
                                           {"|% d|", -30, "|-30|"},
                                           {"|%+d|", 30, "|+30|"},
                                           {"|% 3d|", 30, "| 30|"},
                                           {"|% 4d|", 30, "|  30|"},
                                           {"|% 10d|", 30, "|        30|"}};

    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing integer format of " << tst.format << std::endl;
        std::stringstream out, err;
        ASSERT_TRUE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, out.str());
    }
}
TEST(utils, format_string)
{
    std::vector<format_test<std::string>> tests = {
        {"|%s|", "3", "|3|"},
        {"|%1s|", "30", "|30|"},
        {"|%5s|", "30", "|   30|"},
        {"|%05s|", "-30", "|00-30|"},
        {"%%5s=|%05s|", "-30", "%5s=|00-30|"},
        {"|%10s|", "30", "|        30|"},
        {"|%010s|", "30", "|0000000030|"},
        {"|%-10s|", "30", "|30        |"}};

    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing string format of " << tst.format << std::endl;
        std::stringstream out, err;
        ASSERT_TRUE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, out.str());
    }
}

TEST(utils, format_string_error)
{
    std::vector<format_test<std::string>> tests = {
        {"|%(5s|", "-30",
         "malformed format string: flag '(' does not match the conversion 's'"},
        {"|%+5s|", "-30",
         "malformed format string: flag '+' does not match the conversion 's'"},
        {"|% s|", "30",
         "malformed format string: flag ' ' does not match the conversion 's'"},
        {"|% 3s|", "30",
         "malformed format string: flag ' ' does not match the conversion 's'"},
        {"|% 4s|", "30", "malformed format string: flag ' ' does not match the "
                         "conversion 's'"}};

    for (const auto& tst : tests)
    {
        SCOPED_TRACE(tst.format);
        std::cout << "Testing error string format of " << tst.format
                  << std::endl;
        std::stringstream out, err;
        ASSERT_FALSE(wasp::Format::fmt(out, err, tst.format.c_str(), tst.arg));
        EXPECT_EQ(tst.expected, err.str());
    }
}
