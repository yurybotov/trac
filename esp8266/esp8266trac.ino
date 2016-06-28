/*
  Trac interpreter

 "Железное" исполнение интерпретатора Trac

 Железо: модуль ESP8266 (ESP-12E)с минимальной обвязкой.
 Например NodeMCU 0.9, 1.0, WeMos D1 и другие


 создано 25 Июня 2016
 последнее изменение 28 Июня 2016
 Юрий Ботов

 Код - public domain.

 */

#include <ArduinoJson.h>
#include <FS.h>
#include "lib.h"

struct form {
  int hash;
  char name[8];
  char* value;
  int ptr;
  int css;
};

const char NEXTPARAM = (char)17;
const char ACTIVEFUNC = (char)18;
const char NEYTRALFUNC = (char)19;
const char ENDFUNC = (char)20;
char* idle = "#(ps,#(rs))"; // interpret init string
char meta = '`'; // interpret meta
bool z = false; // function output type
bool runned = false;
RingBuf& I = *new RingBuf(2048);
RingBuf& A = *new RingBuf(2048);
RingBuf& N = *new RingBuf(2048);
RingBuf& O = *new RingBuf(2048);
RingBuf& F = *new RingBuf(2048);
bool isready = false; // input string ready in input buffer (meta exists)
struct form forms[255];
int formlength = 0;
/*
bool loadConfig() {
  File configFile = SPIFFS.open("/settings.json", "r");
  if (!configFile) {
    // make new config
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["idle"] = "#(ps,#(rs))";
    json["meta"] = '`';
    //json["isize"] = 2048;
    //json["asize"] = 2048;
    //json["nsize"] = 2048;
    //json["osize"] = 2048;
  
    File configFile = SPIFFS.open("/settings.json", "w");
    if (!configFile) {
      Serial.println("Failed to create config file");
      return false;
    } 
    json.printTo(configFile);
  }
  Serial.print("settings.json exists. Size is: ");
  size_t size = configFile.size();
  Serial.print(size); Serial.print(" bytes...");

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  Serial.print("loaded...");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("\nFailed to parse config file");
    return false;
  } else Serial.print("parsed...");
  Serial.print("\n");
  idle = json["idle"]; Serial.print("idle = "); Serial.println(idle); 
  meta = json["meta"]; Serial.print("meta = "); Serial.println(meta); 
  //isize = json["isize"]; Serial.print("isize = "); Serial.println(isize); 
  //asize = json["asize"]; Serial.print("asize = "); Serial.println(asize); 
  //nsize = json["nsize"]; Serial.print("nsize = "); Serial.println(nsize); 
  //osize = json["osize"]; Serial.print("osize = "); Serial.println(osize); 
  runned = true;
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["idle"] = idle;
  json["meta"] = meta;
  //json["isize"] = isize;
  //json["asize"] = asize;
  //json["nsize"] = nsize;
  //json["osize"] = osize;

  File configFile = SPIFFS.open("/settings.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
*/
// system setup
void setup() {
  // init Serial
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    delay(100);
  }
  Serial.println("\n\n\n\nESP8266 Trac interpreter");
  Serial.println("\nInit Serial as 115200 baud");
  // init SD and FS
  Serial.print("Initializing SPIFFS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  //if(SPIFFS.format()) { Serial.println("SPIFFS formatted"); } else { Serial.println("SPIFFS format error"); return;}
  struct FSInfo i;
  SPIFFS.info(i);
  delay(1);
  Serial.println("");  Serial.print(i.totalBytes);  Serial.println(": total bytes in SPIFFS");
  Serial.print(i.usedBytes); Serial.println(": used bytes in SPIFFS");
  //if(loadConfig()) Serial.println("Configuration loaded");
  A.push(idle);
  z = false;
  Serial.print("\nTRAC:"); Serial.print(ESP.getFreeHeap());Serial.print(":>");
}

void dbg() {
  Serial.println("---------------------------------------------------------------------------------------------------------------");
  //Serial.print("i : "); Serial.println(inbuf);
  //Serial.print("a : "); Serial.println(a);
  //Serial.print("n : "); Serial.println(n);
  //Serial.print("o : "); Serial.println(outbuf);
  Serial.println("---------------------------------------------------------------------------------------------------------------");
}

char* loads() {
  char* s;
  int metaptr = I.indexof(meta);
  if(metaptr != -1) {
    if(metaptr != I.length()) {
      s = I.cutstart(metaptr-1);
      I.dropfirst();
    } else {
      s = I.asstring();
    }
    return s;
  } else 
  return NULL;
}

char loadc() {
  return I.get();
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
    if( forms[i].hash == h && !strncmp(forms[i].name, n, 8)) {res = i; break;}
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
      // если форма с таким именем есть - заменяем ее
    } else {
      // если формы с таким именем нет - создаем новую      
      struct form* f = forms + formlength;
      f->hash = hash(fname);
      strncpy(f->name, fname, 8);
      f->value = fform;
      f->ptr = 0;
      f->css = 0;
      formlength++;
    }
  }
}

/*
char* firstParam(char* s) {
  int sl = strlen(s);
  int l = -1;
  for(int i = 0; i < sl; i++) {
    if(s[i]==NEXTPARAM) {l = i; break;}
  }
  if(l == sl) {
    return s;
  } else {
    char* o = new char(l+1);
    int i;
    for(i = 0; i < l; i++) {
      o[i] = s[i];
    }
    o[i] = (char)0;
    return o;
  }
} 

char* otherParams(char* s) { 
  int sl = strlen(s);
  int l = -1;
  for(int i = 0; i < sl; i++) {
    if(s[i]==NEXTPARAM) {l = i; break;}
  }
  if(l == sl) {
    return "";
  } else {
    char* o = new char(sl-l+1);
    int i;
    for(i = 0; i < sl-l; i++) {
      o[i] = s[i+l];
    }
    o[i] = (char)0;
    return o;
  }
}
*/
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
    O.push(ptr); 
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
      delete forms[i].value;
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

// main arduino loop
void loop() {
  char* ptr;
  // проверяем состояние интерпретатора: работает или ожидает входных данных
  if(runned) {
    // если работает
    // освободить выходной буфер выдав его содержимое на устройство вывода
    if(O.length() > 0) {
      ptr = O.asstring();
      Serial.print(ptr);
      if(ptr != NULL) delete ptr;
    }
    // если что то есть на устройстве ввода - поместить это во входной буфер
    if(Serial.available()) {
      while (Serial.available()) {
        I.push((char)Serial.read());   
      }
    }
    // если в активной цепочке ничего нет, а во входном буфере нет заключительной литеры команды (meta)
    if(A.length() == 0 && I.indexof(meta) == -1) {
      // переходим в режим ожидания
      Serial.print("\nTRAC:"); Serial.print(ESP.getFreeHeap());Serial.print(":>");
      runned = false;
    } else {
      // иначе интерпретируем дальше
      interpret();
    }
  } else {  
    // в режиме ожидания
    // если что то есть на устройстве ввода - поместить это во входной буфер 
    if(Serial.available()) {
      while (Serial.available()) {
        I.push((char)Serial.read());
      }       
    }
    // если во входном буфере появился символ meta - переходим в рабочий режим
    if(I.indexof(meta) >= 0) {
      N.clear();
      A.push(idle);
      z = false;
      runned = true;
      Serial.print("\n");
    }
  }
}

void Error(char* s){
  Serial.print("Error: "); Serial.println(s);
} 


