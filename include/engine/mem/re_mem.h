/*!
 * \file re_mem.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Engine header for the memory subsystem.
 */
#ifndef RE_MEM_H
#define RE_MEM_H

/*!
 * \brief Initialises the memory subsystem.
 * \return 0 on success, 1 on failure.
 *
 * All memory in the engine is allocated on startup, making memory allocations
 * more predictable, faster, and debuggable. If your game does not require
 * neither of these (or has it's own allocators), you are free to leave
 * \a size at 0 and use mem_sysmalloc() for all your allocations.
 */
int
re_mem_init(void);

/*!
 * \brief Shutdown the memory subsystem.
 * \return 0 on success, 1 on failure.
 */
int
re_mem_shutdown(void);

#endif /* RE_MEM_H */

