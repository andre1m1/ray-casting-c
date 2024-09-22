CFLAGS = -Wall -Wextra -std=c11 -O3  
LINUX_LIBS = ./raylib/raylib-linux/libraylib.a -lm
WINDOWS_LIBS = -L./raylib/raylib-windows/lib -lraylib -lwinmm -lm -lopengl32 -lgdi32 -static  

all: linux windows

linux: main.c 
	clang -o ./build/main $(CFLAGS) -I./raylib/raylib-linux/ main.c $(LINUX_LIBS) 

windows: main.c
	x86_64-w64-mingw32-gcc -o ./build/main.exe $(CFLAGS) -I./raylib/raylib-windows/include main.c $(WINDOWS_LIBS) 
