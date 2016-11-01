#ifndef __litera_H__
#define __litera_H__

#if TARGET == LINUX
#include "../linux/projectcfg.h"
//#if CHARSIZE == ANSI
//typedef char	litera;
//#endif
//#if CHARSIZE == UNICODE
//typedef uint16_t	litera;
//#endif
#endif


int litlen(litera* s);
litera* litcpy(litera* d, litera* s);
litera* litncpy(litera* d, litera* s, int n);
int litcmp(litera* d, litera* s);
int litncmp(litera* d, litera* s, int n);
int litcpy(litera* d, litera* s);
litera* litlit(litera* s, litera* sub);
litera* litchr(litera* s, litera c);

#endif
