#if TARGET == LINUX
#include "../linux/projectcfg.h"
//#if CHARSIZE == ANSI
//typedef char	litera;
//#endif
//#if CHARSIZE == UNICODE
//typedef uint16_t	litera;
//#endif
#endif

#include "litera.h"
#include <sstring>

#if CHARSIZE == ANSI
int litlen(litera* s) { return strlen((char*)s);}
litera* litcpy(litera* d, litera* s) { return (char*) strcpy((char*)d,(char*)s);}
litera* litncpy(litera* d, litera* s, int n) { return (char*) strncpy((char*)d,(char*)s,n);}
int litcmp(litera* d, litera* s) { return strcmp((char*)d,(char*)s); }
litera* litlit(litera* s, litera* sub) { return (litera*)strtsr((char*)s, (char*)sub); }
litera* litchr(litera* s, litera c) { return (litera*)strchr(s(char*),(char)c); }

#elif CHARSIZE == UNICODE
int litlen(litera* s) {}                                            // big TODO
litera* litcpy(litera* d, litera* s) {}
litera* litncpy(litera* d, litera* s, int n) {}
int litcmp(litera* d, litera* s) {}
litera* litlit(litera* s, litera* sub) {}
litera* litchr(litera* s, litera c) {}
#endif
