/*!
 * \file dbg_profiler.h
 * \author Arvid Gerstmann
 * \date October 2016
 * \brief Logging System.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef DBG_PROFILER_H
#define DBG_PROFILER_H


/* ========================================================================= */
/* Convenience usage                                                         */
/* ========================================================================= */

#define profile_function_start()    dbg_profiler_begin(__FUNCTION__, NULL)
#define profile_function_end()      dbg_profiler_end()

#define profile_start(name)         dbg_profiler_begin(__FUNCTION__, name)
#define profile_end()               dbg_profiler_end()

#define gpu_profile_start(name)     gfx_profiler_begin(#name)
#define gpu_profile_end()           gfx_profiler_end()


/* ========================================================================= */
/* Constructor / Destructor                                                  */
/* ========================================================================= */

/*!
 * \brief Initialize the profiler.
 * \return 0 on success, non-zero on failure.
 */
int
dbg_profiler_init(void);

/*!
 * \brief Initialize a thread to be used within the profiler.
 */
void
dbg_profiler_init_thread(void);

/*!
 * \brief Quit the profiler.
 */
void
dbg_profiler_quit(void);

/*!
 * \brief Enable / disable the profiler.
 */
void
dbg_profiler_enabled(int enabled);


/* ========================================================================= */
/* Functions                                                                 */
/* ========================================================================= */

/*!
 * \brief Kickoff a new frame.
 */
void
dbg_profiler_frame_start(void);

/*!
 * \brief End the current frame.
 */
void
dbg_profiler_frame_end(void);

/*!
 * \brief Start CPU profiling.
 * \param function Name of the function containing this sample.
 * \param name Name of sample.
 * \remark If \c name is NULL, calling it more than once in a function is UB.
 */
void
dbg_profiler_begin(char const *function, char const *name);

/*!
 * \brief Stop CPU profiling.
 */
void
dbg_profiler_end(void);


/*!
 * \brief Start GPU profiling.
 */
void
gfx_profiler_begin(char const *name);

/*!
 * \brief Stop GPU profiling.
 */
void
gfx_profiler_end(void);

#endif /* DBG_PROFILER_H */

