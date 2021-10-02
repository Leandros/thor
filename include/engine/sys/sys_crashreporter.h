/*!
 * \file sys_crashreporter.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Crash reporting.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_CRASHREPORTER_H
#define SYS_CRASHREPORTER_H

#include <sys/sys_dll.h>

/*!
 * \brief Starts the crash reporter.
 * \return Pointer to reporter state.
 * \remark Should be called as \b EARLY as possible!
 */
REAPI struct reporter_state *
sys_crashreporter_start(void);

/*!
 * \brief Shuts down the crash reporter.
 * \param state Pointer to valid reporter state.
 * \remark Should be called as \b LATE as possible!
 */
REAPI void
sys_crashreporter_shutdown(struct reporter_state *state);

#endif /* SYS_CRASHREPORTER_H */

