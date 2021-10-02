/*!
 * \file sys_ini.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Simple .ini parser.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef SYS_INI_H
#define SYS_INI_H

#include <stdio.h>
#include <stdlib.h>

/*!
 * \defgroup SYS_INI INI parser
 * \brief Used to parse simple engine configuration files.
 * @{
 */

/* Maximum length of section, in bytes + terminating zero. */
#define SYS_INI_SECLEN 50

/*!
 * \brief Structure holding a single key = value pair.
 */
struct sys_ini_pair {
    char *key, *val;
    size_t lkey, lval;
};

/*!
 * \brief Parse the ini file provided via the FILE handle.
 * \param file File handle to .ini file which should be parsed.
 * \param pairs Pointer to an array of re_pair structures.
 * \param len Maximum pairs to parse.
 */
int sys_ini_parse(FILE *file, struct sys_ini_pair *pairs, size_t len);

/*! @} */
#endif /* SYS_INI_H */

