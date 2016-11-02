TARGET = tracinterpret
PREFIX = /usr/local/bin
C = g++

.PHONY: all clean install uninstall

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o
main.o: main.cpp
	$(C) -std=c++11 -c -o main.o main.cpp
interpret.o: ../libs/interpret.cpp
	$(C) -std=c++11 -c -o interpret.o ../libs/interpret.cpp
ringbuf.o: ../libs/ringbuf.cpp
	$(C) -std=c++11 -c -o ringbuf.o ../libs/ringbuf.cpp
litera.o: ../libs/litera.cpp
	$(C) -std=c++11 -c -o litera.o ../libs/litera.cpp
traclib.o: ../libs/traclib.cpp
	$(C) -std=c++11 -c -o traclib.o ../libs/traclib.cpp
$(TARGET): main.o interpret.o ringbuf.o traclib.o litera.o
	$(C) -o $(TARGET) main.o interpret.o ringbuf.o traclib.o litera.o
install:
	install $(TARGET) $(PREFIX)
uninstall:
	rm -rf $(PREFIX)/$(TARGET)
