#if TARGET == LINUX
#include "../linux/projectcfg.h"
#include <cstring>
#include <new>
#endif

#include "ringbuf.h"

RingBuf::RingBuf(int sz)
{
	size = sz;
	buf = new litera[size];
	clear();
}

RingBuf::~RingBuf(void)
{
	delete[] buf;
}

void RingBuf::clear(void)
{
	b = e = len = 0;
}


void RingBuf::push(litera c)
{
	if (len == size) { error("push overflow"); return; }
  	buf[e] = c;
	if (e != size - 1) { e++; } else { e = 0;}
	len++;
}

litera RingBuf::pop() {
	litera c;
	if(len == 0) { error("pop underflow"); return -1; }
	if (e != 0) { e--; } else { e = size - 1; }
 	c = buf[e];
	len--;
	return c;
}

void RingBuf::put(litera c)
{
	if (len == size) { error("put overflow"); return; }
	if (b != 0) {	b--; } else {	b = size - 1; }
	buf[b] = c;
	len++;
}

litera RingBuf::get(void)
{
	litera c;
	if (len == 0) { error("get underflow"); return -1; }
  	c = buf[b];
	if (b != size - 1) { b++; } else { b = 0; }
	len--;
	return c;
}

void RingBuf::dropfirst(void)
{
  if (len == 0) { error("dropfirst underflow"); return; }
  if (b != size - 1) { b++; } else { b = 0; }
  len--;
}

litera RingBuf::peek(int num)
{
	litera c;
	if (num > len-1 || num < 0) { return ' '; }
	if (b + num < size) { c = buf[b + num]; } else { c = buf[b + num - size]; }
	return c;
}

int RingBuf::indexof(litera ch)
{
	litera c;
	for (int i = 0; i < len; i++) {
		if (b + i < size) { c = buf[b + i]; } else { c = buf[b + i - size]; }
		if (ch == c) { return i; }
	}
	return -1;
}

int RingBuf::lastindexof(litera ch)
{
	litera c;
	for (int i = len-1; i >= 0; i--) {
		if (b + i < size) { c = buf[b + i]; } else { c = buf[b + i - size]; }
		if (ch == c) { return i; }
	}
	return -1;
}

void RingBuf::push(litera* s)
{
  if(s != NULL) {
  	int l = strlen(s);
  	for (int i = 0; i < l; i++) { push(s[i]); }
  }
}

void RingBuf::put(litera* s)
{
  if(s != NULL) {
  	int l = strlen(s);
  	for (int i = l-1; i >= 0; i--) { put(s[i]); }
  }
}

litera* RingBuf::cutstart(int n) {
  if( n == 0) return NULL;
	litera* s = new litera[n + 1];
	for (int i = 0; i < n; i++) { s[i] = get(); }
	s[n] = (litera) 0;
	return s;
}

litera* RingBuf::cutend(int n)
{
  if(n == len) return NULL;
  if(n == 0) return asstring();
  litera* s = new litera[len - n + 1];
  int l = len;
	for (int i = 0; i < l-n; i++) {	s[l - n - i-1] = pop();	}
	s[l-n] = (litera) 0;
	return s;
}

litera* RingBuf::asstring() {
  if(len == 0) return NULL;
  litera* s = new litera[len + 1];
  int l = len;
  for (int i = 0; i < l; i++) { s[i] = get(); }
  s[l] = (litera) 0;
  return s;
}

void RingBuf::error(const char* s)
{
	//Error(s);
}
