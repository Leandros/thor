/*!
 * \file config.h
 * \brief Contains the config with which the engine is compiled
 */
#ifndef RE_CONFIG_H
#define RE_CONFIG_H

/* How to use a feature macro:
 *     #define THIS_IS_A_FEATURE ON
 *     #if USING(THIS_IS_A_FEATURE)
 *         ... do amazing things ...
 *     #endif
 */
#define ON              +
#define OFF             -
#define USING(x)        ((1 x 1) == 2)


/*!
 * \defgroup DEFAULTS Defaults
 * @{
 */

#define GAME_NAME       "Undisclosed Game"
#define GAME_WINSIZE_X  1280
#define GAME_WINSIZE_Y  720

/*! @} */

/*!
 * \defgroup CONSTANTS Constants
 * @{
 */

/*!
 * \brief Tickrate of the fixed-update loop in seconds.
 *        Defaults to \c 0.016666667f
 */
#ifndef TICKRATE
#define TICKRATE 0.016666667f
#endif

/*!
 * \brief The maximum frames which can be skipped on a slow computer.
 *        Defaults to \c 5
 */
#ifndef MAX_FRAME_SKIP
#define MAX_FRAME_SKIP 5
#endif

/*! @} */

/*!
 * \defgroup FEATURES Features
 * \brief Optional features which can be enabled while compiling.
 * @{
 */
/*!
 * \def ENGINE_DEBUG
 * \brief If debugging is enabled.
 * \note Defined in \a Config.bff at compile time.
 *
 * Enabling debugging will do a lot more sanity checks, including, not not
 * exclusively:
 *     - Checking if allocations are corrupted, or freed twice.
 */
#ifndef ENGINE_DEBUG
#error "ENGINE_DEBUG not set"
#endif

/*!
 * \def ENGINE_ASSERTS
 * \brief Enable or disable asserts.
 */
#ifndef ENGINE_ASSERTS
#error "ENGINE_ASSERTS not set"
#endif

/*!
 * \def COLOR_SUPPORT
 * \brief Enable terminal ansi colored output.
 * \note Defined in \a Config.ff at compile time.
 */
#ifndef COLOR_SUPPORT
#error "COLOR_SUPPORT not set"
#endif

/*!
 * \def CONSOLE
 * \brief Console window enabled?
 */
#ifndef ENGINE_CONSOLE
#error "ENGINE_CONSOLE not set"
#endif

/*! @} */

#endif /* RE_CONFIG_H */

