#include "litera.h"
#include <sstring>

#if CHARSIZE == ANSI
int litlen(litera* s) { return strlen((char*)s);}
litera* litcpy(litera* d, litera* s) { return (char*) strcpy((char*)d,(char*)s);}
litera* litncpy(litera* d, litera* s, int n) { return (char*) strncpy((char*)d,(char*)s,n);}
int litcmp(litera* d, litera* s) { return strcmp((char*)d,(char*)s); }
//int litncmp(litera* d, litera* s, int n) { return strncmp((char*)d,(char*)s,n);}
litera* litlit(litera* s, litera* sub) { return strtsr(s, sub); }
litera* litchr(litera* s, litera c) { return strchr(s,c); }
#endif

#if CHARSIZE == UNICODE
int litlen(litera* s) {}                                            // big TODO
litera* litcpy(litera* d, litera* s) {}
litera* litncpy(litera* d, litera* s, int n) {}
int litcmp(litera* d, litera* s) {}
//int litncmp(litera* d, litera* s, int n) {}
litera* litlit(litera* s, litera* sub) {}
litera* litchr(litera* s, litera c) {}
#endif
