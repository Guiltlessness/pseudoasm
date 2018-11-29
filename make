CC = gcc
PATH_EXECUTOR = ./executor/
PATH_OBJDUMP = ./objdump/
PATH_COMPILER = ./asmcompiler/
TARGET_EXECUTOR = main
TARGET = main
HEADER = excutor.h
CFLAGS = -g -Wall

all:  $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	rm -f $(TARGET)