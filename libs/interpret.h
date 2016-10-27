#ifndef __INTERPRET_H__
#define __INTERPRET_H__

class Trac {
  public:
	Trac(void);
	~Trac(void);

        void init(void);
        void run(void);
	void doStep(void);
	void feed(char* s);
	char* out(void);
  private:
	void parse(void);
	void execute(void);
	void runstdtrac(void);
	void runuser(void);
};


#endif // __INTERPRET_H__