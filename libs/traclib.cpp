#if TARGET == LINUX
#include "../linux/projectcfg.h"
#endif

#include "interpret.h"
#include "litera.h"

litera* Trac::loads() {
  litera* s;
  int metaptr = I.indexof(meta);
  if( metaptr != -1) {
    s = I.cutstart(metaptr-1);
    I.dropfirst();
    return s;
  } else
  z = true;
  return NULL;
}

void replaceall( litera* s, litera* find, litera* replace) {
  RingBuf& tmp = *new RingBuf(NBUFSIZE);
  for(int i = 0; i < litlen(s); i++) {
    if(s[i] == f[0]) {
      bool have = true;
      for(int j = 1; j < litlen(find); j++) {
        have &= s[i+j-1] == find[j];
        if(!have) break;
      }
      if(have) {
        tmp.push(replace);
        i += litlen(find)-1;
      } else {
        tmp.push(s[i]);
      }
    } else {
      tmp.push(s[i]);
    }
  }
  delete s;
  s = tmp.asstring();
  delete tmp;
}

litera* Trac::formcall(litera* fst, litera* f) {
  int ref = findform(fst);
  if(ref != -1) {
    for(int i = 0; i < litlen(F[ref].value); i++) {
      if( F[ref].value[i] == FORMBREAK) {
        litera* ptr = param(f,(int)F[ref].value[i+1]);
        R.push(ptr);
        if(ptr != NULL) delete ptr;
        i++;
      } else {
        R.push(F[ref].value[i]);
      }
    }
  }
}

// вычисляет хэш для формы
uint16_t hash(litera* s) {
  uint16_t a = 0;
  for (int i = 0; i < litlen(s); i++) {
    a = ((a << 3) ^ i) + (uint16_t)s[i];
  }
  return a;
}

// ищет номер формы по имени
int Trac::findform(litera* n) {
  uint16_t h = hash(n), res = -1;
  for(int i = 0; i < formlength; i++) {
    if( F[i].hash == h && !litcmp(F[i].name, n)) {res = i; break;}
  }                                           //  TODO strncmp для unicode
  return res;
}

// добавляет форму
int Trac::formadd(litera* fname, litera* fform) {
  int ref;
  int len;
  if (fname != NULL) {
    if((ref = findform(fname)) != -1) {
      // если форма с таким именем есть - заменяем ее
      delete F[ref].value;
      F[ref].value = new litera(litlen(fform)+1);
      litcpy(F[ref].value, fform);
      F[ref].ptr = 0;
      F[ref].css = 0;
    } else {
      // если формы с таким именем нет - создаем новую
      struct form* f = forms + formlength;
      f->hash = hash(fname);
      F[ref].name = new litera(litlen(fname)+1);
      litcpy(F[ref].name, fname);
      F[ref].value = new litera(litlen(fform)+1);
      litcpy(F[ref].value, fform);
      strncpy(f->name, fname, 8);
      f->ptr = 0;
      f->css = 0;
      formlength++;
    }
  }
}

//// управление выполнением
// #(rt) ресет
// ReseT
void Trac::rt(litera* f) {     // TODO в разных сборках по разному
  N.clear();
  A.push(idle);
  z = false;
}
// #(hl) остановка
// HalT
void Trac::hl(litera* f) {
  N.clear();
  A.push(idle);
  z = false;
}
// #(eq,a,b,than,else) сравнение если a == b
// is EQual
void Trac::eq(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  if (litcmp(ptr, ptr1) == 0) {
    ptr2 = param(f,3);
  } else {
    ptr2 = param(f,4);
  }
  R.push(ptr2);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(ptr2 != NULL) delete ptr2;
}
// #(gr,a,b,than,else) сравнение если a > b
// is GReate then
void Trac::gr(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  if (litcmp(ptr, ptr1) > 0) {
    ptr2 = param(f,3);
  } else {
    ptr2 = param(f,4);
  }
  R.push(ptr2);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(ptr2 != NULL) delete ptr2;
}
// #(lt,a,b,than,else) сравнение если a < b
// is LiTtle then
void Trac::lt(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  if (litcmp(ptr, ptr1) < 0) {
    ptr2 = param(f,3);
  } else {
    ptr2 = param(f,4);
  }
  R.push(ptr2);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(ptr2 != NULL) delete ptr2;
}

//// Ввод-вывод

