#ifndef WASP_WASPCORE_BUG_HH_
#define WASP_WASPCORE_BUG_HH_

#ifndef WASP_DEBUG_OUTPUT
#define WASP_DEBUG_OUTPUT 0
#define wasp_debug_call(c)
#else
#define wasp_debug_call(c) c
#endif

// default to c++ compiler
/*
 * @file:   bug.hh
 * @author: Jordan P. Lefebvre, lefebvrejp@ornl.gov
 * @brief Debug functionality for including debug output \
 * in your source without a performance penalty.
 * This is an include ONLY header. There is no library to link against
 * Each function is preprocessed to be included or not.
 * If WASP_DEBUG_OUTPUT is defined the MACROs are included.
 *
 * Available MACROs:
 * wasp(arg) - std::cerr << arg - Pushes content to stderr
 * wasp_line(arg) - std::cerr << arg << std::endl - Push newline terminated
 * content to stderr
 * wasp(arg) - std::cerr << arg - Pushes content to stderr
 * wasp_warning(arg) - std::cerr << arg << std::endl - Push newline terminated
 * content to stderr
 * wasp_tagged_line(arg) - Same as wasp_line, prefixed with FILE and LINE
 * wasp_tagged(arg) - Same as wasp, prefixed with FILE and LINE
 * wasp_tagged_warning(arg) - Same as wasp_warning, prefixed with FILE and Line
 * wasp_block(block) - block - Simply places code block in preprocessor
 */
#include <iostream>
#include <cstdio>
#if WASP_DEBUG_OUTPUT & 1
#define wasp_line(arg) std::cerr << arg << std::endl
#define wasp_flush_line(arg)       \
    std::cerr << arg << std::endl; \
    fflush(stderr)
#define wasp(arg) std::cerr << arg
#define wasp_warning(arg) std::cerr << arg << std::endl
#define wasp_tagged_warning(arg) \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl
#define wasp_flush_warning(arg)    \
    std::cerr << arg << std::endl; \
    fflush(stderr)
#define wasp_tagged_line(arg) \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl
#define wasp_flush_tagged_line(arg)                                       \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl; \
    fflush(stderr)
#define wasp_tagged(arg) std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg
#define wasp_flush_tagged_warning(arg)                                    \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl; \
    fflush(stderr)
#define wasp_tagged_block(block)                                  \
    std::cerr << __FILE__ << ":" << __LINE__ << ":" << std::endl; \
    block
#define wasp_block(block) block
#else
#define wasp(arg)
#define wasp_line(arg)
#define wasp_flush_line(arg)
#define wasp_warning(arg)
#define wasp_flush_warning(arg)
#define wasp_tagged_line(arg)
#define wasp_flush_tagged_line(arg)
#define wasp_tagged(arg)
#define wasp_tagged_warning(arg)
#define wasp_flush_tagged_warning(arg)
#define wasp_tagged_block(block)
#define wasp_block(block)
#endif /* WASP_DEBUG_OUTPUT */

#include <stdexcept>
#include <sstream>
#ifndef WASP_DBC
#define WASP_DBC 0
#endif

//
// Defined all levels of WASP_DBC
// WASP_DBC = 1 enables Require
// WASP_DBC = 2 enables Check
// WASP_DBC = 4 enables Remember & Ensure
// WASP_DBC = 7 enables all
// Insist is always enabled
#if WASP_DBC & 1
#define wasp_require(c)                                                 \
    if (!(c))                                                           \
    {                                                                   \
        std::ostringstream stream;                                      \
        stream << __FILE__ << ":" << __LINE__ << " wasp_require(" << #c \
               << ") failed." << std::endl;                             \
        throw std::runtime_error(stream.str());                         \
    }
