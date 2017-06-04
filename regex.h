/*
 * $Id: regex.h,v 1.1 2004/09/27 07:06:51 mmr Exp $
 */

#include <sys/types.h> /* off_t, reg* */
#include <regex.h>  /* regcomp, regexec, regfree */

int b1n_regMatch(const char*, char*, size_t, regmatch_t[]);
int b1n_substr(const char*, char*, off_t, off_t, off_t);
