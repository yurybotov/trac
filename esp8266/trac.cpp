#include <FS.h>
#include "trac.h"

const char NEXTPARAM = (char)17;
const char ACTIVEFUNC = (char)18;
const char NEYTRALFUNC = (char)19;
const char ENDFUNC = (char)20;
char* idle = "#(ps,#(rs))"; // interpret init string
char meta = '`'; // interpret meta
bool z = false; // function output type
bool runned = false;
char curIterm = 't'; // f
char curOterm = 't';
char curInet = 't'; // f
char curOnet = 't';
char* curItermfilename;
char* curOtermfilename;
char* curInetfilename;
char* curOnetfilename;
RingBuf& Ifile = *new RingBuf(512);
RingBuf& A = *new RingBuf(512);
RingBuf& N = *new RingBuf(512);
RingBuf& Ofile = *new RingBuf(512);
RingBuf& F = *new RingBuf(512);
bool isready = false; // input string ready in input buffer (meta exists)
struct form forms[255];
uint8_t formlength = 0;
struct user users[MAX_SRV_CLIENTS];
uint8_t curuser = 0;

char* loads() {
  char* s;
  int metaptr = users[curuser].I->indexof(meta);
  if(metaptr != -1) {
    s = users[curuser].I->cutstart(metaptr-1);
    users[curuser].I->dropfirst();
    return s;
  } else 
  return NULL;
}

char loadc() {
  return users[curuser].I->get();
}

char* formcall(char* fst, char* oth) {
   return NULL;
}

// вычисляет хэш для формы
int hash(char* s) { 
  int a = 0; 
  for (int i = 0; i < strlen(s); i++) { a = ((a << 3) ^ i) + (unsigned int)s[i]; } 
  return a; 
}

// ищет номер формы по имени
int findform(char* n) {
  int h = hash(n), res = -1;
  for(int i = 0; i < formlength; i++) {
    if( forms[i].owner == curuser && forms[i].hash == h && !strncmp(forms[i].name, n, 8)) {res = i; break;}
  }
  return res;
}

// добавляет форму
int formadd(char* fname, char* fform) {
  int ref;
  if (fname != NULL) {
    if((ref = findform(fname)) != -1) {
      delete forms[ref].value;
      forms[ref].hash = hash(fname);
      strncpy(forms[ref].name, fname, 8);
      forms[ref].value = fform;
      forms[ref].ptr = 0;
      forms[ref].css = 0;
      forms[ref].owner = curuser;
      // если форма с таким именем есть - заменяем ее
    } else {
      // если формы с таким именем нет - создаем новую      
      struct form* f = forms + formlength;
      f->hash = hash(fname);
      strncpy(f->name, fname, 8);
      f->value = fform;
      f->ptr = 0;
      f->css = 0;
      f->owner = curuser;
      formlength++;
    }
  }
}

// возвращает строку параметра с номером n
char* param(char* start, int n) {
  char *bgn = start, *en;
  for(int i = 0; i < n; i++) { bgn = strchr(bgn+1,NEXTPARAM); }
  if(bgn == NULL) return NULL;
  en = strchr(bgn+1,NEXTPARAM);
  if(en == NULL) return NULL;
  int len = (int)(en - bgn);
  char* o = new char[len];
  strncpy(o, bgn+1, len-1);
  o[len-1] = (char)0;
  return o;
}

