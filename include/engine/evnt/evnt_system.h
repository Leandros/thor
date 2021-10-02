/*!
 * \file evnt_system.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief System Events
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef EVNT_SYSTEM_H
#define EVNT_SYSTEM_H

#include <dbg/dbg_assert.h>
#include <sys/sys_macros.h>

/*!
 * \defgroup EVENT Event System
 * \brief Event System
 * @{
 */

typedef struct evnt_event {
    uint msg;
    uptr param1, param2;
} evnt_event;

/* ========================================================================= */
/* Event Messages                                                            */
/* ========================================================================= */

/*!
 * param1:
 * param2:
 */
#define EVNT_QUIT                   (0x0012L)

/*!
 * param1: Width of window.
 * param2: Height of window.
 */
#define EVNT_WM_RESIZED             (0x0005L)

/*!
 * param1: Unused.
 * param2: Unused.
 */
#define EVNT_WM_CLOSED              (0x0010L)

/*!
 * param1: keycode
 * param2: (scancode << 5) | (modifier << 1) | pressed
 */
#define EVNT_KEYDOWN                (0x0100L)

/*!
 * param1: keycode
 * param2: (scancode << 5) | (modifier << 1) | pressed
 */
#define EVNT_KEYUP                  (0x0101L)

/*!
 * param1: character code
 * param2:
 */
#define EVNT_CHAR                   (0x0102L)

/*!
 * param1: Pressed virtual keys
 * param2: x-coord in low-order word. y-coord in high-order word.
 */
#define EVNT_WM_MOUSEMOVE           (0x0200)
#define EVNT_WM_LBUTTONDOWN         (0x0201)
#define EVNT_WM_LBUTTONUP           (0x0202)
#define EVNT_WM_RBUTTONDOWN         (0x0204)
#define EVNT_WM_RBUTTONUP           (0x0205)
#define EVNT_WM_MBUTTONDOWN         (0x0207)
#define EVNT_WM_MBUTTONUP           (0x0208)
#define EVNT_WM_MOUSEWHEEL          (0x020A)
#define EVNT_WM_MOUSEHWHEEL         (0x020E)


/* ========================================================================= */
/* Key Codes                                                                 */
/* ========================================================================= */
#define KEY_INVALID                 (0x000)

#define KEY_SPACE                   (0x020)
#define KEY_APOSTROPHE              (0x027)
#define KEY_COMMA                   (0x02C)
#define KEY_DASH                    (0x02D)
#define KEY_PERIOD                  (0x02E)
#define KEY_SLASH                   (0x02F)
#define KEY_SEMICOLON               (0x03B)
#define KEY_EQUAL                   (0x03D)

#define KEY_LEFT_BRACKET            (0x05B)
#define KEY_BACKSLASH               (0x05C)
#define KEY_RIGHT_BRACKET           (0x05D)
#define KEY_GRAVE_ACCENT            (0x05F)
#define KEY_WORLD_1                 (0x0A1)
#define KEY_WORLD_2                 (0x0A2)

#define KEY_0                       (0x030)
#define KEY_1                       (0x031)
#define KEY_2                       (0x032)
#define KEY_3                       (0x033)
#define KEY_4                       (0x034)
#define KEY_5                       (0x035)
#define KEY_6                       (0x036)
#define KEY_7                       (0x037)
#define KEY_8                       (0x038)
#define KEY_9                       (0x039)

#define KEY_A                       (0x061)
#define KEY_B                       (0x062)
#define KEY_C                       (0x063)
#define KEY_D                       (0x064)
#define KEY_E                       (0x065)
#define KEY_F                       (0x066)
#define KEY_G                       (0x067)
#define KEY_H                       (0x068)
#define KEY_I                       (0x069)
#define KEY_J                       (0x06A)
#define KEY_K                       (0x06B)
#define KEY_L                       (0x06C)
#define KEY_M                       (0x06D)
#define KEY_N                       (0x06E)
#define KEY_O                       (0x06F)
#define KEY_P                       (0x070)
#define KEY_Q                       (0x071)
#define KEY_R                       (0x072)
#define KEY_S                       (0x073)
#define KEY_T                       (0x074)
#define KEY_U                       (0x075)
#define KEY_V                       (0x076)
#define KEY_W                       (0x077)
#define KEY_X                       (0x078)
#define KEY_Y                       (0x079)
#define KEY_Z                       (0x07A)

