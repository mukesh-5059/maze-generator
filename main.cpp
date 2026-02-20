#include "raylib.h"
#include <math.h>
#include <stdlib.h> 
#include <time.h>   
#include <stdio.h> 

#define MAP_SIZE 15
#define TILE_SIZE 24 

struct p {
    float wx, wy, wz, r;
    int mx, my;
    float speed;
};

const int screenWidth = MAP_SIZE * TILE_SIZE;
const int screenHeight = MAP_SIZE * TILE_SIZE;

struct p player;
char map[MAP_SIZE][MAP_SIZE + 1];

double startTime;
bool winner = false;
double finishTime = 0;

void generate_maze(int x, int y) {
    map[y][x] = ' '; 
    
    int dirs[4][2] = {{0, -2}, {0, 2}, {-2, 0}, {2, 0}};
    
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp_dx = dirs[i][0];
        int temp_dy = dirs[i][1];
        dirs[i][0] = dirs[r][0];
        dirs[i][1] = dirs[r][1];
        dirs[r][0] = temp_dx;
        dirs[r][1] = temp_dy;
    }
    
    for (int i = 0; i < 4; i++) {
        int nx = x + dirs[i][0];
        int ny = y + dirs[i][1];
        
        if (nx > 0 && nx < MAP_SIZE - 1 && ny > 0 && ny < MAP_SIZE - 1 && map[ny][nx] == '#') {
            map[y + dirs[i][1] / 2][x + dirs[i][0] / 2] = ' '; 
            generate_maze(nx, ny); 
        }
    }
}

void init_maze() {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) map[i][j] = '#';
        map[i][MAP_SIZE] = '\0'; 
    }
    
    generate_maze(1, 1);
    
    map[MAP_SIZE - 2][MAP_SIZE - 2] = 'E';
}

void init(){
    srand(time(NULL));
    init_maze();
    for(int i = 0; i<MAP_SIZE; i++){
        for(int j = 0; j<MAP_SIZE; j++) printf("%c", map[i][j]);
        printf("\n");
    }

    player.mx = 1;
    player.my = 1;
    player.wx = player.mx * TILE_SIZE + (TILE_SIZE / 2.0f);
    player.wy = player.my * TILE_SIZE + (TILE_SIZE / 2.0f);
    player.r = PI; 
    player.speed = 150.0f; 
    
    startTime = GetTime(); 
    winner = false;
}

void draw(){
    ClearBackground(BLACK);

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            if (map[y][x] == '#') {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
            } else if (map[y][x] == 'E') {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
            } else {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RAYWHITE);
            }
            // Draw grid lines
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
        }
    }
    
    DrawCircle((int)player.wx, (int)player.wy, TILE_SIZE / 3.0f, BLUE);
    Vector2 lineEnd = { 
        player.wx + cos(player.r) * (TILE_SIZE * 1.2f), 
        player.wy + sin(player.r) * (TILE_SIZE * 1.2f) 
    };
    DrawLineEx((Vector2){player.wx, player.wy}, lineEnd, 3.0f, RED);
    
    DrawRectangle(0, 0, screenWidth, 40, Fade(BLACK, 0.8f));
    
    double currentTime = winner ? finishTime : (GetTime() - startTime);
    
    if (winner) DrawText(TextFormat("WINNER! Time: %.2f sec | Press R to Restart", currentTime), 10, 10, 20, GREEN);
    else DrawText(TextFormat("FPS: %d | Time: %.2f", GetFPS(), currentTime), 10, 10, 20, WHITE);
    
}

void collisions(float dx, float dy){
    float next_wx = player.wx;
    float next_wy = player.wy;
    next_wx += dx;
    next_wy += dy;
    int next_mx = (int)(next_wx / TILE_SIZE);
    int next_my = (int)(next_wy / TILE_SIZE);
    
    // Only move if we aren't hitting a wall
    if (map[next_my][next_mx] != '#') {
        player.wx = next_wx;
        player.wy = next_wy;
        player.mx = next_mx;
        player.my = next_my;
        
        // Check Win Condition
        if (map[player.my][player.mx] == 'E' && !winner) {
            winner = true;
            finishTime = GetTime() - startTime;
        }
    }
}

void inputs(){
    if (winner) {
        if (IsKeyPressed(KEY_R)) init(); // Allow restart
        return;
    }

    float dt = GetFrameTime();

    if (IsKeyDown(KEY_A)) player.r -= 4.0 * dt;
    if (IsKeyDown(KEY_D)) player.r += 4.0 * dt;
    float mov_dist = player.speed * dt;
    float dx = cos(player.r) * mov_dist;
    float dy = sin(player.r) * mov_dist;
    
    if (IsKeyDown(KEY_W)) {
        collisions(dx, 0);
        collisions(0, dy);
    }
    if (IsKeyDown(KEY_S)) {
        collisions(-dx, 0);
        collisions(0, -dy);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "");
    SetTargetFPS(60);

    init();
    
    while (!WindowShouldClose()) {
        inputs();
        BeginDrawing();
        draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}