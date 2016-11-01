#if TARGET == LINUX
#include "../linux/projectcfg.h"
#include <new>
#endif

#include "interpret.h"

Trac::Trac(void) {
	I = *new RingBuf(IBUFSIZE);
	A = *new RingBuf(ABUFSIZE);
	N = *new RingBuf(NBUFSIZE);
	O = *new RingBuf(OBUFSIZE);
	R = *new RingBuf(RBUFSIZE);
	L = *new TracLib();
	meta = (litera) '`';
	notstop = true;
	trace = false;
	radix = 10;
	z = false;
	formlength = 0;
	litcpy(in,"stdin");
	litcpy(out,"stdout");
	#if CHARSIZE == UNICODE
	  idle = u8"#(ps,#(rs))";
	#endif
	#if CHARSIZE == ANSI
	  idle = "#(ps,#(rs))"
	#endif
}

Trac::~Trac(void) {
	delete I;
	delete A;
	delete N;
	delete O;
	delete R;
}

void Trac::run(void){
	while(notstop) {
	  if(O.len > 0) {} // copy o to stdout
	  if(I.len > 0) {} // copy stdin to a
	  if(trace) {}     // напечатать отладочную информацию
	  if(A.len > 0) doStep();
  }
}

// поиск парной скобки
int searchparent(void) {
  int cnt = 0;
  for (int i = 0; i < A.length(); i++) {
    if (A.peek(i) == ((litera)'(')) { cnt++; } else {
      if (A.peek(i) == ((litera)')')) {
        if (cnt == 0) { return i+1; } else { cnt--; }
      }
    }
  }
  return -1;
}

