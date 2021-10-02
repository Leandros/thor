/*!
 * \file dbg_log.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Logging System.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef DBG_LOG_H
#define DBG_LOG_H

#include <config.h>
#include <sys/sys_dll.h>
#include <sys/sys_types.h>

/*!
 * \defgroup LogVerbosity Logging verbosity
 * \brief Levels of global logging verbosity.
 *
 * To set the current log verbosity, set desired verbosity to
 * #dbg_log_verbosity
 *
 * \see dbg_log_verbosity
 * @{
 */
#define LOG_VERBOSITY_VERBOSE   0
#define LOG_VERBOSITY_DEBUG     1
#define LOG_VERBOSITY_INFO      2
#define LOG_VERBOSITY_WARNING   3
#define LOG_VERBOSITY_ERROR     4
#define LOG_VERBOSITY_OFF       5
/*! @} */

#if USING(ENGINE_DEBUG)
#   define VLOG(args) dbg_log_verbose args
#   define DLOG(args) dbg_log_debug args
#   define ILOG(args) dbg_log_info args
#   define WLOG(args) dbg_log_warn args
#   define ELOG(args) dbg_log_err args
#else
#   define VLOG(args) ((void)0)
#   define DLOG(args) ((void)0)
#   define ILOG(args) ((void)0)
#   define WLOG(args) dbg_log_warn args
#   define ELOG(args) dbg_log_err args
#endif

/*!
 * \brief Control the verbosity of the printed logs.
 * \param verbosity Verbosity value.
 * \remark It's a threshold, everything below this verbosity is ignored.
 *         Default is \ref LOG_VERBOSITY_VERBOSE.
 */
REAPI void
dbg_set_verbosity(int verbosity);

/*!
 * \brief Controls if the output should use stderr for ELOG output.
 * \param use_stderr Boolean value, whether to use stderr for ELOG output.
 * \remark It's turned \b OFF by default.
 */
REAPI void
dbg_set_use_stderr(int use_stderr);

/*!
 * \brief Initialize the logging system.
 * \param verbosity Initial verbosity
 * \return Zero on success, non-zero on failure.
 */
REAPI int
dbg_log_init(int verbosity);

/*!
 * \brief Flush logs.
 */
REAPI void
dbg_log_flush(void);

/*!
 * \brief Shutdown the logging system.
 */
REAPI void
dbg_log_shutdown(void);

/*!
 * \brief Print a string to the system log.
 * \param verbosity The desired verbosity of the log message.
 * \param fmt The format string.
 * \param ... Arguments
 *
 * The physical location where to print depends on the target system,
 * on Android it's printed to the LogCat, on iOS to ASL,
 * on desktop to \c stdout.
 * Errors are always also printed to \c stderr.
 */
REAPI void
dbg_log(int verbosity, char const *fmt, ...);

/*!
 * \brief Print a string to stdout.
 * \param color Color, from 0 to 4.
 * \param fmt The format string.
 * \param ... Arguments.
 */
REAPI void
dbg_print(int color, char const *fmt, ...);

/*!
 * \brief Read all buffered logs.
 * \param handler Function pointer which will be called for each log
 * \param userdata Pointer which will be passed to the handler.
 */
void
dbg_log_read(void (*handler)(char const *, u32, void *), void *userdata);

/*!
 * \brief Clear all buffered logs.
 */
void
dbg_log_clear(void);

/*
 * Following functions are just omitting the `verbosity` argument,
 * to be able to be used with macros.
 */
/*! \cond */
REAPI void dbg_log_verbose(char const *fmt, ...);
REAPI void dbg_log_debug(char const *fmt, ...);
REAPI void dbg_log_info(char const *fmt, ...);
REAPI void dbg_log_warn(char const *fmt, ...);
REAPI void dbg_log_err(char const *fmt, ...);
/*! \endcond */

#endif /* DBG_LOG_H */

