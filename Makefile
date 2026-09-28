CC      = gcc
CFLAGS  = -Wall -Wextra -g
SRC     = $(wildcard *.c)
OBJ     = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
  TARGET = hospital.exe
  RM     = del /Q /F
else
  TARGET = hospital
  RM     = rm -f
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) *.o $(TARGET)

.PHONY: all clean
