
TARGET = tracinterpret
PREFIX = /usr/local/bin

.PHONY: all clean install uninstall

all: $(TARGET)
	
clean:
			rm -rf $(TARGET) *.o
main.o: main.c
			gcc -c -o main.o main.c
interpret.o: interpret.c
			gcc -c -o interpret.o ../libs/interpret.c
ringbuf.o: ringbuf.c
			gcc -c -o ringbuf.o ../libs/ringbuf.c
traclib.o: traclib.c
			gcc -c -o traclib.o ../libs/traclib.c
$(TARGET): main.o interpret.o ringbuf.o
			gcc -o $(TARGET) main.o interpret.o ringbuf.o
install:
			install $(TARGET) $(PREFIX)
uninstall:
			rm -rf $(PREFIX)/$(TARGET)