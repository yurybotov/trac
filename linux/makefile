TARGET = tracinterpret
PREFIX = /usr/local/bin
C = g++

.PHONY: all clean install uninstall

all: $(TARGET)
	
clean:
	rm -rf $(TARGET) *.o
main.o: main.cpp
	$(C) -c -o -std=c++11 main.o main.cpp
interpret.o: ../libs/interpret.cpp
	$(C) -c -o -std=c++11 interpret.o ../libs/interpret.cpp
ringbuf.o: ../libs/ringbuf.cpp
	$(C) -c -o -std=c++11 ringbuf.o ../libs/ringbuf.cpp
traclib.o: ../libs/traclib.cpp
	$(C) -c -o -std=c++11 traclib.o ../libs/traclib.cpp
$(TARGET): main.o interpret.o ringbuf.o traclib.o
	$(C) -o $(TARGET) main.o interpret.o ringbuf.o traclib.o
install:
	install $(TARGET) $(PREFIX)
uninstall:
	rm -rf $(PREFIX)/$(TARGET)
