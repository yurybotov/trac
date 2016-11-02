#ifndef __LITERA_H__
#define __LITERA_H__

int litlen(litera* s);
litera* litcpy(litera* d, litera* s);
litera* litncpy(litera* d, litera* s, int n);
int litcmp(litera* d, litera* s);
int litncmp(litera* d, litera* s, int n);
litera* litlit(litera* s, litera* sub);
litera* litchr(litera* s, litera c);

#endif // __LITRA_H__
