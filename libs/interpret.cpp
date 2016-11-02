#if TARGET == LINUX
#include "../linux/projectcfg.h"
#include <new>
#include <stdio.h>
#endif

#include "interpret.h"

Trac::Trac(void) {
//	I = *new RingBuf(IBUFSIZE);
//	A = *new RingBuf(ABUFSIZE);
//	N = *new RingBuf(NBUFSIZE);
//	O = *new RingBuf(OBUFSIZE);
//	R = *new RingBuf(RBUFSIZE);
	meta = (litera) '`';
	notstop = true;
	trace = false;
	radix = 10;
	z = false;
	formlength = 0;
	litcpy(in,"stdin");
	inoffset = 0;
	litcpy(out,"stdout");
	#if CHARSIZE == UNICODE
	  idle = u8"#(ps,#(rs))";
	#endif
	#if CHARSIZE == ANSI
	  idle = "#(ps,#(rs))";
	#endif
}

Trac::~Trac(void) {
	delete &I;
	delete &A;
	delete &N;
	delete &O;
	delete &R;
}

void Trac::run(void){
	while(notstop) {
		// очистить выходной буфер
	  if(O.len > 0) {
			FILE* h;
			if(litcmp(out,"stdout") != 0) {
				h = fopen(in,"a+");
			} else {
				h = stdout;
			}
			while(O.len > 0) {
				fputc(O.get(),h);
			}
		}
    // добавить вызовы прерываний         TODO

		// пополнить входной буфер
	  if(I.len < IBUFSIZE-1) {
			FILE* h;
			if(litcmp(in,"stdin") != 0) {
				h = fopen(in,"r");
				fseek( h, inoffset, SEEK_SET );
			} else {
				h = stdin;
			}
			while(I.len < IBUFSIZE-1) {
				int c = fgetc(h);
				if(c == EOF) { litcpy(in,"stdin"); inoffset = 0; break; }
				I.push((char)c);
				inoffset++;
			}
		}
		// распечатать трассировку
	  if(trace) {}                       // TODO напечатать отладочную информацию
		// выполнить шаг интерпретатора
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
  for(int i = 0; i < n; i++) { bgn = litchr(bgn+1,NEXTPARAM); }
  if(bgn == NULL) return NULL;
  en = litchr(bgn+1,NEXTPARAM);
  if(en == NULL) return NULL;
  int len = (int)(en - bgn);
  litera* o = new litera[len];
#if CHARSIZE == UNICODE
  litncpy(o, (litera*)(bgn+1), len-1);
  o[len-1] = (litera)0;
#endif
#if CHARSIZE == ANSI
  litncpy(o, (litera*)(bgn+1), len-1);
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
			  case (litera)'d': ad(f); free(f); free(first); return;
			  case (litera)'n': an(f); free(f); free(first); return;
			  default: break;
		  }
		  break;
		  case (litera)'b':
				switch(first[1]){
					case (litera)'r': br(f); free(f); free(first); return;
					case (litera)'s': bs(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'c':
				switch(first[1]){
					case (litera)'c': cc(f); free(f); free(first); return;
					case (litera)'l': cl(f); free(f); free(first); return;
					case (litera)'m': cm(f); free(f); free(first); return;
					case (litera)'n': cn(f); free(f); free(first); return;
					case (litera)'r': cr(f); free(f); free(first); return;
					case (litera)'s': cs(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'d':
				switch(first[1]){
					case (litera)'a': da(f); free(f); free(first); return;
					case (litera)'d': dd(f); free(f); free(first); return;
					case (litera)'s': ds(f); free(f); free(first); return;
					case (litera)'v': dv(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'e':
				switch(first[1]){
					case (litera)'b': eb(f); free(f); free(first); return;
					case (litera)'q': eq(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'f':
				switch(first[1]){
					case (litera)'b': fb(f); free(f); free(first); return;
					case (litera)'d': fd(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'g':
				switch(first[1]){
					case (litera)'m': gm(f); free(f); free(first); return;
					case (litera)'r': gr(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'h':
				switch(first[1]){
					case (litera)'l': hl(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'l':
				switch(first[1]){
					case (litera)'t': lt(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'m':
				switch(first[1]){
					case (litera)'l': ml(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'n':
				switch(first[1]){
					case (litera)'l': nl(f); free(f); free(first); return;
					case (litera)'o': no(f); free(f); free(first); return;
					case (litera)'p': np(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'o':
				switch(first[1]){
					case (litera)'r': or_(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'p':
				switch(first[1]){
					case (litera)'b': pb(f); free(f); free(first); return;
					case (litera)'s': ps(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'r':
				switch(first[1]){
					case (litera)'c': rc(f); free(f); free(first); return;
					case (litera)'s': rs(f); free(f); free(first); return;
					case (litera)'t': rt(f); free(f); free(first); return;
					case (litera)'x': rx(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'s':
				switch(first[1]){
					case (litera)'b': sb(f); free(f); free(first); return;
					case (litera)'i': si(f); free(f); free(first); return;
					case (litera)'o': so(f); free(f); free(first); return;
					case (litera)'s': ss(f); free(f); free(first); return;
					case (litera)'u': su(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'t':
				switch(first[1]){
					case (litera)'f': tf(f); free(f); free(first); return;
					case (litera)'r': tr(f); free(f); free(first); return;
					default: break;
				}
			break;
		  case (litera)'x':
				switch(first[1]){
					case (litera)'r': xr(f); free(f); free(first); return;
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
		if(first[0] == (litera)'+') { ad(f); free(f); free(first); return;}
		if(first[0] == (litera)'-') { sb(f); free(f); free(first); return;}
		if(first[0] == (litera)'*') { ml(f); free(f); free(first); return;}
		if(first[0] == (litera)'/') { dv(f); free(f); free(first); return;}
		if(first[0] == (litera)'&') { an(f); free(f); free(first); return;}
		if(first[0] == (litera)'|') { or_(f); free(f); free(first); return;}
		if(first[0] == (litera)'^') { xr(f); free(f); free(first); return;}
		if(first[0] == (litera)'!') { no(f); free(f); free(first); return;}
		if(first[0] == (litera)'=') { eq(f); free(f); free(first); return;}
		if(first[0] == (litera)'<') { lt(f); free(f); free(first); return;}
		if(first[0] == (litera)'>') { gt(f); free(f); free(first); return;}
	}
	if(first[2] == 0) {
		if(first[0] == (litera)'<' && first[1] == (litera)'<') { bs(f); free(f); free(first); return;}
		if(first[0] == (litera)'>' && first[1] == (litera)'>') { br(f); free(f); free(first); return;}
		if(first[0] == (litera)'=' && first[1] == (litera)'=') { eq(f); free(f); free(first); return;}
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
