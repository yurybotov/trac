#ifndef __INTERPRET_H__
#define __INTERPRET_H__

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
  litera* idle;     // код выполняемый пока нет ввода пользователя
  bool z;           // признак наличия ошибки в вызываемой функции
  char* in[MAXFILEPATH];    // откуда берутся данные
  long inoffset;    // сдвиг в файле
  char* out[MAXFILEPATH];   // куда они складываются

	RingBuf& I;       // входной буфер
	RingBuf& A;       // активный буфер
	RingBuf& N;       // нейтральный буфер
  RingBuf& R;       // буфер результата функции
	RingBuf& O;       // выходной буфер

  struct form F[MAXFORMS]; // хранилище форм
  int formlength;         // количество форм в нем

  #include "traclib.h"    // встроенные функции trac
};

litera* param(litera*, int);

#endif // __INTERPRET_H__
