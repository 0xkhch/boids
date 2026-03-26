#include <stdio.h>
#include "raylib.h"
#include <stdlib.h> 
#include <math.h>
#include <time.h>

#define WIDTH  1000
#define HEIGHT 1000
#define V 150

#define BOIDS_LEN 128
#define BOID_SIZE 3
#define BOID_RADIUS 50
#define COLLISION_RADIUS 25

#define COHESION_FACTOR 0.2
#define SEPARATION_FACTOR 0.5
#define ALIGNMENT_FACTOR 0.3

typedef struct {
    Vector2 pos;
    float ori;
} boid;
boid boid_array[BOIDS_LEN] = {0};

int rand_range(int min, int max)
{
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

double randzto(void)
{
   return (double)rand()/(double)RAND_MAX;
}
void init_boids(void)
{
    srand(time(NULL));
    for (size_t i = 0; i < BOIDS_LEN; i++) {
        boid_array[i].pos.x = rand_range(0, WIDTH);
        boid_array[i].pos.y = rand_range(0, HEIGHT);
        boid_array[i].ori = randzto()*2*PI;
    }
}

Vector2 rotate_point(Vector2 o, Vector2 p, float t)
{
    p.x = p.x - o.x;
    p.y = p.y - o.y;
    float new_x = p.x * cos(t) - p.y * sin(t);
    float new_y = p.y * cos(t) + p.x * sin(t);
    new_x = new_x + o.x;
    new_y = new_y + o.y;
    return (Vector2) {.x = new_x, .y = new_y};
}


double euclidean_distance(Vector2 a, Vector2 b)
{
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

Vector2 vector_sub(Vector2 a, Vector2 b)
{
    return (Vector2){.x = b.x - a.x, .y = b.y - a.y};
}

void show_neighbours(size_t i)
{
    Vector2 p = { .x = boid_array[i].pos.x, .y = boid_array[i].pos.y};
    DrawCircleLines(p.x, p.y, BOID_RADIUS, WHITE);
    DrawCircleLines(p.x, p.y, COLLISION_RADIUS, VIOLET);
    size_t n = 0;
    for (size_t j = 0; j < BOIDS_LEN; j++) {
       if (j == i) continue;
       if (euclidean_distance(boid_array[j].pos, p) < BOID_RADIUS) n++;
    }
    DrawText(TextFormat("%d", n), p.x, p.y + 10, 12, WHITE);
}

void update_boids(void)
{
    for (size_t i = 0; i < BOIDS_LEN; i++) {
        Vector2 p = boid_array[i].pos;
        float o = boid_array[i].ori;

        size_t n = 0;
        Vector2 mean_point = {0};
        Vector2 avg_heading = {0};
        Vector2 separation = {0};
        for (size_t j = 0; j < BOIDS_LEN; j++) {
           if (j == i) continue;
            float dist = euclidean_distance(boid_array[j].pos, p);
           if (dist < BOID_RADIUS) {
                mean_point.x = mean_point.x + boid_array[j].pos.x;
                mean_point.y = mean_point.y + boid_array[j].pos.y;

                avg_heading.x += cosf(boid_array[j].ori);
                avg_heading.y += sinf(boid_array[j].ori);

                n++;
           }
           if (dist < COLLISION_RADIUS && dist > 0.0f) {
            separation.x += (p.x - boid_array[j].pos.x) / dist;
            separation.y += (p.y - boid_array[j].pos.y) / dist;
           }
        }

        float new_angle = o;
        if (n != 0) {
            mean_point.x = mean_point.x / n;
            mean_point.y = mean_point.y / n;
            float desired_angle = atan2(mean_point.y - p.y, mean_point.x - p.x);

            avg_heading.x /= n;
            avg_heading.y /= n;

            Vector2 blended = {
                .x = cos(o) + COHESION_FACTOR * cos(desired_angle) + SEPARATION_FACTOR * separation.x + ALIGNMENT_FACTOR * avg_heading.x,
                .y = sin(o) + COHESION_FACTOR * sin(desired_angle) + SEPARATION_FACTOR * separation.y + ALIGNMENT_FACTOR * avg_heading.y
            };
            new_angle = atan2(blended.y, blended.x);

        }
        boid_array[i].ori = new_angle;
        Vector2 new_pos = {
            .x = (p.x + V * cos(new_angle) * GetFrameTime()),
            .y = (p.y + V * sin(new_angle) * GetFrameTime())
        };

        if (new_pos.x < 0) new_pos.x = new_pos.x + WIDTH;
        else if (new_pos.x >= WIDTH) new_pos.x = new_pos.x - WIDTH;

        if (new_pos.y < 0) new_pos.y = new_pos.y + HEIGHT;
        else if (new_pos.y >= HEIGHT) new_pos.y = new_pos.y - HEIGHT;

        boid_array[i].pos = new_pos;
    }
}
int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Boids algorithm");
    init_boids();
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        update_boids();
        BeginDrawing();
            ClearBackground(DARKGRAY);
            for (size_t i = 0; i < BOIDS_LEN; i++) {
                Vector2 pos = { .x = boid_array[i].pos.x, .y = boid_array[i].pos.y};
                DrawCircle(pos.x, pos.y, BOID_SIZE, BLUE);
                // show_neighbours(i);
                float ori = boid_array[i].ori;
                Vector2 pos_ori = { .x = pos.x + 10, .y = pos.y};
                Vector2 rotated = rotate_point(pos, pos_ori, ori);
                DrawLineEx(pos, rotated, 1, ORANGE);
            }
        EndDrawing();
        DrawFPS(0, 0);
    }
    CloseWindow();
    return 0;
}
