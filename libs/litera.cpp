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
litera* num2lit(num i, int r) { return (litera*)NULL; }
litera* int2lit(int i, int r) { return (litera*)NULL; }

#elif CHARSIZE == UNICODE
int litlen(litera* s) { return 0;}                                  // big TODO
litera* litcpy(litera* d, litera* s) {return (litera*)NULL;}
litera* litncpy(litera* d, litera* s, int n) {return (litera*)NULL;}
int litcmp(litera* d, litera* s) { return 0;}
litera* litlit(litera* s, litera* sub) {return (litera*)NULL;}
litera* litchr(litera* s, litera c) {return (litera*)NULL;}
litera* num2lit(num i, int r) { return (litera*)NULL; }
litera* int2lit(int i, int r) { return (litera*)NULL; }
#endif
