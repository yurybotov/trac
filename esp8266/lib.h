//
// Вспомогательная библиотека: Циклический буфер
//


#ifndef __LIB_H__
#define __LIB_H__

class RingBuf {
public:
	RingBuf(int size);
	~RingBuf(void);
	void clear(void);
	inline int length(void) { return len; };
	
	void push(char c);	// добавить в конец
	char pop(void);		// вытолкнуть из конца
	void put(char c);	// добавить в начало
	char get(void);		// вытолкнуть из начала
  void dropfirst(void); // удалить первый символ

	char peek(int num); // посмотреть символ с номером num

	int indexof(char c); // возращает адрес первого вхождения
	int lastindexof(char c); // возращает адрес последнего вхождения

	void push(char* s); // добавить в конец
	void put(char* s);	// добавить в начало

	char* cutstart(int n); // отрезать и вернуть построку с начала длиной n
	char* cutend(int n); // отрезать и вернуть построку сконца начиная с символа номер n
  char* asstring(); // вынуть все из буфера в виде строки

//#ifdef __DBG__
//	void dbgprint(void);
//#endif
private:
	char* buf;
	int b;
	int e;
	int len;
	int size;
	//void (*overflow)();
	//void (*underflow)();
	void error(char* s);
};

#endif // __LIB_H__
