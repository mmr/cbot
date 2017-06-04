/*
 * $Id: regex.c,v 1.1 2004/09/27 07:06:51 mmr Exp $
 */

#include "regex.h"
#include "misc.h"

int
b1n_regMatch(const char* str, char* pattern, size_t nmatch, regmatch_t pmatch[])
{
  regex_t re;

  if(regcomp(&re, pattern, REG_EXTENDED) == 0){
    if(regexec(&re, str, nmatch, pmatch, 0) == 0){
      return b1n_TRUE;
    }
  }
  return b1n_FALSE;
}

int
b1n_substr(const char* str, char* sub, off_t max, off_t so, off_t eo)
{
  off_t i, j;

  if(eo - so <= max){
    for(i=so, j=0; i<eo; i++, j++){
      sub[j] = str[i];
    }
    sub[j] = '\0';
    return b1n_TRUE;
  }

  return b1n_FALSE;
}
