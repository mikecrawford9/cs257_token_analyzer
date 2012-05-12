// Primitive functions for class 267 project
// All primitives are prefixed by "P267_".
// Bieu Do, Fall 2011
// Primitive function headers for CS267
#ifndef _P267_PRIMITIVES_H
#define _P267_PRIMITIVES_H

#include <string.h>
#include <ctype.h>
#include <stdio.h>

typedef FILE P267_FILE;

#define P267_IOREAD 1
#define P267_IOWRITE 2
#define P267_IOAPPEND 3

//ASCII aliases
#define P267_NEWLINE 0xA
#define P267_CARRIAGERETURN 0xD
#define P267_EXCLAMATION 0x21
#define P267_TAB 0x9
#define P267_QUESTION 0x3F
#define P267_PERIOD 0x2E
#define P267_SPACE 0x20
#define P267_ENDFILE EOF

P267_FILE * P267_open(const char *sfilename, int mode);
void P267_close(P267_FILE *);
int P267_getcf(P267_FILE *);
int P267_ungetc(int c, P267_FILE *pfd);
int P267_isspace(int);
int P267_isalpha(int c);
int P267_isalphanum(int c);


#endif //_P267_PRIMITIVES_H
