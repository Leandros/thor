/*!
 * \file sys_argparse.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Convenient commandline argument parsing.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_ARGPARSE_H
#define SYS_ARGPARSE_H

#include <sys/sys_dll.h>

/*!
 * \defgroup ARGPARSING Argument parsing
 * @{
 */

/*!
 * \brief Structure to define the arguments to parse.
 */
struct argparse_option {
    char shortname;
    char const *longname;
    int *flag;
    char **arg;
};

/*!
 * \brief Structure to configure the parser.
 */
struct argparse_config {
    int abortIfNotFound;
};

/*!
 * \brief Parse arguments.
 * \param argc Number of arguments.
 * \param argv NULL terminated array to all argument strings.
 * \param options Pointer to array holding all sys_argparse_option's.
 * \param config Pointer to a single structure containing the config for the parser.
 *               NULL to use the default config.
 * \return Returns 0 on success, 1 otherwise.
 */
REAPI int
sys_parseargs(int argc, char **argv,
        struct argparse_option *options, struct argparse_config *config);

/*! @} */


#endif /* SYS_ARGPARSE_H */