void Trac::doStep(void){
	litera* ptr;
	// если активная строка пуста - рестарт
	if (A.length() == 0) {
		N.clear();
		A.push(idle);
		z = false;
	} else {
		//  если ( - ищем закрывающую скобку и все это вместе уносим в нейтральную цепочку (без скобок)
		if (A.peek(0) == ((litera)'(')) {
			A.dropfirst();
			int parent = searchparent();
			// если не нашли закрывающую скобку - рестарт
			if(parent == -1) {
				N.clear();
				A.push(idle);
				z = false;
				return;
			}
			ptr = A.cutstart(parent-1);
			A.dropfirst();
			N.push(ptr);
			if(ptr != NULL) delete ptr;
		}
		else {
			// если псевдо-пробелы - выкидываем их
			if (A.peek(0) == ((litera)'\r') || A.peek(0) == ((litera)'\n') || A.peek(0) == ((litera)'\t')) {
				 A.dropfirst();
			}
			else {
				// если запятая вставляем признак параметра
				if (A.peek(0) == ((litera)',')) {
					 A.dropfirst();
					 N.push(NEXTPARAM);
				}
				else {
					// если ##( - вставляем признак нейтральной функции
					if (A.peek(0) == ((litera)'#') && A.peek(1) == ((litera)'#') && A.peek(2) == ((litera)'(') {
						A.dropfirst();A.dropfirst();A.dropfirst();
						N.push(NEYTRALFUNC);
					}
					else {
						// если #( - вставляем признак активной функции
						if (A.peek(0) == ((litera)'#') && A.peek(1) == ((litera)'(')) {
							A.dropfirst();A.dropfirst();
							N.push(ACTIVEFUNC);
						}
						else {
							// если ) - зпускаем функцию на выполнение
							if (A.peek(0) == ((litera)')')) {
								A.dropfirst();
								N.push(NEXTPARAM);
								N.push(ENDFUNC);
								execute(); // вызов функции
							}
							else {
								// все остальное перемещаем в нейтральную цепочку
								N.push(A.get());
							}
						}
					}
				}
			}
		}
	}
}

void Trac::feed(litera* s) {
}

litera* Trac::out(void) {
}

void Trac::parse(void) {
}

void Trac::execute(void) {
	litera* ptr;
	// ищем начало функции в нейтральной цепочке
	int af = N.lastindexof(ACTIVEFUNC);
	int nf = N.lastindexof(NEYTRALFUNC);
	int fstart = (af > nf) ? af : nf;
	if(fstart > -1) {
		// запускаем стандартный обработчик функций
		runstdtrac( fstart);
		// помещение результата функции если он не пустой
		if( R.length() > 0) {
			ptr = R.asstring();
			if (N.peek(fstart) == NEYTRALFUNC && z == false) {
				// если нейтральная функция то в конец нейтральной цепочки
				N.push(ptr);
			}
			else
			{
			 // если активная функция то в начало активной цепочки
				A.put(ptr);
			}
			if(ptr != NULL) delete ptr;
		}
	}
	z = false;
	R.clear();
}

// возвращает строку параметра с номером n
litera* param(litera* start, int n) {
  litera *bgn = start, *en;
  for(int i = 0; i < n; i++) { bgn = strchr(bgn+1,NEXTPARAM); }
  if(bgn == NULL) return NULL;
  en = strchr(bgn+1,NEXTPARAM);
  if(en == NULL) return NULL;
  int len = (int)(en - bgn);
  litera* o = new litera[len];
#if CHARSIZE == UNICODE
  strncpy(o, (char*)(bgn+1), (len-1)*2);
  o[len-1] = (litera)0;
#endif
#if CHARSIZE == ANSI
  strncpy(o, (char*)(bgn+1), len-1);
  o[len-1] = (litera)0;
#endif
  return o;
}

void Trac::runstdtrac(int start) {
	litera* f = N.cutend(fstart);
	litera* first = param(f,0);
	if(first[2] == 0) {
	  switch(first[0]) {
		  case (litera)'a':
		  switch(first[1]){
			  case (litera)'d': L.ad(f); free(f); free(first); return;
			  case (litera)'n': L.an(f); free(f); free(first); return;
			  default: break;
		  }
		  break;
		  case (litera)'b':
				switch(first[1]){
					case (litera)'r': L.br(f); free(f); free(first); return;
					case (litera)'s': L.bs(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'c':
				switch(first[1]){
					case (litera)'c': L.cc(f); free(f); free(first); return;
					case (litera)'l': L.cl(f); free(f); free(first); return;
					case (litera)'m': L.cm(f); free(f); free(first); return;
					case (litera)'n': L.cn(f); free(f); free(first); return;
					case (litera)'r': L.cr(f); free(f); free(first); return;
					case (litera)'s': L.cs(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'d':
				switch(first[1]){
					case (litera)'a': L.da(f); free(f); free(first); return;
					case (litera)'d': L.dd(f); free(f); free(first); return;
					case (litera)'s': L.ds(f); free(f); free(first); return;
					case (litera)'v': L.dv(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'e':
				switch(first[1]){
					case (litera)'b': L.eb(f); free(f); free(first); return;
					case (litera)'q': L.eq(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'f':
				switch(first[1]){
					case (litera)'b': L.fb(f); free(f); free(first); return;
					case (litera)'d': L.fd(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'g':
				switch(first[1]){
					case (litera)'m': L.gm(f); free(f); free(first); return;
					case (litera)'r': L.gr(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'h':
				switch(first[1]){
					case (litera)'l': L.hl(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'l':
				switch(first[1]){
					case (litera)'t': L.lt(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'m':
				switch(first[1]){
					case (litera)'l': L.ml(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'n':
				switch(first[1]){
					case (litera)'l': L.nl(f); free(f); free(first); return;
					case (litera)'o': L.no(f); free(f); free(first); return;
					case (litera)'p': L.np(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'o':
				switch(first[1]){
					case (litera)'r': L.or(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'p':
				switch(first[1]){
					case (litera)'b': L.pb(f); free(f); free(first); return;
					case (litera)'s': L.ps(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'r':
				switch(first[1]){
					case (litera)'c': L.rc(f); free(f); free(first); return;
					case (litera)'s': L.rs(f); free(f); free(first); return;
					case (litera)'t': L.rt(f); free(f); free(first); return;
					case (litera)'x': L.rx(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'s':
				switch(first[1]){
					case (litera)'b': L.sb(f); free(f); free(first); return;
					case (litera)'i': L.si(f); free(f); free(first); return;
					case (litera)'o': L.so(f); free(f); free(first); return;
					case (litera)'s': L.ss(f); free(f); free(first); return;
					case (litera)'u': L.su(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'t':
				switch(first[1]){
					case (litera)'f': L.tf(f); free(f); free(first); return;
					case (litera)'r': L.tr(f); free(f); free(first); return;
					default: break;
				}
			break;
//		  case (litera)'u':
//				switch(first[1]){
//					case (litera)'n': L.un(f); free(f); free(first); return;
//					case (litera)'s': L.us(f); free(f); free(first); return;
//					default: break;
//				}
//			break;
		  case (litera)'x':
				switch(first[1]){
					case (litera)'r': L.xr(f); free(f); free(first); return;
					default: break;
				}
			break;
		  default: break;
	  }
  }
#if EXTENDEDSYNTAX == TRUE
	runexttrac( first, f);
#else
	runuser( first, f);
#endif
}

#if EXTENDEDSYNTAX == TRUE
void Trac::runexttrac(litera* first, litera* f) {
  if(first[1] == 0) {
		if(first[0] == (litera)'+') { L.ad(f); free(f); free(first); return;}
		if(first[0] == (litera)'-') { L.sb(f); free(f); free(first); return;}
		if(first[0] == (litera)'*') { L.ml(f); free(f); free(first); return;}
		if(first[0] == (litera)'/') { L.dv(f); free(f); free(first); return;}
		if(first[0] == (litera)'&') { L.an(f); free(f); free(first); return;}
		if(first[0] == (litera)'|') { L.or(f); free(f); free(first); return;}
		if(first[0] == (litera)'^') { L.xr(f); free(f); free(first); return;}
		if(first[0] == (litera)'!') { L.no(f); free(f); free(first); return;}
		if(first[0] == (litera)'=') { L.eq(f); free(f); free(first); return;}
		if(first[0] == (litera)'<') { L.lt(f); free(f); free(first); return;}
		if(first[0] == (litera)'>') { L.gt(f); free(f); free(first); return;}
	}
	if(first[2] == 0) {
		if(first[0] == (litera)'<' && first[1] == (litera)'<') { L.bs(f); free(f); free(first); return;}
		if(first[0] == (litera)'>' && first[1] == (litera)'>') { L.br(f); free(f); free(first); return;}
		if(first[0] == (litera)'=' && first[1] == (litera)'=') { L.eq(f); free(f); free(first); return;}
	}
	runuser(first,f);
}
#endif

void Trac::runuser(litera* first, litera* f) {
	litera* ptr1 = param(f,1);
	litera* ptr2 = L.formcall(first, ptr1);
	R.push(ptr2);
	if(ptr1 !=NULL) delete ptr1; if(ptr2 !=NULL) delete ptr2;
	if( f != NULL) delete f; if( first != NULL) delete first;
	return;
}
