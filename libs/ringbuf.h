//
// Вспомогательная библиотека: Циклический буфер
//
#ifndef __RINGBUF_H__
#define __RINGBUF_H__

class RingBuf {
public:
	RingBuf(int size);
	~RingBuf(void);
	void clear(void);
	inline int length(void) { return len; };

	void push(litera c);	// добавить в конец
	char pop(void);		// вытолкнуть из конца
	void put(litera c);	// добавить в начало
	char get(void);		// вытолкнуть из начала
	void dropfirst(void); // удалить первый символ

	litera peek(int num); // посмотреть символ с номером num

	int indexof(litera c); // возращает адрес первого вхождения
	int lastindexof(litera c); // возращает адрес последнего вхождения

	void push(const litera* s); // добавить в конец
	void put(const litera* s);	// добавить в начало

	litera* cutstart(int n); // отрезать и вернуть построку с начала длиной n
	litera* cutend(int n); // отрезать и вернуть построку сконца начиная с символа номер n
	litera* asstring(); // вынуть все из буфера в виде строки

private:
	litera* buf;
	int b;
	int e;
	int len;
	int size;
	void error(const char* s);
};

#endif // __LIB_H__
