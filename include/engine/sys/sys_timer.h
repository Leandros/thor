/*!
 * \file sys_timer.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Timer
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include <sys/sys_dll.h>
#include <sys/sys_macros.h>
#include <sys/sys_types.h>

#ifndef _WINDOWS_
typedef union _LARGE_INTEGER LARGE_INTEGER;
int __stdcall QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);
int __stdcall QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);
#endif


/*!
 * \brief Get the number of milliseconds elapsed since boot.
 * \return An integer representing the milliseconds elapsed since boot.
 */
REAPI u64
timer_ticks(void);


/*!
 * \defgroup TIMER_PERF Performance timing
 * \brief Timing to measure how long a task takes
 *
 * @{
 */

/*!
 * \brief Converts cycle counters into microseconds.
 */
#define PERF_TO_MICROSECONDS 1000000

/*!
 * \brief Converts cycle counters into milliseconds.
 */
#define PERF_TO_MILLISECONDS 1000

/*!
 * \brief Get current high-resolution cycle count.
 * \return Non-Zero value on success, 0 on failure.
 * \remark It's an entirely processor dependend value, and should only be
 *         used to time sections of code.
 * \remark Usage:
 * \code
 *  u64 start, end, elapsed;
 *  start = perf_ticks();
 *  ... time intensive task here ...
 *  end = perf_ticks();
 *  elapsed  = end - start;
 *  elapsed *= PERF_TO_MICROSECONDS;
 *  elapsed /= perf_freq();
 * \endcode
 */
static u64
perf_ticks(void)
{
    u64 lint;
    QueryPerformanceCounter((LARGE_INTEGER *)&lint);
    return lint;
}


/*!
 * \brief Get CPU cycle counts per second.
 * \return Cycle count per second.
 * \remark This is a processor dependend value. Can be cached.
 */
static u64
perf_freq(void)
{
    u64 lint;
    QueryPerformanceFrequency((LARGE_INTEGER *)&lint);
    return lint;
}

/*! @} */

#endif /* SYS_TIMER_H */

