#include <stdio.h>
#include <stdlib.h>
#include "raylib/raylib.h"

const int WIDTH     = 800;
const int HEIGHT    = 800;
const int FPS       = 60;

const int    GRID_SIZE = 8;
const int    RADIUS    = 10;
const double EPS       = 1e-4;


int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Hello Raylib");

    SetTargetFPS(FPS); 
    while(!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
