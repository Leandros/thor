/* Just one simple .c file which you have to compile. Simple as that! */
#include <libc/bits/noreturn.h>
#include <libc/bits/restrict.h>
#include <libc/ctype.h>
#include <libc/errno.h>
#include <libc/limits.h>
#include <libc/math.h>
#include <libc/signal.h>
#include <libc/stdarg.h>
#include <libc/stdatomic.h>
#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/string.h>


/* stdio: */
#include <libc/stdio/fclose.c>
#include <libc/stdio/fflush.c>
#include <libc/stdio/fopen.c>
#include <libc/stdio/fprintf.c>
#include <libc/stdio/fread.c>
#include <libc/stdio/fseeko.c>
#include <libc/stdio/ftello.c>
#include <libc/stdio/fwrite.c>
#include <libc/stdio/getchar.c>
#include <libc/stdio/printf.c>
#include <libc/stdio/puts.c>
#include <libc/stdio/rewind.c>
#include <libc/stdio/snprintf.c>
#include <libc/stdio/vfprintf.c>
#include <libc/stdio/vprintf.c>
#include <libc/stdio/vsnprintf.c>


/* stdlib: */
#include <libc/stdlib/_Exit.c>
#include <libc/stdlib/abort.c>
#include <libc/stdlib/atoi.c>
#include <libc/stdlib/atol.c>
#include <libc/stdlib/atoll.c>
#include <libc/stdlib/exit.c>
#include <libc/stdlib/malloc.c>
#include <libc/stdlib/strtof.c>
#include <libc/stdlib/strtol.c>
#include <libc/stdlib/atof.c> /* depends on strtod. because lazy. */
#include <libc/stdlib/wcrtomb.c>
#include <libc/stdlib/wctomb.c>


/* string: */
#include <libc/string/memcmp.c>
#include <libc/string/memcpy.c>
#include <libc/string/memmove.c>
#include <libc/string/memset.c>
#include <libc/string/memchr.c>
#include <libc/string/strchr.c> /* strchr, strrchr */
#include <libc/string/strcmp.c>
#include <libc/string/strcpy.c>
#include <libc/string/strdup.c>
#include <libc/string/strerror.c>
#include <libc/string/strlen.c>
#include <libc/string/strncmp.c>
#include <libc/string/strncpy.c>
#include <libc/string/strstr.c>


/* wchar: */
#include <libc/wchar/wcslen.c>


/* math: */
#include <libc/math/__cosdf.c>
#include <libc/math/__cos.c>
#include <libc/math/__sindf.c>
#include <libc/math/__sin.c>
#include <libc/math/__tandf.c>
#include <libc/math/__tan.c>
#include <libc/math/__rem_pio2_large.c>
#include <libc/math/__rem_pio2f.c>
#include <libc/math/__rem_pio2.c>
#include <libc/math/__fpclassify.c>
#include <libc/math/__fpclassifyf.c>
#include <libc/math/__fpclassifyl.c>
#include <libc/math/copysign.c>
#include <libc/math/frexp.c>
#include <libc/math/frexpf.c>
#include <libc/math/frexpl.c>
#include <libc/math/scalbn.c>
#include <libc/math/floor.c>
#include <libc/math/sqrt.c>
#include <libc/math/fabs.c>
#include <libc/math/fmod.c>
#include <libc/math/log.c>
#include <libc/math/cos.c>
#include <libc/math/sin.c>
#include <libc/math/tan.c>


/* time: */
#include <libc/time/__month_to_secs.c>
#include <libc/time/__year_to_secs.c>
#include <libc/time/__tm_to_time_t.c>
#include <libc/time/__time_t_to_tm.c>
#include <libc/time/localtime.c>
#include <libc/time/mktime.c>
#include <libc/time/time.c>


/* misc: */
#include <libc/misc/assert.c>


/* C Run-Time: */
#include <libc/tlssup.c>
#include <libc/startup.c>

