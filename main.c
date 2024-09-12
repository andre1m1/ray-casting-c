#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "raylib/raylib.h"

#define grid_at(grid, i, j) grid.items[i*grid.cols+j] 

const int WIDTH     = 800;
const int HEIGHT    = 800;
const int FPS       = 60;

const int    GRID_SIZE = 8;
const int    RADIUS    = 10;
const double EPS       = 1e-4;


typedef struct {
    int rows;
    int cols;
    int *items;

} Grid;

int make_grid(Grid* grid, int rows, int cols) {
    grid->rows = rows;
    grid->cols = cols;
    grid->items = malloc(sizeof(int)*rows*cols);
    
    if (grid->items == NULL) {
        return -1;
    }

    memset(grid->items, 0, sizeof(int)*rows*cols);

    return 0;
}

void draw_grid(Grid grid)
{
    int cell_size = WIDTH / GRID_SIZE;
       
    DrawLineV((Vector2){.x = 0, .y = 1}, (Vector2){.x = WIDTH, .y = 1}, RAYWHITE);
    DrawLineV((Vector2){.x = 1, .y = 0}, (Vector2){.x = 1, .y = HEIGHT}, RAYWHITE);
    DrawLineV((Vector2){.x = WIDTH, .y = 0}, (Vector2){.x = WIDTH, .y = HEIGHT}, RAYWHITE);
    for (int i = 1; i <= GRID_SIZE; i++) {
        DrawLineV((Vector2){.x = i*cell_size, .y = 0}, (Vector2){.x = i*cell_size, .y = HEIGHT},  RAYWHITE);
        DrawLineV((Vector2){.x = 0, .y = i*cell_size}, (Vector2){.x = WIDTH, .y = i*cell_size}, RAYWHITE);
    }

}

int main(void)
{
    Grid g = {0};
    make_grid(&g, GRID_SIZE, GRID_SIZE); 

    for (int i = 0; i < g.rows; i++){ 
        for (int j = 0; j < g.cols; j++) {
            printf("%d ", grid_at(g, i, j));
        }
        printf("\n");
    }


    InitWindow(WIDTH, HEIGHT, "Hello Raylib");
    SetTargetFPS(FPS); 
        while(!WindowShouldClose()) 
    {
        BeginDrawing();
            ClearBackground(BLACK);
            draw_grid(g);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
