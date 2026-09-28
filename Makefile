CC      = gcc
CFLAGS  = -Wall -Wextra -g -MMD -MP
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

-include $(OBJ:.o=.d)

clean:
	-$(RM) *.o *.d $(TARGET)

.PHONY: all clean
