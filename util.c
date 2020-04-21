/*
 * util.c - commonly used utility functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
void *checked_malloc(int len)
{
  void *p = malloc(len);
  if (!p) 
  {
    fprintf(stderr,"\nRan out of memory!\n");
    exit(1);
  }
 return p;
}

string String(char *s)
{
  string p = checked_malloc(strlen(s)+1);
  strcpy(p,s);
  return p;
}

/**
 * 将数字转为字符串
 */ 
char* itoa(int num)
{
    char* tmp = (char*)malloc(sizeof(char)*128);
    sprintf(tmp,"%d",num);
    return tmp;
}
