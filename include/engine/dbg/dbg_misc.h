/*!
 * \file dbg_misc.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Misc debug functions.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef DBG_MISC_H
#define DBG_MISC_H

#include <sys/sys_types.h>
#include <sys/sys_dll.h>

/*!
 * \brief Checks if a debugger is attached.
 */
REAPI int
dbg_debugger_attached(void);

#endif /* DBG_MISC_H */
