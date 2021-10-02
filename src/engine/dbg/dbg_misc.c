#include <libc/string.h>
#include <dbg/dbg_log.h>
#include <dbg/dbg_misc.h>
#include <sys/sys_platform.h>
#include <sys/sys_macros.h>
#include <sys/sys_types.h>
#include <sys/sys_sysheader.h>
#include <sys/sys_windows.h>

int
dbg_debugger_attached(void)
{
#if IS_WIN32 || IS_WIN64
    return IsDebuggerPresent();
#elif IS_LINUX
#define SEARCH "TracerPid:\t"
#define SEARCHLEN (sizeof(SEARCH))
    ssize_t len;
    int fd;
    char buf[256];
    char *result;
    int attached = 0;

    if ((fd = open("/proc/self/status", O_RDONLY)) == -1)
        return 0;

    while ((len = read(fd, buf, sizeof(buf))) > 0) {
        buf[len] = '\0';

        result = strstr(buf, SEARCH);
        if (result) {
            attached = !!atoi(result + SEARCHLEN - 1);
            break;
        }
    }

    return attached;

#elif IS_OSX
    /*
     * We're doing multiple checks here, just to be safe. The first check is
     * is checking for attached mach exception handlers, and is taken from:
     * https://zgcoder.net/ramblings/osx-debugger-detection
     *
     * The second check is checking the process information, which gdb,
     * unfortunately, does not set correctly, it'll therefore only find lldb.
     */
    {
        mach_msg_type_number_t count = 0;
        exception_mask_t masks[EXC_TYPES_COUNT];
        mach_port_t ports[EXC_TYPES_COUNT];
        exception_behavior_t behaviours[EXC_TYPES_COUNT];
        thread_state_flavor_t flavours[EXC_TYPES_COUNT];
        exception_mask_t mask = EXC_MASK_ALL
            & ~(EXC_MASK_RESOURCE | EXC_MASK_GUARD);
        kern_return_t result;
        mach_msg_type_number_t portIndex;

        result = task_get_exception_ports(mach_task_self(),
                mask, masks, &count, ports, behaviours, flavours);
        if (result == KERN_SUCCESS) {
            for (portIndex = 0; portIndex < count; ++portIndex) {
                if (MACH_PORT_VALID(ports[portIndex]))
                    return 1;
            }
        }
    }

    {
        int pid = getpid();
        int mib[4];
        size_t len;
        struct kinfo_proc kp;

        len = 4;
        sysctlnametomib("kern.proc.pid", mib, &len);

        mib[3] = pid;
        len = sizeof(kp);
        if (sysctl(mib, 4, &kp, &len, NULL, 0) == -1) {
            return 0;
        } else if (len > 0) {
            return !!(kp.kp_proc.p_flag & P_TRACED);
        }
    }

    return 0;
#else
    return 0;
#endif
}

