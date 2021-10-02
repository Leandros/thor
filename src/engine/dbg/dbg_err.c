#include <sys/sys_macros.h>
#include <dbg/dbg_err.h>

static char const *dbg_errs_[] = {
    "Out of range",

    /* always last element, to return an appropriate error message for the
     * case of input to dbg_strerror() being out of range.
     */
    "Error number out of range"
};

char const *
dbg_strerror(int err)
{
    /* make sure there is always a valid string returned. */
    return dbg_errs_[MIN(err, (int)ARRAY_SIZE(dbg_errs_) - 1)];
}

