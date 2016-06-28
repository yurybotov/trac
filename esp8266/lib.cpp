#include "lib.h"
#include <string.h>

/*void dumb(void) {
#ifdef __DBG__
	printf("dumb()\n");
#endif
};*/

RingBuf::RingBuf(int sz)
{
	size = sz;
	buf = new char[size];
	//overflow = underflow = &dumb;
	clear();
}

RingBuf::~RingBuf(void)
{
	delete buf;
}

void RingBuf::clear(void)
{
	b = e = len = 0;
	/*for (int i = 0; i < size; i++) {
		buf[i] = 0;
	}*/
}


void RingBuf::push(char c)
{
	if (len == size) { error("push overflow"); return; }
  buf[e] = c;
	if (e != size - 1) { e++; }	else { e = 0;}
	len++;
}

char RingBuf::pop() {
	char c;
	if(len == 0) { error("pop underflow"); return -1; }
	if (e != 0) { e--; } else { e = size - 1; }
  c = buf[e];
  /*buf[e] = 0;*/
	len--;
	return c;
}

void RingBuf::put(char c)
{
	if (len == size) { error("put overflow"); return; }
	if (b != 0) {	b--; } else {	b = size - 1; }
  buf[b] = c;
	len++;
}

char RingBuf::get(void)
{
	char c;
	if (len == 0) { error("get underflow"); return -1; }
  c = buf[b];
  /*buf[b] = 0;*/
	if (b != size - 1) { b++;	}	else { b = 0; }
	len--;
	return c;
}

void RingBuf::dropfirst(void)
{
  if (len == 0) { error("dropfirst underflow"); return; }
  /*buf[b] = 0;*/
  if (b != size - 1) { b++; } else { b = 0; }
  len--;
}

char RingBuf::peek(int num)
{
	char c;
	if (num > len-1 || num < 0) { return ' '; }
	if (b + num < size) {	c = buf[b + num];	}	else { c = buf[b + num - size]; }
	return c;
}

int RingBuf::indexof(char ch)
{
	int c;
	for (int i = 0; i < len; i++) {
		if (b + i < size) { c = buf[b + i];	}	else { c = buf[b + i - size];	}
		if (ch == c) { return i; }
	}
	return -1;
}

int RingBuf::lastindexof(char ch)
{
	int c;
	for (int i = len-1; i >= 0; i--) {
		if (b + i < size) {	c = buf[b + i];	}	else { c = buf[b + i - size];	}
		if (ch == c) { return i; }
	}
	return -1;
}

void RingBuf::push(char* s)
{
  if(s != NULL) {
  	int l = strlen(s);
  	for (int i = 0; i < l; i++) {	push(s[i]);	}
  }
}

void RingBuf::put(char * s)
{
  if(s != NULL) {
  	int l = strlen(s);
  	for (int i = l-1; i >= 0; i--) { put(s[i]);	}
  }
}

char* RingBuf::cutstart(int n) {
  if( n == 0) return NULL;
	char* s = new char[n + 1];
	for (int i = 0; i < n; i++) {	s[i] = get(); }
	s[n] = 0;
	return s;
}

char* RingBuf::cutend(int n)
{
  if(n == len) return NULL;
  if(n == 0) return asstring();
  char* s = new char[len - n + 1];
  int l = len;
	for (int i = 0; i < l-n; i++) {	s[l - n - i-1] = pop();	}
	s[l-n] = 0;
	return s;
}

char* RingBuf::asstring() {
  if(len == 0) return NULL;
  char* s = new char[len + 1];
  int l = len;
  for (int i = 0; i < l; i++) { s[i] = get(); }
  s[l] = 0;
  return s;
}

void Error(char* s);

void RingBuf::error(char* s)
{
	Error(s);
}
