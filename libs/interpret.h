#ifndef __INTERPRET_H__
#define __INTERPRET_H__

struct form {
  uint16_t hash;
  litera* name;
  litera* value;
  int ptr;
  int css;
  uint8_t owner;
};

class Trac {
  public:
	Trac(void);
	~Trac(void);

  void run(void);        // начало интерпретации
	void doStep(void);     // выполнить один проход цикла интерпретации
	void feed(litera* s);  // ввод
	char* out(void);       // вывод

  private:
	void parse(void);      // парсер входного потока
	void execute(void);    // исполнитель функции
	void runstdtrac(int start); // вызов встроенной функции трак
#ifdef EXTENDEDSYNTAX
  void runexttrac(litera* first,litera* f); // вызов встроенной функции расширенного синтаксиса
#endif
	void runuser(litera* first,litera* f); // вызов пользовательской функции

	litera meta;      // символ разделитель ввода
  litera* idle;     // код выполняемый пока нет ввода пользователя
  bool z;           // признак наличия ошибки в вызываемой функции

	RingBuf& I;       // входной буфер
	RingBuf& A;       // активный буфер
	RingBuf& N;       // нейтральный буфер
  RingBuf& R;       // буфер результата функции
	RingBuf& O;       // выходной буфер

  struct form F[MAXFORMS]; // хранилище форм
};

#endif // __INTERPRET_H__
