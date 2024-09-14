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
#define MAX_DIST   10

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

bool check_collision(Vector2 p, Vector2 dir, Grid grid)
{
    if (p.x < GRID_SIZE && p.y < GRID_SIZE) {
        if (p.x == (int)p.x) {
            if (dir.x == -1 && grid_at(grid, (int)p.x - 1, (int)floorf(p.y)) != 0) return 1;
            else if (dir.x == 1 && grid_at(grid, (int)p.x, (int)floorf(p.y)) != 0) return 1;
        }
        else {
            if (dir.y == -1 && grid_at(grid, (int)floorf(p.x), (int)p.y - 1) != 0) return 1;
            else if (dir.y == 1 && grid_at(grid, (int)floorf(p.x), (int)p.y) != 0) return 1;
        }

    }
    return 0;

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
        Vector2 p1 = {1,1};
        Vector2 p2 = world_to_grid(GetMousePosition());
        BeginDrawing();
            ClearBackground(BLACK);
            draw_grid(g);
            draw_line(p1, p2);
            draw_point(p1);
            draw_point(p2);
                
            
            while(Vector2DistanceSqr(p1, p2) < MAX_DIST*MAX_DIST)
            {
                draw_line(p1, p2);
                Vector2 p3 = cast_ray(p1, p2);
                Vector2 eps = Vector2Subtract(p2, p1);
                
                eps.x /= fabsf(eps.x);
                eps.y /= fabsf(eps.y);

                if (check_collision(p3, eps, g)) {
                    draw_line(p2, p3);
                    draw_point(p3);
                    break;
                }
                    
                p1 = p2;
                p2 = Vector2Add(p3,Vector2Scale(eps, EPS));
                draw_point(p3);
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
