#include <sys/sys_misc.h>
#include <sys/sys_windows.h>
#include <dbg/dbg_assert.h>

void
sys_misc_monitor_size(int *width, int *height)
{
    dbg_assert(width != NULL, "width must be non-NULL");
    dbg_assert(height != NULL, "height must be non-NULL");

    *width = GetSystemMetrics(SM_CXSCREEN);
    *height = GetSystemMetrics(SM_CYSCREEN);
}

int
sys_misc_num_monitors(void)
{
    return GetSystemMetrics(SM_CMONITORS);
}

