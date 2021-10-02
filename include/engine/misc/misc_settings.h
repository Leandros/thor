/*!
 * \file misc_settings.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Runtime settings.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef MISC_SETTINGS_H
#define MISC_SETTINGS_H

#include <sys/sys_types.h>

/* ========================================================================= */
/* Settings Structure                                                        */
/* !!! DO NOT MODIFY DIRECTLY !!!                                            */
struct misc_settings {
    u16 vsync_interval;
    u16 fullscreen;
};


/* ========================================================================= */
/* Initialization / Destruction                                              */
struct misc_settings *
misc_settings_init(void);


/* ========================================================================= */
/* Modifier                                                                  */
/* ========================================================================= */
void misc_settings_set_vsync(u16 interval);
void misc_settings_set_fullscreen(u16 fullscreen);

#endif /* MISC_SETTINGS_H */