#else
#define wasp_require(c)
#endif
#if WASP_DBC & 2
#define wasp_check(c)                                                 \
    if (!(c))                                                         \
    {                                                                 \
        std::ostringstream stream;                                    \
        stream << __FILE__ << ":" << __LINE__ << " wasp_check(" << #c \
               << ") failed." << std::endl;                           \
        throw std::runtime_error(stream.str());                       \
    }
#else
#define wasp_check(c)
#endif
#if WASP_DBC & 4
#define wasp_ensure(c)                                                 \
    if (!(c))                                                          \
    {                                                                  \
        std::ostringstream stream;                                     \
        stream << __FILE__ << ":" << __LINE__ << " wasp_ensure(" << #c \
               << ") failed." << std::endl;                            \
        throw std::runtime_error(stream.str());                        \
    }
#define wasp_remember(c) c
#else
#define wasp_ensure(c)
#define wasp_remember(c)
#endif

#define wasp_insist(c, msg)                                           \
    if (!(c))                                                         \
    {                                                                 \
        std::ostringstream stream;                                    \
        stream << __FILE__ << ":" << __LINE__ << "wasp_insist(" << #c \
               << ") failed with this message:" << std::endl          \
               << msg << std::endl;                                   \
        throw std::runtime_error(stream.str());                       \
    }

#define wasp_not_implemented(msg)                             \
    {                                                         \
        std::ostringstream stream;                            \
        stream << __FILE__ << ":" << __LINE__ << " : " << msg \
               << " is not implemented. " << std::endl;       \
        throw std::runtime_error(stream.str());               \
    }

/// set default timing to off
#ifndef WASP_TIMING
#define WASP_TIMING 0
#endif

/// provide for necessary includes if timing is on
#if WASP_TIMING
#include <chrono>
#include <ctime>
namespace wasp
{
class WaspTimer
{
  private:
    bool                                  mRunning;
    std::chrono::steady_clock::time_point mStart;
    std::chrono::steady_clock::time_point mEnd;
    std::chrono::nanoseconds              mDuration;
    size_t                                mIntervals;

  public:
    WaspTimer() : mRunning(false), mDuration(0), mIntervals(0) {}
    void start()
    {
        wasp_check(!mRunning);
        mRunning = true;
        mIntervals++;
        mStart = std::chrono::steady_clock::now();
    }
    void stop()
    {
        wasp_check(mRunning);
        mEnd     = std::chrono::steady_clock::now();
        mRunning = false;
        mDuration +=
            std::chrono::duration_cast<std::chrono::nanoseconds>(mEnd - mStart);
    }

    double duration() const
    {
        wasp_check(!mRunning);
        return mDuration.count();
    }
    size_t intervals() const { return mIntervals; }
};  // class Timer
}  // namespace wasp
#endif

#if WASP_TIMING > 0
#define wasp_timer(name) wasp::WaspTimer name
#define wasp_timer_start(name) name.start()
#define wasp_timer_stop(name) name.stop()
#define wasp_timer_block(content) content
#else
#define wasp_timer(name)
#define wasp_timer_start(name)
#define wasp_timer_stop(name)
#define wasp_timer_block(content)
#endif
#if WASP_TIMING > 1
#define wasp_timer_2(name) wasp::WaspTimer name
#define wasp_timer_start_2(name) name.start()
#define wasp_timer_stop_2(name) name.stop()
#define wasp_timer_block_2(content) content
#else
#define wasp_timer_2(name)
#define wasp_timer_start_2(name)
#define wasp_timer_stop_2(name)
#define wasp_timer_block_2(content)
#endif

#if WASP_TIMING > 2
#define wasp_timer_3(name) wasp::WaspTimer name
#define wasp_timer_start_3(name) name.start()
#define wasp_timer_stop_3(name) name.stop()
#define wasp_timer_block_3(content) content
#else
#define wasp_timer_3(name)
#define wasp_timer_start_3(name)
#define wasp_timer_stop_3(name)
#define wasp_timer_block_3(content)
#endif

#endif /* WASP_WASPCORE_BUG_HH_*/