// #(gm) возвращает текущий символ мета
// Get Meta
void Trac::gm(litera* f) {
  R.push(meta);
}
// #(cm) заменяет текущий символ мета. Возвращает пустую строку
// Change Meta
void Trac::cm(litera* f) {
  litera* ptr = param(f,1);
  meta = ptr[0];
  if( ptr != NULL) delete ptr;
}
// #(rs,Z) читает из входного буфера строку от начала до символа мета и
// переносит ее в конец активной цепочки. Мету из буфера выкидываем. Если все
// нормально возвращает пустую строку, в случае ошибки возвращает Z
// Read String
void Trac::rs(litera* f) {
  litera* ptr = loads();
  if(!z) {
    R.push(ptr);
  } else {
    z = true;
    litera* ptr = param(f,1);
    R.push(ptr);
  }
  if( ptr != NULL) delete ptr;
}
// #(rc,Z) читает из входного буфера один символ. Любой, включая мету.
// Возвращает этот символ или Z если ошибка
// Read Character
void Trac::rc(litera* f) {
  if(I.len > 0) {
    R.push( I.get());
  } else {
    z = true;
    litera* ptr = param(f,1);
    R.push(ptr);
    if( ptr != NULL) delete ptr;
  }
}
// #(ps,s) отправляет строку на текущее устройство вывода
// Print String
void Trac::ps(litera* f) {
  litera* ptr = param(f,1);
  O.push(ptr);
  if(ptr !=NULL) delete ptr;
}
// #(si,f) подключить подсистему ввода к файлу с именем f,
// если #(si) - стандартный ввод из терминала. Возвращает пустую строку
// Set Input
void Trac::si(litera* f) {
  litera* ptr = param(f,1);
  if(ptr != NULL) {
    litcpy(in,ptr);
  } else {
    litcpy(in,"stdin");
  }
  inoffset = 0;
  if(ptr !=NULL) delete ptr;
}
// #(so,f) подключить подсистему вывода к файлу с именем f,
// если #(so) - стандартный вывод на терминал. Возвращает пустую строку
// Set Output
void Trac::so(litera* f) {
  litera* ptr = param(f,1);
  if(ptr != NULL) {
    litcpy(out,ptr);
  } else {
    litcpy(out,"stdout");
  }
  if(ptr !=NULL) delete ptr;
}

//// Арифметика

// #(rx,r) установить r-значную систему счисления (число записывается в 10-чной системе)
// RadiX
void Trac::rx(litera* f) {
  litera* ptr = param(f,1);
  radix = strtol(ptr,NULL,10);
  if(ptr != NULL) delete ptr;
}

