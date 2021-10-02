#include <sys/sys_timer.h>
#include <sys/sys_windows.h>

u64
timer_ticks(void)
{
    return GetTickCount64();
}

