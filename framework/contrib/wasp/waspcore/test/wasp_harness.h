#ifndef WASP_TEST_HARNESS_H
#define WASP_TEST_HARNESS_H
#include <stdexcept>
#include <sstream>

int PASS      = 0;
int FAIL      = 1;
int HARD_FAIL = -1;

// Macro to start a test file
#define WASP_TESTS \
    int main()     \
    {              \
        int WASP_TEST_GROUP_RESULT = 0;
// Macro to finish a test file
#define WASP_TESTS_END             \
    return WASP_TEST_GROUP_RESULT; \
    }

#define W_THROW_MSG(a, b)                                                  \
    {                                                                      \
        std::stringstream msg;                                             \
        msg << "Expected (" << #a << ") " << a << std::endl                \
            << "Actual (" << #b << ") " << b << std::endl;                 \
        msg << "Failed in " << __FILE__ << " : " << __LINE__ << std::endl; \
        throw std::domain_error(msg.str());                                \
    }

#define W_ASSERT_EQ(a, b)      \
    {                          \
        if (a != b)            \
            W_THROW_MSG(a, b); \
    }

#define TEST(CATEGORY, NAME) \
    try                      \
    {                        \
        std::cout << "Starting " << #CATEGORY << "." << #NAME << std::endl;

#define TEST_END(CATEGORY, NAME)                                             \
    std::cout << " " << #CATEGORY << "." << #NAME << " PASSED" << std::endl; \
    }                                                                        \
    catch (const std::domain_error& error)                                   \
    {                                                                        \
        std::cout << "!" << #CATEGORY << "." << #NAME << " FAILED"           \
                  << std::endl;                                              \
        std::cout << error.what() << std::endl;                              \
        WASP_TEST_GROUP_RESULT = -1;                                         \
    }
#define do_test(x, result)                                                 \
    {                                                                      \
        std::cout << "Testing " #x << std::endl;                           \
        int r = x();                                                       \
        std::cout << #x << " "                                             \
                  << (r == PASS ? "passed"                                 \
                                : (r == FAIL ? "FAILED" : "HARD FAILED!")) \
                  << std::endl;                                            \
        if (r == HARD_FAIL || r == FAIL)                                   \
            result = r;                                                    \
        if (result == HARD_FAIL)                                           \
        {                                                                  \
            std::cout << "STOPPING" << std::endl;                          \
            return result;                                                 \
        }                                                                  \
    }
#define FAILED(a, b)                           \
    std::cout << "Expected " << a << std::endl \
              << "Actual " << b << std::endl,  \
        FAIL
#define EXPECT_EQ(a, b) a != b ? (FAILED(a, b)) : PASS

#endif
