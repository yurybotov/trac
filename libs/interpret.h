#ifndef __INTERPRET_H__
#define __INTERPRET_H__

#if TARGET == LINUX
#include "../linux/projectcfg.h"
#endif

#include "litera.h"
#include "ringbuf.h"

struct form {
  uint16_t hash;
  litera* name;
  litera* value;
  int ptr;
  int css;
};

class Trac {
  public:
	Trac(void);
	~Trac(void);

  void run(void);        // начало интерпретации
	void doStep(void);     // выполнить один проход цикла интерпретации

  private:
	void execute(void);    // исполнитель функции
	void runstdtrac(int start); // вызов встроенной функции трак
#if EXTENDEDSYNTAX == TRUE
  void runexttrac(litera* first,litera* f); // вызов встроенной функции расширенного синтаксиса
#endif
	void runuser(litera* first,litera* f); // вызов пользовательской функции

	litera meta;      // символ разделитель ввода
  bool notstop;     // признак того что вычисления продолжаются
  bool trace;       // признак отладочной трассировки
  int radix;        // система счисления
  litera idle[MAXFILEPATH];  // код выполняемый пока нет ввода пользователя
  bool z;           // признак наличия ошибки в вызываемой функции
  char in[MAXFILEPATH];    // откуда берутся данные
  long inoffset;    // сдвиг в файле
  char out[MAXFILEPATH];   // куда они складываются

	RingBuf& I = *new RingBuf(IBUFSIZE);       // входной буфер
	RingBuf& A = *new RingBuf(ABUFSIZE);       // активный буфер
	RingBuf& N = *new RingBuf(NBUFSIZE);       // нейтральный буфер
  RingBuf& R = *new RingBuf(RBUFSIZE);       // буфер результата функции
	RingBuf& O = *new RingBuf(OBUFSIZE);       // выходной буфер

  struct form F[MAXFORMS]; // хранилище форм
  int formlength;         // количество форм в нем

  // встроенные функции trac
  #include "traclib.h"
};

litera* param(litera*, int);

#endif // __INTERPRET_H__
