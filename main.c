#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "raylib/raylib.h"
#include "raylib/raymath.h"
#include "math.h"

#define GRID_SIZE  8
#define FPS        60
#define WIDTH      800
#define HEIGHT     800
#define RADIUS     10.0f
#define EPS        1e-4


#define grid_at(grid, i, j) grid.items[i*grid.cols+j] 

typedef struct {
    int rows;
    int cols;
    bool *items;

} Grid;

int make_grid(Grid* grid, int rows, int cols) {
    grid->rows = rows;
    grid->cols = cols;
    grid->items = malloc(sizeof(bool)*rows*cols);
    
    if (grid->items == NULL) {
        return -1;
    }

    memset(grid->items, 0, sizeof(bool)*rows*cols);

    return 0;
}

void draw_grid(Grid grid)
{
    int cell_size = WIDTH / GRID_SIZE;
       
    DrawLineV((Vector2){.x = 0, .y = 1}, (Vector2){.x = WIDTH, .y = 1}, RAYWHITE);
    DrawLineV((Vector2){.x = 1, .y = 0}, (Vector2){.x = 1, .y = HEIGHT}, RAYWHITE);
    DrawLineV((Vector2){.x = WIDTH, .y = 0}, (Vector2){.x = WIDTH, .y = HEIGHT}, RAYWHITE);
    DrawLineV((Vector2){.x = 0, .y = HEIGHT}, (Vector2){.x = WIDTH, .y = HEIGHT}, RAYWHITE);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++){
            if (grid_at(grid, i, j) != 0) {
                DrawRectangleV((Vector2){i*cell_size, j*cell_size}, (Vector2){cell_size, cell_size}, RAYWHITE);
            }
        }

        DrawLineV((Vector2){.x = i*cell_size, .y = 0}, (Vector2){.x = i*cell_size, .y = HEIGHT},  RAYWHITE);
        DrawLineV((Vector2){.x = 0, .y = i*cell_size}, (Vector2){.x = WIDTH, .y = i*cell_size}, RAYWHITE);
    }

}

// TODO: Stop using global constants
Vector2 world_to_grid(Vector2 point)
{
    return (Vector2){ .x = point.x / WIDTH * GRID_SIZE, .y = point.y / HEIGHT * GRID_SIZE };
}

Vector2 grid_to_world(Vector2 point)
{
    return (Vector2){ .x = point.x * WIDTH / GRID_SIZE, .y = point.y * HEIGHT / GRID_SIZE };
}

void draw_line(Vector2 p1, Vector2 p2)
{
    Vector2 p1w = grid_to_world(p1);
    Vector2 p2w = grid_to_world(p2);
    DrawLineV(p1w, p2w, RED);
}

void draw_point(Vector2 p)
{
    Vector2 pw = grid_to_world(p);
    DrawCircleV(pw, RADIUS, RED);
}

Vector2 get_line_eq(Vector2 p1, Vector2 p2)
{
    float m = 0.0;
    if (p1.x != p2.x) m = (p2.y - p1.y) / (p2.x - p1.x);
    
    float n = p2.y - m * p2.x;

    return (Vector2){.x = m, .y = n};
}

// TODO: Refactor the way collisions are computed
Vector2 cast_ray(Vector2 p1, Vector2 p2)
{
    Vector2 line_eq = get_line_eq(p1, p2);
    Vector2 diff = Vector2Subtract(p2, p1);
    float m = line_eq.x;
    float n = line_eq.y;
    
    Vector2 dx = {0};
    Vector2 dy = {0};

    // Find closest X axis collision
    if (m) {
        if (diff.y > 0) dx = (Vector2){.x = (ceil(p2.y) - n) / m, .y = ceil(p2.y)};
        else dx = (Vector2){.x = (floorf(p2.y) - n) / m, .y = floorf(p2.y)};
    } else dx = (Vector2){.x = 0, .y = n};

    // Find Y axis collision
    if (diff.x > 0) dy = (Vector2){.x = ceil(p2.x), .y = m * ceil(p2.x) + n};
    else if (diff.x < 0) dy = (Vector2){.x = floorf(p2.x), .y = m * floorf(p2.x) + n};
    else {
        if (diff.y > 0) dy = (Vector2){.x = p2.x, .y = ceil(p2.y)};
        else dy = (Vector2){.x = p2.x, .y = floorf(p2.x)};
    }
    
    if (Vector2DistanceSqr(p2, dx) < Vector2DistanceSqr(p2, dy)) return dx;
    return dy;

}


int main(void)
{
    Grid g = {0};
    make_grid(&g, GRID_SIZE, GRID_SIZE); 
    grid_at(g, 2, 2) = 1;

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
        Vector2 mouse_pos = world_to_grid(GetMousePosition());
        Vector2 p = {1,1};
        BeginDrawing();
            ClearBackground(BLACK);
            draw_grid(g);
            draw_line(p, mouse_pos);
            draw_point(p);
            draw_point(mouse_pos);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
