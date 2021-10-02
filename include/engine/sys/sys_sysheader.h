/*!
 * \file sys_sysheader.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Platform agnostic system header includes.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_SYSHEADER_H
#define SYS_SYSHEADER_H

#if !defined(MCPP_RUNNING)

#include <sys/sys_platform.h>

#if IS_LINUX

    #include <sys/types.h>

    #include <sys/resource.h>
    #include <sys/ptrace.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sched.h>

    #if IS_ANDROID
        #include <sys/syscall.h>
    #endif

#elif IS_WIN32 || IS_WIN64

#include <sys/sys_windows.h>

#elif IS_OSX || IS_BSD

    #include <sys/types.h>

    #include <sys/resource.h>
    #include <sys/ptrace.h>
    #include <sys/sysctl.h>
    #include <sys/param.h>
    #include <sys/wait.h>
    #include <unistd.h>

    #if IS_OSX || IS_IOS
        #include <mach/task.h>
        #include <mach/mach_init.h>
        #include <mach/thread_policy.h>
        #include <mach/thread_act.h>
    #endif

#endif /* CHAIN */
#endif /* !defined(MCPP_RUNNING) */

#endif /* SYS_SYSHEADER_H */

