CC = gcc
CFLAGS = -Wall

TARGET = simulator.exe

SRC = main.c compiler.c processor.c memory.c
OBJ = main.o compiler.o processor.o memory.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c compiler.h processor.h memory.h
	$(CC) $(CFLAGS) -c main.c

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c

processor.o: processor.c processor.h memory.h
	$(CC) $(CFLAGS) -c processor.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

compile: $(TARGET)
	$(TARGET)

run: $(TARGET)
	@if "$(PROGRAM)"=="" (echo ERROR: Please specify PROGRAM=name && exit /b 1)
	copy /Y programs\$(PROGRAM).txt program.txt
	copy /Y data\$(PROGRAM).byte data.byte
	$(TARGET)
clean:
	del /Q *.o
	del /Q $(TARGET)
	del /Q program.byte