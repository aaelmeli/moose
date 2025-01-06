#include "gtest/gtest.h"

// force WASP_TIMING on
#define WASP_TIMING 3
#include "waspcore/wasp_bug.h"

TEST(waspcore, WaspTimer)
{
    wasp_line("Starting timer 1.");
    wasp_timer(timer1);
    wasp_line("Starting timer 2.");
    wasp_timer_2(timer2);
    wasp_line("Starting timer 3.");
    wasp_timer_3(timer3);
    double value = 0;
    auto func    = [](double value) -> double { return value * value; };
    //
    // demostrate timing metrics
    wasp_timer_start(timer1);
    for (size_t i = 0; i < 100; ++i)
    {
        wasp_timer_start_2(timer2);
        for (size_t j = 0; j < 10000; ++j)
        {
            wasp_timer_start_3(timer3);
            value = value + func(value);
            wasp_timer_stop_3(timer3);
        }
        wasp_timer_stop_2(timer2);
    }
    wasp_timer_stop(timer1);
    wasp_timer_block(std::cout << "First Timer duration: " << timer1.duration()
                               << " nanoseconds with " << timer1.intervals()
                               << " invervals" << std::endl);
    wasp_timer_block_2(std::cout
                       << "Second Timer duration: " << timer2.duration()
                       << " nanoseconds with " << timer2.intervals()
                       << " invervals" << std::endl);
    wasp_timer_block_3(std::cout
                       << "Third Timer duration: " << timer3.duration()
                       << " nanoseconds with " << timer3.intervals()
                       << " invervals" << std::endl);

    EXPECT_EQ(timer1.intervals(), 1);
    EXPECT_EQ(timer2.intervals(), 100);
    EXPECT_EQ(timer3.intervals(), (100 * 10000));
}

TEST(waspcore, not_implemented_test)
{
    ASSERT_THROW(wasp_not_implemented("test"), std::runtime_error);
}
