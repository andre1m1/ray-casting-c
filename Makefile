CFLAGS = -Wall -Wextra -std=c11 -O3 
LIBS = ./raylib/libraylib.a -lm
main: main.c ./raylib/libraylib.a ./raylib/raylib.h

	clang -o main $(CFLAGS) main.c $(LIBS) 
