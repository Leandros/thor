/*!
 * \file re_includes.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */


/* ========================================================================= */
/* Generally Important Includes                                              */
#include <state.h>
#include <config.h>

/* ========================================================================= */
/* Always Included                                                           */
#include <sys/sys_spinlock.h>
#include <sys/sys_macros.h>
#include <sys/sys_sched.h>
#include <sys/sys_iconv.h>
#include <sys/sys_types.h>
#include <sys/sys_dll.h>
#include <dbg/dbg_profiler.h>
#include <dbg/dbg_assert.h>
#include <dbg/dbg_log.h>
#include <mem/alloc.h>


/* ========================================================================= */
/* AI                                                                        */
#ifdef WITH_AI
#undef WITH_AI
    #include <ai/re_ai.h>
#endif

/* ========================================================================= */
/* Debug                                                                     */
#ifdef WITH_DBG
#undef WITH_DBG
    #include <dbg/re_dbg.h>
    #include <dbg/dbg_assert.h>
    #include <dbg/dbg_err.h>
    #include <dbg/dbg_log.h>
    #include <dbg/dbg_misc.h>
    #include <dbg/dbg_profiler.h>
#endif

/* ========================================================================= */
/* Event                                                                     */
#ifdef WITH_EVNT
#undef WITH_EVNT
    #include <evnt/evnt_system.h>
#endif

/* ========================================================================= */
/* Graphics                                                                  */
#ifdef WITH_GFX
#undef WITH_GFX
    #include <gfx/re_gfx.h>
    #include <gfx/gfx_bob.h>
    #include <gfx/gfx_buf.h>
    #include <gfx/gfx_camera.h>
    #include <gfx/gfx_command.h>
    #include <gfx/gfx_dds.h>
    #include <gfx/gfx_debug.h>
    #include <gfx/gfx_imgui.h>
    #include <gfx/gfx_inputassembler.h>
    #include <gfx/gfx_outputmerger.h>
    #include <gfx/gfx_rasterizer.h>
    #include <gfx/gfx_resourcelist.h>
    #include <gfx/gfx_shader.h>
    #include <gfx/gfx_state.h>
    #include <gfx/gfx_tex.h>
#endif

/* ========================================================================= */
/* Direct3D 11                                                               */
#ifdef WITH_D3D11
#undef WITH_D3D11
    #include <gfx/d3d11/gfx_d3d11.h>
    #include <gfx/d3d11/gfx_d3d11_state.h>
#endif

/* ========================================================================= */
/* Math                                                                      */
#ifdef WITH_MATH
#undef WITH_MATH
    #include <math/color.h>
    #include <math/ivec3.h>
    #include <math/ivec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
#endif

/* ========================================================================= */
/* Mem                                                                       */
#ifdef WITH_MEM
#undef WITH_MEM
    #include <mem/re_mem.h>
    #include <mem/mem_alloc.h>
    #include <mem/mem_ref.h>
    #include <mem/mem_string.h>
#endif

/* ========================================================================= */
/* Misc                                                                      */
#ifdef WITH_MISC
#undef WITH_MISC
    #include <misc/misc_settings.h>
#endif

/* ========================================================================= */
/* Network                                                                   */
#ifdef WITH_NET
#undef WITH_NET
    #include <net/re_net.h>
    #include <net/net_tcp.h>
    #include <net/net_udp.h>
#endif

/* ========================================================================= */
/* Sound                                                                     */
#ifdef WITH_SND
#undef WITH_SND
    #include <snd/re_snd.h>
#endif

/* ========================================================================= */
/* System                                                                    */
#ifdef WITH_SYS
#undef WITH_SYS
    #include <sys/re_sys.h>
    #include <sys/sys_argparse.h>
    #include <sys/sys_atomic.h>
    #include <sys/sys_crashreporter.h>
    #include <sys/sys_dll.h>
    #include <sys/sys_fibers.h>
    #include <sys/sys_iconv.h>
    #include <sys/sys_ini.h>
    #include <sys/sys_lolpack.h>
    #include <sys/sys_macros.h>
    #include <sys/sys_misc.h>
    #include <sys/sys_platform.h>
    #include <sys/sys_sched.h>
    #include <sys/sys_sysheader.h>
    #include <sys/sys_threads.h>
    #include <sys/sys_timer.h>
    #include <sys/sys_types.h>
#endif

/* ------------------------------------------------------------------------- */
/* Filesystem                                                                */
#ifdef WITH_FS
#undef WITH_FS
    #include <sys/fs/fs_dir.h>
    #include <sys/fs/fs_file.h>
    #include <sys/fs/isanpack.h>
#endif

/* ========================================================================= */
/* UI                                                                        */
#ifdef WITH_UI
#undef WITH_UI
    #include <ui/re_ui.h>
#endif

