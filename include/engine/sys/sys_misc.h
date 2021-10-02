/*!
 * \file sys_misc.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Misc functions, everything which is otherwise unrelated.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_MISC_H
#define SYS_MISC_H

/*!
 * \brief Retrieve the resolution of the primary monitor
 * \param width Width in pixels. [OUTPUT]
 * \param height Height in pixels. [OUTPUT]
 */
void
sys_misc_monitor_size(int *width, int *height);

/*!
 * \brief Retrieve the number of *active* displays.
 * \return Number of active displays.
 * \remark Does not count inactive or virtual displays.
 */
int
sys_misc_num_monitors(void);

#endif /* SYS_MISC_H */

