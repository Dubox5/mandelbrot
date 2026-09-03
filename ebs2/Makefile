CC      = gcc
LOGIN  ?= login

CFLAGS  = -Wall -Wextra -O2 -std=c11 -pthread -DLOGIN=\"$(LOGIN)\"
OMPFLAG = -fopenmp
LDFLAGS = -lm

TARGET  = mandelbrot
SRC     = main.c common.c serial.c openmp.c pthreads1.c pthreads2.c
OBJ     = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OMPFLAG) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c mandelbrot.h
	$(CC) $(CFLAGS) $(OMPFLAG) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) mandelbrot_*.pgm times.txt