// обработчик встроенных функций
void start(int fstart) {
  char *ptr, *ptr1, *ptr2, *ptr3, *ptr4;
  char* f = N.cutend(fstart);
  char* first = param(f,0);
  // общий ввод вывод
  // #(rs[,Z]) - читает строку из входного потока до первого встреченного метасимвола, метасимвол удаляется из потока, если на устройстве ввода ошибка - возвращает Z
  if(!strcmp(first,"rs")) { 
    ptr = loads(); 
    F.push(ptr); 
    if(ptr !=NULL) delete ptr; 
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(rc[,Z]) - читает один символ из потока, любой, включая метасимвол, если на устройстве ввода ошибка - возвращает Z
  if(!strcmp(first,"rc")) { 
    F.push(loadc()); 
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(gm) - возвращает текущий метасимвол
  if(!strcmp(first,"gm")) { 
    F.push(meta); 
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(cm,C) - заменяет системный метасимвол на символ С
  if(!strcmp(first,"cm")) { 
    ptr = param(f,1); 
    meta = ptr[0]; 
    if(ptr !=NULL) delete ptr; 
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(ps,S) - отправляет S на текущее устройство вывода
  if(!strcmp(first,"ps")) { 
    ptr = param(f,1); 
    users[curuser].O->push(ptr); 
    if(ptr !=NULL) delete ptr; 
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(rx,N,RI,RO) - преобразование основания системы счисления из текущей RI в RO { B, O, D, H }
  // TODO -----------------------------------------
  
  // работа с формами: пользовательские функции, массивы и т.п.
  // #(ds,N,F) - создает форму с именем N и значением F
  if(!strcmp(first,"ds")) { 
    ptr = param(f,1); ptr1 = param(f,2);
    formadd( ptr, ptr1);
    if(ptr != NULL) delete ptr; /*if(ptr1 != NULL) delete ptr1;*/
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(ss,N,d1[,d2...]) - нарезка формы N на сегменты ограниченные цепочками dx
  // TODO -----------------------------------------
//case 'ss': case 'разбить': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.formseg(n, ss); this.funcvalue = ''; return;
  // #(cl,N,d1[,d2...]) - вызов формы N как функцию с параметрами dx
  // TODO -----------------------------------------
//case 'cl': case 'вызвать': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.funcvalue = this.formcall(n, ss); return;
  // #(сs,N[,Z]) - вызов формы N как массив последовательно выдаваемых записей, при каждом обращении выдается одна "следующая" запись
  // TODO -----------------------------------------
//case 'cs': case 'сегмент': this.funcvalue = this.formcallseg(this.get(s, 1), this.get(s, 2)); return;
  // #(сс,N[,Z]) - вызов формы N как массив последовательно выдаваемых символов, при каждом обращении выдается один "следующий" символ
  // TODO -----------------------------------------
//case 'cc': case 'символ': this.funcvalue = this.formcallchr(this.get(s, 1), this.get(s, 2)); return;
  // #(сn,N,D[,Z]) - вызов формы N как массив последовательно выдаваемых полей равной длины D, при каждом обращении выдается D символов, при отрицательном D выдаются символы слева от маркера
  // TODO -----------------------------------------
//case 'cn': case 'символы': this.funcvalue = this.formcallchrs(this.get(s, 1), this.get(s, 2), this.get(s, 3)); return;
  // #(fd,N,S[,Z]) - перемещение внутреннего указателя в форме N на начало найденной в форме подстроки S
  if(!strcmp(first,"fd")) { 
    ptr = param(f,1); ptr1 = param(f,2);
    int ref = findform(ptr);
    if(ref != -1) { 
      char* substr = strstr(forms[ref].value, ptr1);
      if(substr != NULL) { forms[ref].ptr = (int)(substr - forms[ref].value); }
    }
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(сr,N) - обнуление внутреннего указателя в форме N
  if(!strcmp(first,"cr")) { 
    ptr = param(f,1);
    int ref = findform(ptr);
    if(ref != -1) { forms[ref].ptr = 0; }
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(dd,N) - удаление формы N
  if(!strcmp(first,"dd")) { 
    ptr = param(f,1);
    int ref = findform(ptr);
    if(ref != -1) {
      delete forms[ref].value;
      for(int i = ref; i < (formlength-1); i++) {
        forms[i].hash = forms[i+1].hash;
        strncpy(forms[i].name, forms[i+1].name, 8);
        forms[i].value = forms[i+1].value;
        forms[i].ptr = forms[i+1].ptr;
        forms[i].css = forms[i+1].css;
      }
      formlength--;
    }
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
  // #(da) - удаление всех форм 
  if(!strcmp(first,"da")) { 
    for(int i = 0; i < formlength; i++) {
      delete forms[i].value;///////////////////////////////////////////////////////////////////
    }
    formlength = 0;
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }

 // арифметика
 // #(ad,D1,D2) - воспринимая D1 и D2 как десятичные числа складывает их и возвращает результат в виде строки
  if(!strcmp(first,"ad") || !strcmp(first,"+")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1+a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
 // #(su,D1,D2) - воспринимая D1 и D2 как десятичные числа вычитает D2 из D1 и возвращает результат в виде строки
  if(!strcmp(first,"su") || !strcmp(first,"-")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1-a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
 // #(ml,D1,D2) - воспринимая D1 и D2 как десятичные числа перемножает их и возвращает результат в виде строки
  if(!strcmp(first,"ml") || !strcmp(first,"*")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1*a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }

 // #(dv,D1,D2[,Z]) - воспринимая D1 и D2 как десятичные числа делит D1 на D2 и возвращает результат в виде строки, если D2 == 0 вместо результата возвращает Z
  if(!strcmp(first,"dv") || !strcmp(first,"/")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10);
    if (a2 == 0) {
      ptr2 = param(f,3);
      F.push(ptr2);
      if(ptr != NULL) delete ptr; if(ptr1 != NULL) delete ptr1; if(ptr2 != NULL) delete ptr2;
      if( f != NULL) delete f; if( first != NULL) delete first; return;
    }
    char* o = new char[34]; 
    ltoa(a1/a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }

// битовые операции
  if(!strcmp(first,"an") || !strcmp(first,"&")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1&a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
  if(!strcmp(first,"or") || !strcmp(first,"|")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1|a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
  if(!strcmp(first,"xr") || !strcmp(first,"^")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1^a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
  if(!strcmp(first,"no") || !strcmp(first,"!")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    char* o = new char[34]; 
    ltoa(!a1,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
    if(!strcmp(first,"bs") || !strcmp(first,"<<")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1<<a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }
    if(!strcmp(first,"br") || !strcmp(first,">>")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    int a1 = strtol(ptr,NULL,10); 
    int a2 = strtol(ptr1,NULL,10); 
    char* o = new char[34]; 
    ltoa(a1>>a2,o,10);
    F.push(o); 
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    if(o != NULL) delete o; return;
  }

  // управление выполнением
  // #(eq,S1,S2,V1,V2) - условный оператор. Если S1 == S2 то выполняется выражение V1 иначе V2
  if(!strcmp(first,"eq") || !strcmp(first,"=")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    if (strcmp(ptr, ptr1) == 0) {
      ptr2 = param(f,3);
    } else {
      ptr2 = param(f,4);
    }
    F.push(ptr2);
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1; if(ptr2 != NULL) delete ptr2;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    return;
  }
  // #(gr,S1,S2,V1,V2) - условный оператор. Если S1 > S2 то выполняется выражение V1 иначе V2
  if(!strcmp(first,"gr") || !strcmp(first,">")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    if (strcmp(ptr, ptr1) > 0) {
      ptr2 = param(f,3);
    } else {
      ptr2 = param(f,4);
    }
    F.push(ptr2);
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1; if(ptr2 != NULL) delete ptr2;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    return;
  }
  // #(lt,S1,S2,V1,V2) - условный оператор. Если S1 < S2 то выполняется выражение V1 иначе V2
  if(!strcmp(first,"lt") || !strcmp(first,"<")) { 
    ptr = param(f,1); ptr1 = param(f,2); 
    if (strcmp(ptr, ptr1) < 0) {
      ptr2 = param(f,3);
    } else {
      ptr2 = param(f,4);
    }
    F.push(ptr2);
    if(ptr != NULL) delete ptr;  if(ptr1 != NULL) delete ptr1; if(ptr2 != NULL) delete ptr2;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    return;
  }
  // #(hl) - остановить программу, по сути рестартует интерпретатор
  if(!strcmp(first,"hl")) { 
    N.clear(); 
    A.push(idle); 
    z = false; 
    if( f != NULL) delete f; if( first != NULL) delete first;  return;
  }
  // #(rt) - жестко рестартует весь контроллер
  if(!strcmp(first,"rt")) { 
    ESP.reset();
  }
  // #(np,S)- нет операции, можно использовать для комментирования кода S
  if(!strcmp(first,"np")) { 
     if( f != NULL) delete f;  if( first != NULL) delete first; return;
  }
  
 // интерфейс с внешним хранилищем форм
 // #(sb,N,n1[,n2...]) - сохранение форм n1.. в блок на диске с именем N
 // TODO -----------------------------------------
//case 'sb': case 'сохранить': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.blockstore(n, ss); this.funcvalue = ''; return;
 // #(fb,N) - загрузить все формы из блока на диске с именем N
 // TODO -----------------------------------------
//case 'fb': case 'загрузить': this.blockfetch(this.get(s, 1)); this.funcvalue = ''; return;
 // #(eb,N) - удалить блок на диске с именем N
 // TODO -----------------------------------------
//case 'eb': case 'удалить': this.blockerase(this.get(s, 1)); this.funcvalue = ''; return;
 // перенаправление ввода вывода в файл
 // #(si[,N]) - подключить подсистему ввода к файлу с именем N, если #(si) - стандартный ввод из терминала
 // TODO -----------------------------------------
 // #(so[,N]) - подключить подсистему вывода к файлу с именем N, если #(so) - стандартный вывод на терминал
 // TODO -----------------------------------------
 
  // пользователи
  // #(us,n) - сделать текущим пользователем пользователя с номером n
  if(!strcmp(first,"us")) { 
    ptr = param(f,1); 
    curuser = strtol(ptr,NULL,10); 
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; 
    return;
  }
 // #(un) - вернуть имя текщего пользователя
  if(!strcmp(first,"un")) { 
    F.push(users[curuser].name);
    if( f != NULL) delete f; if( first != NULL) delete first; 
    return;
  }
 
 // отладка
 // #(tr) - начать трассировку
 // TODO -----------------------------------------
 // #(tf) - закончить трассировку
 // TODO -----------------------------------------
 // #(nl[,S]) - возвращает список имен имеющихся форм, S - делимитер между именами форм
  if(!strcmp(first,"nl")) { 
    ptr = param(f,1);
    char dl = (ptr == NULL)? ' ' : ptr[0];
    for(int i = 0; i < formlength; i++) {
      F.push(forms[i].name);
      F.push(dl);
    }
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }
 // #(pb,S) - возвращает содержимое формы с именем S
  if(!strcmp(first,"pb")) { 
    ptr = param(f,1);
    int ref = findform(ptr);
    if(ref != -1) { F.push(forms[ref].value); }
    if(ptr != NULL) delete ptr;
    if( f != NULL) delete f; if( first != NULL) delete first; return;
  }

  // если имя не стандартное - будем считать что оно имя функции из формы
  ptr = param(f,0); ptr1 = param(f,1);
  ptr2 = formcall(ptr, ptr1);
  F.push(ptr2); 
  if(ptr !=NULL) delete ptr; if(ptr1 !=NULL) delete ptr1; if(ptr2 !=NULL) delete ptr2;
  if( f != NULL) delete f; if( first != NULL) delete first; return;
}

/*

                // экстра
            case 'sl': case 'длинастроки': break;
            case 'vl': case 'значениесимвола': break;
            case 'as': case 'длинамассива': break;
            case 'rx': case 'основание': break;
                // file io
            case 'si': case 'входфайл': break;
            case 'so': case 'выходфайл': break;
            case 'rp': case 'переместитьуказатель': break;
            case 'gp': case 'значениеуказателя': break;

*/

// отработка вызова функции
void gofunc() {
  char* ptr;
  // ищем начало функции в нейтральной цепочке
  int af = N.lastindexof(ACTIVEFUNC);
  int nf = N.lastindexof(NEYTRALFUNC);
  int fstart = (af > nf) ? af : nf;
  if(fstart > -1) {
    // запускаем стандартный обработчик функций
    start( fstart);
    // помещение результата функции если он не пустой 
    if( F.length() > 0) {
      ptr = F.asstring();
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
}

// поиск парной скобки
int searchparent(void) {
  int cnt = 0;
  for (int i = 0; i < A.length(); i++) {
    if (A.peek(i) == '(') { cnt++; } else {
      if (A.peek(i) == ')') {
        if (cnt == 0) { return i+1; } else { cnt--; }
      }
    }
  }
  return -1;
}

// синтаксический разбор строки
void interpret() {
  char* ptr;
  // если активная строка пуста - рестарт
  if (A.length() == 0) {
    N.clear();
    A.push(idle);
    z = false;
  } else {
    //  если ( - ищем закрывающую скобку и все это вместе уносим в нейтральную цепочку (без скобок)
    if (A.peek(0) == '(') {
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
      if (A.peek(0) == '\r' || A.peek(0) == '\n' || A.peek(0) == '\t') {
         A.dropfirst();
      }
      else {
        // если запятая вставляем признак параметра
        if (A.peek(0) == ',') {
           A.dropfirst();
           N.push(NEXTPARAM);
        }
        else {
          // если ##( - вставляем признак нейтральной функции
          if (A.peek(0) == '#' && A.peek(1) == '#' && A.peek(2) == '(') {
            A.dropfirst();A.dropfirst();A.dropfirst();
            N.push(NEYTRALFUNC);
          }
          else {
            // если #( - вставляем признак активной функции
            if (A.peek(0) == '#' && A.peek(1) == '(') {
              A.dropfirst();A.dropfirst();
              N.push(ACTIVEFUNC);
            }
            else {
              // если ) - зпускаем функцию на выполнение
              if (A.peek(0) == ')') {
                A.dropfirst();
                N.push(NEXTPARAM);
                N.push(ENDFUNC);
                //Serial.println(N.asstring());
                gofunc(); // вызов функции
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
