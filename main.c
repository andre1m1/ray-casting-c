#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "raylib/raylib.h"
#include "raylib/raymath.h"

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

void draw_point(Vector2 p, Color color)
{
    Vector2 pw = grid_to_world(p);
    DrawCircleV(pw, RADIUS, color);
}

Vector2 get_line_eq(Vector2 p1, Vector2 p2)
{
    float m = 0.0f;
    if (p1.x != p2.x) m = (p2.y - p1.y) / (p2.x - p1.x);
    
    float n = p2.y - m * p2.x;

    return (Vector2){.x = m, .y = n};
}

// TODO: Refactor the way collisions are computed
Vector2 step_ray(Vector2 p1, Vector2 p2)
{   
    Vector2 line_eq = get_line_eq(p1, p2);
    float m = line_eq.x;
    float n = line_eq.y;
    
    Vector2 dx = {0};
    Vector2 dy = {0};

    // Find closest X axis collision
    if (m) {
        if (p2.y > p1.y) dx = (Vector2){.x = (ceil(p2.y) - n) / m, .y = ceil(p2.y)};
        else dx = (Vector2){.x = (floorf(p2.y) - n) / m, .y = floorf(p2.y)};
    } else dx = (Vector2){.x = 0, .y = n};

    // Find Y axis collision
    if (p2.x > p1.x) dy = (Vector2){.x = ceil(p2.x), .y = m * ceil(p2.x) + n};
    else if (p2.x < p1.x) dy = (Vector2){.x = floorf(p2.x), .y = m * floorf(p2.x) + n};
    else {
        if (p2.y > p1.y) dy = (Vector2){.x = p2.x, .y = ceil(p2.y)};
        else dy = (Vector2){.x = p2.x, .y = floorf(p2.x)};
    }
    
    if (Vector2DistanceSqr(p2, dx) < Vector2DistanceSqr(p2, dy)) return dx;
    return dy;

}

bool check_collision(Vector2 p, Vector2 dir, Grid grid)
{

    if (p.x < GRID_SIZE && p.y < GRID_SIZE) {
        if (p.x == (int)p.x) {
            if (dir.y > 0 && grid_at(grid, (int)p.x - 1, (int)floorf(p.y)) != 0) return 1;
            else if (dir.y < 0 && grid_at(grid, (int)p.x, (int)floorf(p.y)) != 0) return 1;
        }
        else {
            if (dir.x > 0 && grid_at(grid, (int)floorf(p.x), (int)p.y) != 0) return 1;
            else if (dir.x < 0 && grid_at(grid, (int)floorf(p.x), (int)p.y) != 0) return 1;
        }

    }
    return 0;

}

Vector2 get_plane(Vector2 dir)
{
    cos_45 = cos(PI/4.0f);
    float x = cos_45 * dir.x - cos_45 * dir.y;
    float y = cos_45 * dir.x + cos_45 * dir.y;
    return (Vector2) {x, y};
}


int main(void)
{
    Grid g = {0};
    make_grid(&g, GRID_SIZE, GRID_SIZE); 
    grid_at(g, 2, 2) = 1;
    grid_at(g, 1, 2) = 1;

    for (int i = 0; i < g.rows; i++){ 
        for (int j = 0; j < g.cols; j++) {
            printf("%d ", grid_at(g, i, j));
        }
        printf("\n");
    }


    InitWindow(WIDTH, HEIGHT, "Hello Raylib");
    SetTargetFPS(FPS); 

    // TODO: Fix bug where direction cannot be zero anymore
    Vector2 dir = {-EPS * 100, EPS * 100};
    Vector2 pos = {4, 5.6};
    Vector2 plane = get_plane(dir);

    while(!WindowShouldClose()) 
    {
        Vector2 start = pos;
        Vector2 eps = {.x = (dir.x / fabsf(dir.x)) * EPS, .y = (dir.y / fabsf(dir.y)) * EPS};
        if (IsKeyPressed(KEY_D))
        {
            
            float x = dir.x;
            dir.x = -dir.y;
            dir.y = x;
        }

        else if (IsKeyPressed(KEY_W))
        {
            pos = Vector2Add(pos, Vector2Scale(dir, 100));
        
        }

        BeginDrawing();
            ClearBackground(BLACK);
            draw_grid(g);
            while(Vector2DistanceSqr(start, pos) < MAX_DIST*MAX_DIST)
            {
                draw_point(start, RED);
                Vector2 next = step_ray(start, Vector2Add(start, dir));
                next = Vector2Add(next, eps);
                draw_line(start, next);
                draw_point(next, RED);
                if (check_collision(next, dir, g)) break;
                
                start = next;
            }
            draw_line(pos, Vector2Add(plane, Vector2Add(dir,pos)));
            draw_line(pos, Vector2Subtract(Vector2Add(dir,pos), plane));

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
