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
#include <cstring>
#include <string>

#if CHARSIZE == ANSI
int litlen(litera* s) { return strlen((char*)s);}
litera* litcpy(litera* d, litera* s) { return (char*) strcpy((char*)d,(char*)s);}
litera* litncpy(litera* d, litera* s, int n) { return (char*) strncpy((char*)d,(char*)s,n);}
int litcmp(litera* d, litera* s) { return strcmp((char*)d,(char*)s); }
litera* litlit(litera* s, litera* sub) { return (litera*)strstr((char*)s, (char*)sub); }
litera* litchr(litera* s, litera c) { return (litera*)strchr((char*)s,(char)c); }
#if FLOATINGPOINT == TRUE
litera* num2lit(num f, int r) {
  //string s = std::to_string((float)f);
  //litera* o = new litera(s.length()+1);
  //strcpy(o,s.c_str());
  //return o;
  return NULL;
}
#else
litera* num2lit(num i, int r) {
  //string s = std::to_string((int)i);
  //litera* o = new litera(s.length()+1);
  //strcpy(o,s.c_str());
  //return o;
  return NULL;
}
#endif
litera* int2lit(int i, int r) {
  //string s = std::to_string(i);
  //litera* o = new litera(s.length()+1);
  //strcpy(o,s.c_str());
  //return o;
  return NULL;
}

#elif CHARSIZE == UNICODE
int litlen(litera* s) { return 0;}                                  // big TODO
litera* litcpy(litera* d, litera* s) {return (litera*)NULL;}
litera* litncpy(litera* d, litera* s, int n) {return (litera*)NULL;}
int litcmp(litera* d, litera* s) { return 0;}
litera* litlit(litera* s, litera* sub) {return (litera*)NULL;}
litera* litchr(litera* s, litera c) {return (litera*)NULL;}
#if FLOATINGPOINT == TRUE
litera* num2lit(num f, int r) { string s = to_string(f); return s.c_str(); }
#else
litera* num2lit(num i, int r) { string s = to_string(i); return s.c_str(); }
#endif
litera* int2lit(int i, int r) { string s = to_string(i); return s.c_str(); }
#endif
