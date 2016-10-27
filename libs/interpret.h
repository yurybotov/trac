#ifndef __INTERPRET_H__
#define __INTERPRET_H__

class Trac {
  public:
	Trac(void);
	~Trac(void);

        void run(void);
	void doStep(void);
	void feed(litera* s);
	char* out(void);
  private:
	void parse(void);
	void execute(void);
	void runstdtrac(void);
	void runuser(void);

	litera meta = (litera) '`';

	RingBuf& i;
	RingBuf& a;
	RingBuf& n;
	RingBuf& o;
};


#endif // __INTERPRET_H__