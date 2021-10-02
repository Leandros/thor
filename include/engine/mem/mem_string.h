/*!
 * \file mem_string.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Dynamic string.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef MEM_STRING_H
#define MEM_STRING_H

#include <sys/sys_types.h>
#include <sys/sys_dll.h>

/*!
 * \defgroup MEM_STRING String
 * \brief String data structure.
 * @{
 */

/*!
 * \brief Opaque string type.
 */
typedef union mem_str_union mem_string;

/*!
 * \brief Factor for resizing the string.
 */
#ifndef MEM_STRING_RESIZE
    #define MEM_STRING_RESIZE 2
#endif


/*!
 * \brief
 * \param str
 * \return
 */
REAPI mem_string *
mem_str_init(char const *str, size_t len);

/*!
 * \brief
 * \param str
 */
REAPI void
mem_str_free(mem_string *str);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI char *
mem_str_str(mem_string *str);

/*!
 * \brief
 * \param str
 */
REAPI void
mem_str_set(mem_string *str, char const *s);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI char
mem_str_get(mem_string *str, size_t i);

/*!
 * \brief
 * \param str
 */
REAPI void
mem_str_clear(mem_string *str);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI mem_string *
mem_str_prepend(mem_string *str, char const * s);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI mem_string *
mem_str_append(mem_string *str, char const *s);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI int
mem_str_push(mem_string *str, char c);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI char
mem_str_pop(mem_string *str);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI int
mem_str_cmp(mem_string *lhs, mem_string *rhs);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI int
mem_str_insert(mem_string *str, size_t pos, char const *s);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI int
mem_str_erase(mem_string *str, size_t pos, size_t count);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI int
mem_str_replace(mem_string *str, size_t pos, size_t count, char const *s);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI mem_string *
mem_str_substr(mem_string *str, size_t pos, size_t count);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI size_t
mem_str_reserve(mem_string *str, size_t size);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI size_t
mem_str_shrink(mem_string *str);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI int
mem_str_empty(mem_string *str);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI size_t
mem_str_length(mem_string *str);

/*!
 * \brief
 * \param str
 * \return
 */
REAPI size_t
mem_str_maxsize(mem_string *str);

/*! @} */

#endif /* MEM_STRING_H */

