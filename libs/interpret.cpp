#if TARGET == LINUX
#include "../linux/projectcfg.h"
#endif

#include "interpret.h"

Trac::Trac(void) {
	i = *new RingBuf(IBUFSIZE);
	a = *new RingBuf(ABUFSIZE);
	n = *new RingBuf(NBUFSIZE);
	o = *new RingBuf(OBUFSIZE);
}

Trac::~Trac(void) {
	delete i;
	delete a;
	delete n;
	delete o;	
}

void Trac::run(void){
	if(o.len > 0) // copy o to stdout 
	if(i.len > 0) // copy stdin to a
	if(a.len > 0) doStep();
}

void Trac::doStep(void){
}

void Trac::feed(litera* s){
}

litera* Trac::out(void){
}

void Trac::parse(void){
}

void Trac::execute(void){
}

void Trac::runstdtrac(void){
}

void Trac::runuser(void){
}
