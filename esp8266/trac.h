
#ifndef __TRAC_H__
#define __TRAC_H__

#include "lib.h"

#define MAX_SRV_CLIENTS 3

struct form {
  int hash;
  char name[8];
  char* value;
  int ptr;
  int css;
  uint8_t owner;
};

struct user {
  char* name;
  char status;
  char imode;
  char omode;
  char* ifile;
  char* ofile;
  uint32_t lastuse;
  RingBuf* I;
  RingBuf* O;
};

void interpret(void);

#endif // __TRAC_H__
