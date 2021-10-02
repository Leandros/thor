#include <libc/string.h>
#include <sys/sys_argparse.h>
#include <dbg/dbg_log.h>
#include <dbg/dbg_assert.h>

static struct argparse_config defaultConfig = {
    0 /* abortIfNotFound */
};

static int isNull(struct argparse_option *opt)
{
    return opt->shortname == 0
        && opt->longname == NULL
        && opt->arg == NULL
        && opt->flag == NULL;
}

int
sys_parseargs(int argc, char **argv,
        struct argparse_option *options, struct argparse_config *config)
{
    char **argPtr = NULL;
    struct argparse_option *opts;
    struct argparse_config *cfg = config == NULL ? &defaultConfig : config;
    size_t i, j, len, n = (size_t)argc;

    dbg_assert(argc != 0, "argc may not be zero");
    dbg_assert(argv != NULL, "argv may not be NULL");
    dbg_assert(options != NULL, "options may not be NULL");

    /* the first argument is the program name, we don't want to parse that. */
    for (i = 1; i < n; ++i) {
        if (argv[i][0] != '-' && argPtr == NULL) {
            if (cfg->abortIfNotFound)
                return 1;
            continue;
        } else if (argPtr != NULL) {
            /* TODO: Currently only single arguments are supported, add support
             * for multiple arguments as well.
             */
            *argPtr = argv[i];
            argPtr = NULL;
            continue;
        }

        opts = options;
        len = strlen(argv[i]);

        /* Stop parsing anything if we encounter an '--' argument */
        if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == '\0') {
            return 0;
        }

        /* Short option. */
        if (argv[i][0] == '-' && argv[i][1] != '-') {

            /* Parse over every letter in the short options, if found set the
             * flag, and if not found either ignore it or abort.
             */
            for (j = 1; j < len; ++j) {
                while (!isNull(opts)) {
                    if (argv[i][j] == opts->shortname) {
                        dbg_assert(!(opts->flag != NULL && opts->arg != NULL),
                            "both flag and arg are set, only one may be set at a time");

                        if (opts->flag != NULL) {
                            *opts->flag = 1;
                            goto nextletter;
                        } else if (opts->arg != NULL) {
                            argPtr = opts->arg;
                            goto nextarg;
                        } else {
                            ELOG(("Error: sys_parseargs(): Malformed option, missing both flag and arg"));
                            return 1;
                        }
                    }

                    opts++;
                }

                if (cfg->abortIfNotFound)
                    return 1;

nextletter:
                continue;
            }

        /* Long option. */
        } else {
            while (!isNull(opts)) {
                if (strncmp(argv[i] + 2, opts->longname, len) == 0) {
                    dbg_assert(!(opts->flag != NULL && opts->arg != NULL),
                        "both flag and arg are set, only one may be set at a time");

                    if (opts->flag != NULL) {
                        *opts->flag = 1;
                        goto nextarg;
                    } else if (opts->arg != NULL) {
                        argPtr = opts->arg;
                        goto nextarg;
                    } else {
                        ELOG(("Error: sys_parseargs(): Malformed option, missing both flag and arg"));
                        return 1;
                    }
                }

                opts++;
            }

            if (cfg->abortIfNotFound)
                return 1;
        }

nextarg:
        continue;
    }

    return 0;
}
