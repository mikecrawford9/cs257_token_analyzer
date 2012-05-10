// Primitive function implementation for Microsoft Visual C++ 6.0
// for CS267

#include "P267_Primitives.h"

P267_FILE * P267_open(const char *sfilename, int mode)
{
	FILE *fp=NULL;
	switch (mode)
	{
	case P267_IOREAD:
		fp=fopen(sfilename, "r");
		break;
	case P267_IOWRITE:
		fp=fopen(sfilename, "w");
		break;
	case P267_IOAPPEND:
		fp=fopen(sfilename, "a");
		break;
	}

	return fp;
}

void P267_close(P267_FILE *pfd)
{
	fclose(pfd);
}

int P267_getcf(P267_FILE *pfd)
{
	return getc(pfd);
}

int P267_ungetc(int c, P267_FILE *pfd)
{
	return ungetc(c,pfd);
}

int P267_isspace(int c)
{
	return isspace(c);
}

int P267_isalpha(int c)
{
	return isalpha(c);
}

int P267_isalphanum(int c)
{
	return isalnum(c);
}

