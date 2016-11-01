#if TARGET == LINUX
#include "../linux/projectcfg.h"
#endif

#include "traclib.h"

litera* loads() {
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

litera loadc() {
  return I.get();
}

litera* formcall(litera* fst, litera* oth) {
   return NULL;                                       // TODO!!!!!!!!!!!!!!!!
}

// вычисляет хэш для формы
int hash(litera* s) {
  int a = 0;
  for (int i = 0; i < strlen(s); i++) {
    a = ((a << 3) ^ i) + (unsigned int)s[i];
  }                                                 // TODO заменитель strlen для unicode
  return a;
}

// ищет номер формы по имени
int findform(litera* n) {
  int h = hash(n), res = -1;
  for(int i = 0; i < formlength; i++) {
    if( forms[i].owner == curuser && forms[i].hash == h && !strncmp(forms[i].name, n, 8)) {res = i; break;}
  }                                           //  TODO strncmp для unicode
  return res;
}

// добавляет форму
int formadd(litera* fname, litera* fform) {
  int ref;
  if (fname != NULL) {
    if((ref = findform(fname)) != -1) {
      delete F[ref].value;
      F[ref].hash = hash(fname);
      strncpy(F[ref].name, fname, 8); /// TODO переделать на длинные имена в хипе
      F[ref].value = fform;
      F[ref].ptr = 0;
      F[ref].css = 0;
      F[ref].owner = curuser;
      // если форма с таким именем есть - заменяем ее
    } else {
      // если формы с таким именем нет - создаем новую
      struct form* f = forms + formlength;
      f->hash = hash(fname);
      strncpy(f->name, fname, 8); /// TODO переделать на длинные имена в хипе
      f->value = fform;
      f->ptr = 0;
      f->css = 0;
      f->owner = curuser;
      formlength++;
    }
  }
}

//// управление выполнением
// #(rt) ресет
void rt(litera* f) {     // TODO в разных сборках по разному
  N.clear();
  A.push(idle);
  z = false;
  return;
}
// #(hl) остановка
void hl(litera* f) {
  N.clear();
  A.push(idle);
  z = false;
  return;
}
// #(eq,a,b,than,else) сравнение если a == b
void eq(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  if (strcmp(ptr, ptr1) == 0) { // TODO проверить правильность
    ptr2 = param(f,3);
  } else {
    ptr2 = param(f,4);
  }
  R.push(ptr2);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(ptr2 != NULL) delete ptr2;
  return;
}
// #(gr,a,b,than,else) сравнение если a > b
void gr(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  if (strcmp(ptr, ptr1) > 0) { // TODO проверить правильность
    ptr2 = param(f,3);
  } else {
    ptr2 = param(f,4);
  }
  R.push(ptr2);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(ptr2 != NULL) delete ptr2;
  return;
}
// #(lt,a,b,than,else) сравнение если a < b
void lt(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  if (strcmp(ptr, ptr1) < 0) { // TODO проверить правильность
    ptr2 = param(f,3);
  } else {
    ptr2 = param(f,4);
  }
  R.push(ptr2);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(ptr2 != NULL) delete ptr2;
  return;
}

//// Ввод-вывод

// #(gm) возвращает текущий символ мета
void gm(litera* f) {
  R.push(meta);
  return;
}
// #(cm) заменяет текущий символ мета. Возвращает пустую строку
void cm(litera* f) {
  litera* ptr = param(f,1);
  meta = ptr[0];
  if( ptr != NULL) delete ptr;
  return;
}
// #(rs,Z) читает из входного буфера строку от начала до символа мета и
// переносит ее в конец активной цепочки. Мету из буфера выкидываем. Если все
// нормально возвращает пустую строку, в случае ошибки возвращает Z
void rs(litera* f) {
  litera* ptr = loads();
  R.push(ptr);
  if( ptr != NULL) delete ptr;
  return;
}
// #(rc,Z) читает из входного буфера один символ. Любой, включая мету.
// Возвращает этот символ или Z если ошибка
void rc(litera* f) {
  R.push( loadc());
  return;
}
// #(ps,s) отправляет строку на текущее устройство вывода
void ps(litera* f) {
  litera* ptr = param(f,1);
  O.push(ptr);
  if(ptr !=NULL) delete ptr;
  return;
}
// #(si,f) подключить подсистему ввода к файлу с именем f,
// если #(si) - стандартный ввод из терминала. Возвращает пустую строку.
void si(litera* f) {}
// #(so,f) подключить подсистему вывода к файлу с именем f,
// если #(so) - стандартный вывод на терминал. Возвращает пустую строку
void so(litera* f) {}

//// Арифметика

// #(ad,n1,n2,Z) сложение
void ad(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1+a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}
// #(su,n1,n2,Z) вычитание
void su(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1-a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}
// #(ml,n1,n2,Z) умножение
void ml(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1*a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}
// #(dv,n1,n2,Z) деление
void dv(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  if (a2 == 0) {
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
  return;
}

//// Побитовые операции

// #(an,n1,n2) возвращает побитовое И
void an(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1&a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}
// #(or,n1,n2) возвращает побитовое ИЛИ
void or(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1|a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}
// #(xr,n1,n2) возвращает побитовое XOR
void xr(litera* f) {  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1^a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;}
// #(no,n1) возвращает побитовое NOT
void no(litera* f) {
  litera* ptr = param(f,1);
  int a1 = strtol(ptr,NULL,10);
  litera* o = new litera[34];
  ltoa(!a1,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(o != NULL) delete o;
  return;
}
// #(bs,n1,n2) возвращает n1 сдвинутое влево на n2 бит
void bs(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1<<a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}
// #(br,n1,n2) возвращает n1 сдвинутое вправо на n2 бит
void br(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int a1 = strtol(ptr,NULL,10);
  int a2 = strtol(ptr1,NULL,10);
  litera* o = new litera[34];
  ltoa(a1>>a2,o,10);
  R.push(o);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  if(o != NULL) delete o;
  return;
}

//// Отладка

// #(np,smth) нет операции, можно использовать для временного скрытия кода
void np(litera* f) {
  return;
}
// #(nl,s) возвращает список имен форм, s - делимитер между именами
void nl(litera* f) {
  litera* ptr = param(f,1);
  litera dl = (ptr == NULL)? ' ' : ptr[0];
  for(int i = 0; i < formlength; i++) {  // TODO проверить правильность использования formlength
    R.push(forms[i].name);
    R.push(dl);
  }
  if(ptr != NULL) delete ptr;
  return;
}
// #(pb,s) возвращает содержимое формы с именем s
void pb(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if( ref != -1 ) { R.push(F[ref].value); } // TODO сформатировать отладочнгый вывод
  if( ptr != NULL ) delete ptr;
  return;
}
// #(tr) начать трассировку
void tr(litera* f) {}
// #(tf) закончить трассировку
void tf(litera* f) {}

//// Формы

// #(ds,n,s) создает форму с именем n и значением s. Возвращает пустую строку
void ds(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  formadd( ptr, ptr1);
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  return;
}
// #(ss,n,d1,...,dn) нарезка формы с именем N на сегменты ограниченные цепочками
// dx. Возвращает пустую строку
void ss(litera* f) {}
// #(cl,n,d1,...,dn) вызывает форму n как функцию с параметрами dx
void cl(litera* f) {}
// #(cs,n,Z) вызов формы с именем N как список. Возвращает одну "следующую"
// запись из списка. Если записей нет или они закончились возвращает Z
void cs(litera* f) {}
// #(cc,n,Z) вызов формы N как массив символов. Возвращает один"следующий"
// символ. Если вернуть нечего возвращает Z
void cc(litera* f) {}
// #(cn,n,d,Z) вызов формы n как массив полей равной длины d. Возвращает d
// символов, при отрицательном d выдаются символы слева от маркера при
// положительном - справа. При ошибке возвращает Z
void cn(litera* f) {}
// #(fd,n,s,Z) перемещение внутреннего указателя в форме n на начало
// найденной в форме подстроки n. Возвращает пустую строку, а при ошибке - Z
void fd(litera* f) {
  litera* ptr = param(f,1);
  litera* ptr1 = param(f,2);
  int ref = findform(ptr);
/*  if(ref != -1) {                                      TODO
    char* substr = strstr(forms[ref].value, ptr1);
    if(substr != NULL) { forms[ref].ptr = (int)(substr - forms[ref].value); }
  }*/
  if(ptr != NULL) delete ptr;
  if(ptr1 != NULL) delete ptr1;
  return;
}
// #(cr,n) обнуление внутреннего указателя в форме n. Возвращает пустую строку
void cr(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ref != -1) { F[ref].ptr = 0; }
  if(ptr != NULL) delete ptr;
  return;
}
// #(dd,n) удаление формы n. Возвращает пустую строку.
void dd(litera* f) {
  litera* ptr = param(f,1);
  int ref = findform(ptr);
  if(ref != -1) {
/*    delete forms[ref].value;                            TODO
    for(int i = ref; i < (formlength-1); i++) {
      forms[i].hash = forms[i+1].hash;
      strncpy(forms[i].name, forms[i+1].name, 8);
      forms[i].value = forms[i+1].value;
      forms[i].ptr = forms[i+1].ptr;
      forms[i].css = forms[i+1].css;
    }
    formlength--;*/
  }
  if(ptr != NULL) delete ptr;
  return;
}
// #(da) удаление всех форм. Возвращает пустую строку.
void da(litera* f) {
  for(int i = 0; i < MAXFORMS; i++) {
    if(F[i].value != NULL) delete F[i].value; // TODO проверить правильность
  }
  formlength = 0;
  return;
}
// #(sb,f,n1,...,nn) сохранение форм n1.. в блок на диске с именем f.
// Возвращает пустую строку
void sb(litera* f) {}
// #(fb,f) загрузить все формы из блока на диске с именем f.
// Возвращает пустую строку
void fb(litera* f) {}
// #(eb,f) удалить блок на диске с именем N. Возвращает пустую строку
void eb(litera* f) {}

//// Пользователи

// #(un) возвращает имя текщего пользователя
void un(litera* f) {}
// #(us,n) делает текущим пользователем пользователя с номером n.
// Возвращает пустую строку.
void us(litera* f) {}