// #(ad,n1,n2,Z) сложение
// ADd
void Trac::ad(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1+a2,o,radix);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(su,n1,n2,Z) вычитание
// SUbstract
void Trac::su(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1-a2,o,radix);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(ml,n1,n2,Z) умножение
// MuLtiplify
void Trac::ml(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1*a2,o,radix);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(dv,n1,n2,Z) деление
// DiVide
void Trac::dv(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  if (a2 == 0) {
    z = true;
    litera* ptr2 = param(f,3);
    R.push(ptr2);
    if(ptr != NULL) delete ptr;
    if(ptr1 != NULL) delete ptr1;
    if(ptr2 != NULL) delete ptr2;
    return;
  }
  litera* o = new litera[34];
  ltoa(a1/a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}

//// Побитовые операции

// #(an,n1,n2) возвращает побитовое И
// ANd
void Trac::an(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1&a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(or,n1,n2) возвращает побитовое ИЛИ
// OR
void Trac::or(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1|a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(xr,n1,n2) возвращает побитовое XOR
// XoR
void Trac::xr(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1^a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(no,n1) возвращает побитовое NOT
// NOt
void Trac::no(litera* f) {
  litera* ptr = param(f,1);
  int a1 = strtol(ptr,NULL,radix);
  litera* o = new litera[34];
  ltoa(!a1,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(o != NULL) delete o;
}
// #(bs,n1,n2) возвращает n1 сдвинутое влево на n2 бит
// Bit Shift left
void Trac::bs(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1<<a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}
// #(br,n1,n2) возвращает n1 сдвинутое вправо на n2 бит
// Bit shift Right
void Trac::br(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,radix);
  int a2 = strtol(ptr1,NULL,radix);
  litera* o = new litera[34];
  ltoa(a1>>a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
}

//// Отладка

// #(np,smth) нет операции, можно использовать для временного скрытия кода
// No oPeration
void Trac::np(litera* f) {
  return;
}
// #(nl,s) возвращает список имен форм, s - делимитер между именами
// Name List
void Trac::nl(litera* f) {
  litera* ptr = param(f,1);
  litera dl = (ptr == NULL)? ' ' : ptr[0];
  for(int i = 0; i < formlength; i++) {
    R.push(F[i].name);
    R.push(dl);
  }
  if(ptr != NULL) delete ptr;
}
// #(pb,s) возвращает содержимое формы с именем s
// ???
void Trac::pb(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if( ref != -1 ) { R.push(F[ref].value); }                 // TODO сформатировать отладочнгый вывод
  if( ptr != NULL ) delete ptr;
}
// #(tr) начать трассировку
// TRace on
void Trac::tr(litera* f) {
  trace = true;
}
// #(tf) закончить трассировку
// Trace ofF
void Trac::tf(litera* f) {
  trace = false;
}

//// Формы

// #(ds,n,s) создает форму с именем n и значением s. Возвращает пустую строку
// Define String
void Trac::ds(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  formadd( ptr, ptr1);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
}
// #(ss,n,d1,...,dn) нарезка формы с именем N на сегменты ограниченные цепочками
// dx. Возвращает пустую строку
// String Segmentation
void Trac::ss(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ptr != NULL) delete ptr;
  if(ref != -1) {
    for(int i = 2; true ; i++) {
      litera* ptr = param(f,i);
      if(ptr != NULL) {
        litera r[3]; r[0] = (litera)FORMBRAKE; r[1]=(litera)i-1; r[2]=(litera)0;
        replaceall( F[ref].value, ptr, r);
        delete ptr;
      } else { break;}
    }
  }
}
// #(cl,n,d1,...,dn) вызывает форму n как функцию с параметрами dx
// CalL
void Trac::cl(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ptr != NULL) delete ptr;
  if(ref != -1) {
    for(int i = 0; i < litlen(F[ref].value); i++) {
      if( F[ref].value[i] == FORMBREAK) {
        litera* ptr = param(f,(int)F[ref].value[i+1]-1);
        R.push(ptr);
        if(ptr != NULL) delete ptr;
        i++;
      } else {
        R.push(F[ref].value[i]);
      }
    }
  }
}
// #(cs,n,Z) вызов формы с именем N как список. Возвращает одну "следующую"
// запись из списка. Если записей нет или они закончились возвращает Z
// ???
void Trac::cs(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ptr != NULL) delete ptr;
  if(ref != -1) {
    while(1) {
      litera c = F[ref].value[F[ref].ptr++];
      if(c != 0) {
        if(c != (litera)FORMBREAK) {
          R.push(c);
        } else {
          F[ref].ptr++;
          break;
        }
      } else {
        if(R.len == 0) {
          z = true;
          litera* ptr = param(f,2);
          R.push(ptr);
          if(ptr != NULL) delete ptr;
          break;
        } else {
          break;
        }
      }
    }
  }
}
// #(cc,n,Z) вызов формы N как массив символов. Возвращает один"следующий"
// символ. Если вернуть нечего возвращает Z
// ???
void Trac::cc(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ptr != NULL) delete ptr;
  if(ref != -1) {
    F[ref].ptr++;
    litera c = F[ref].value[F[ref].ptr];
    if(c != 0) {
      R.push(c);
    } else {
      z = true;
      litera* ptr = param(f,2);
      R.push(ptr);
      if(ptr != NULL) delete ptr;
    }
  }
}
// #(cn,n,d,Z) вызов формы n как массив полей равной длины d. Возвращает d
// символов, при отрицательном d выдаются символы слева от маркера при
// положительном - справа. При ошибке возвращает Z
// ???
void Trac::cn(litera* f) {}                                           // TODO
// #(fd,n,s,Z) перемещение внутреннего указателя в форме n на начало
// найденной в форме подстроки n. Возвращает пустую строку, а при ошибке - Z
// FinD
void Trac::fd(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int ref = findform(ptr);
  if(ref != -1) {
    char* substr = litlit(F[ref].value, ptr1);
    if(substr != NULL) { F[ref].ptr = (int)(substr - F[ref].value); }
  }
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
}
// #(cr,n) обнуление внутреннего указателя в форме n. Возвращает пустую строку
// Counter Reset
void Trac::cr(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ref != -1) { F[ref].ptr = 0; }
  if(ptr != NULL) delete ptr;
}
// #(dd,n) удаление формы n. Возвращает пустую строку
// ???
void Trac::dd(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ref != -1) {
    delete forms[ref].value;
    delete forms[ref].name;
    for(int i = ref; i < (formlength-1); i++) {
      forms[i].hash = forms[i+1].hash;
      forms[i].name = forms[i+1].name;
      forms[i].value = forms[i+1].value;
      forms[i].ptr = forms[i+1].ptr;
      forms[i].css = forms[i+1].css;
    }
    formlength--;
  }
  if(ptr != NULL) delete ptr;
}
// #(da) удаление всех форм. Возвращает пустую строку.
// Delete All forms
void Trac::da(litera* f) {
  for(int i = 0; i < (formlength-1); i++) {
    if(F[i].value != NULL) delete F[i].value;
    if(F[i].name != NULL) delete F[i].name;
  }
  formlength = 0;
}
// #(sb,f,n1,...,nn) сохранение форм n1.. в блок на диске с именем f.
// Возвращает пустую строку
// Save Block
void Trac::sb(litera* f) {}                                           // TODO
// #(fb,f) загрузить все формы из блока на диске с именем f.
// Возвращает пустую строку
// Fetch Block
void Trac::fb(litera* f) {}                                           // TODO
// #(eb,f) удалить блок на диске с именем N. Возвращает пустую строку
// Erase Block
void Trac::eb(litera* f) {}                                           // TODO
