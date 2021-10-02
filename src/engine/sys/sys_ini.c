#include <sys/sys_ini.h>
#include <libc/errno.h>
#include <libc/string.h>
#include <libc/ctype.h>

int
sys_ini_parse(FILE *file, struct sys_ini_pair *pairs, size_t len)
{
    return 0;
    /* int c; */
    /* int invalid = 0, comment = 0, section = 0; */
    /* size_t pair = 0, key = 0, val = 0; */
    /* size_t seclen = 0; */
    /* char sec[SYS_INI_SECLEN]; */

    /* /1* Main I/O loop *1/ */
    /* while ((c = fgetc(file)) != EOF) { */
    /*     /1* Stop parsing if we reached the maxpairs *1/ */
    /*     if (pair > len) break; */

    /*     /1* Skip invalid lines gracefully *1/ */
    /*     if (invalid && c != '\n') continue; */
    /*     if (invalid && c == '\n') { */
    /*         invalid = 0; */
    /*         continue; */
    /*     } */

    /*     /1* Skip all whitespace, if we're not in a value. *1/ */
    /*     if (!val && c != '\n' && (c <= ' ' && c > '\0')) continue; */

    /*     /1* Skip comments, if we're not parsing a value. Which disallows *1/ */
    /*     /1* trailing comments, to allow the character to be used in the value. *1/ */
    /*     if (!val && c == ';') { */
    /*         comment = 1; */
    /*         continue; */
    /*     } */

    /*     /1* We're in the middle of a comment, skip it until we encounter *1/ */
    /*     /1* a newline character *1/ */
    /*     if (comment && c != '\n') continue; */
    /*     if (comment && c == '\n') { */
    /*         comment = 0; */
    /*         continue; */
    /*     } */

    /*     /1* Real parsing starts here *1/ */
    /*     /1* Section parsing *1/ */
    /*     if (!val) { */
    /*         if (c == '[') { */
    /*             section = 1; */
    /*             seclen = 0; */
    /*             continue; */
    /*         } else if (c == ']') { */
    /*             sec[seclen] = '\0'; */
    /*             section = 0; */
    /*             continue; */
    /*         } */
    /*         if (section) { */
    /*             if ((seclen) < (SYS_INI_SECLEN - 1)) */
    /*                 sec[seclen++] = (char)c; */
    /*             continue; */
    /*         } */

    /*         /1* Key parsing *1/ */
    /*         /1* Only accept these characters: [A-Za-z_-] as keys. *1/ */
    /*         if (isalpha(c) || c == '_' || c == '-') { */
    /*             if (key == 0) { */
    /*                 memcpy(pairs[pair].key, sec, */
    /*                         seclen < (pairs->lkey - 1) ? seclen : pairs->lkey - 1); */
    /*                 /1* by removing the following line, the section will NOT *1/ */
    /*                 /1* be prepended if the key is to small. *1/ */
    /*                 key = seclen; */
    /*                 if (seclen > 0 && seclen < (pairs->lkey - 1)) { */
    /*                     pairs[pair].key[seclen] = '.'; */
    /*                     key = seclen + 1; */
    /*                 } */
    /*             } */
    /*             if (key < (pairs->lkey - 1)) */
    /*                 pairs[pair].key[key++] = (char)c; */
    /*             continue; */
    /*         } else if (c != '=' && c != '\n') { */
    /*             key = val = 0; */
    /*             invalid = 1; */
    /*             continue; */
    /*         } */

    /*         /1* if we encounter an equal sign, we stop parsing the key *1/ */
    /*         /1* and start parsing the value. *1/ */
    /*         if (c == '=') { */
    /*             pairs[pair].key[key] = '\0'; */
    /*             key = 0; */

    /*             /1* Not really delightful, but we need some indicator, that we *1/ */
    /*             /1* are now parsing the value *1/ */
    /*             val = 1; */
    /*             continue; */
    /*         } */
    /*     } */

    /*     if (c != '\n') { */
    /*         if (val < (pairs->lval - 1)) */
    /*             pairs[pair].val[val++ - 1] = (char)c; */
    /*         continue; */
    /*     } */

    /*     if (c == '\n') { */
    /*         pairs[pair].val[val - 1] = '\0'; */
    /*         if (val > 0) { */
    /*             val = 0; */
    /*             pair++; */
    /*         } */
    /*     } */
    /* } */

    /* return (int)pair; */
}