#define KEY_CIRCUMFLEX              (0x0FF)
#define KEY_BACKSPACE               (0x100)
#define KEY_DELETE                  (0x101)
#define KEY_END                     (0x102)
#define KEY_ENTER                   (0x103)
#define KEY_ESCAPE                  (0x104)
#define KEY_HOME                    (0x105)
#define KEY_INSERT                  (0x106)
#define KEY_MENU                    (0x107)
#define KEY_PAGE_DOWN               (0x108)
#define KEY_PAGE_UP                 (0x109)
#define KEY_PAUSE                   (0x10a)
#define KEY_TAB                     (0x10b)
#define KEY_CAPS_LOCK               (0x10c)
#define KEY_NUM_LOCK                (0x10d)
#define KEY_SCROLL_LOCK             (0x10e)
#define KEY_F1                      (0x10f)
#define KEY_F2                      (0x110)
#define KEY_F3                      (0x111)
#define KEY_F4                      (0x112)
#define KEY_F5                      (0x113)
#define KEY_F6                      (0x114)
#define KEY_F7                      (0x115)
#define KEY_F8                      (0x116)
#define KEY_F9                      (0x117)
#define KEY_F10                     (0x118)
#define KEY_F11                     (0x119)
#define KEY_F12                     (0x11a)
#define KEY_F13                     (0x11b)
#define KEY_F14                     (0x11c)
#define KEY_F15                     (0x11d)
#define KEY_F16                     (0x11e)
#define KEY_F17                     (0x11f)
#define KEY_F18                     (0x120)
#define KEY_F19                     (0x121)
#define KEY_F20                     (0x122)
#define KEY_F21                     (0x123)
#define KEY_F22                     (0x124)
#define KEY_F23                     (0x125)
#define KEY_F24                     (0x126)
#define KEY_LEFT_ALT                (0x127)
#define KEY_LEFT_CONTROL            (0x128)
#define KEY_LEFT_SHIFT              (0x129)
#define KEY_LEFT_SUPER              (0x12a)
#define KEY_PRINT_SCREEN            (0x12b)
#define KEY_RIGHT_ALT               (0x12c)
#define KEY_RIGHT_CONTROL           (0x12d)
#define KEY_RIGHT_SHIFT             (0x12e)
#define KEY_RIGHT_SUPER             (0x12f)
#define KEY_DOWN                    (0x130)
#define KEY_LEFT                    (0x131)
#define KEY_RIGHT                   (0x132)
#define KEY_UP                      (0x133)

#define KEY_KP_0                    (0x135)
#define KEY_KP_1                    (0x136)
#define KEY_KP_2                    (0x137)
#define KEY_KP_3                    (0x138)
#define KEY_KP_4                    (0x139)
#define KEY_KP_5                    (0x13a)
#define KEY_KP_6                    (0x13b)
#define KEY_KP_7                    (0x13c)
#define KEY_KP_8                    (0x13d)
#define KEY_KP_9                    (0x13e)
#define KEY_KP_ADD                  (0x13f)
#define KEY_KP_DECIMAL              (0x140)
#define KEY_KP_DIVIDE               (0x141)
#define KEY_KP_ENTER                (0x142)
#define KEY_KP_MULTIPLY             (0x143)
#define KEY_KP_SUBTRACT             (0x144)


/* Modifier Keys: */
#define KEY_MOD_SHIFT               (0x001)
#define KEY_MOD_CONTROL             (0x002)
#define KEY_MOD_ALT                 (0x004)
#define KEY_MOD_SUPER               (0x008)

#endif /* EVNT_SYSTEM_H */

