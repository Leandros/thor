/*!
 * \file dbg_err.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Error routines.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef DBG_ERR_H
#define DBG_ERR_H

#include <sys/sys_dll.h>

/*!
 * \brief Returns pointer to string describing the error represented by \a err.
 * \returns Pointer to string.
 */
REAPI char const *
dbg_strerror(int err);

/*!
 * \defgroup ErrNums Error Constants
 * \brief Macros expanding into integers representing the most common errors.
 * @{
 */

#define DBG_ERANGE      0

/*! @} */

#endif /* DBG_ERR_H */

