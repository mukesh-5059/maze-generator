#include "raylib.h"
#include <math.h>

#define MAP_SIZE 32
#define TILE_SIZE 24 // Increased from 8 so the window isn't tiny

struct p {
    float wx, wy, wz, r;
    int mx, my;
    float speed;
};

const int screenWidth = MAP_SIZE * TILE_SIZE;
const int screenHeight = MAP_SIZE * TILE_SIZE;

struct p player;

const char *map[MAP_SIZE] = {
    "################################",
    "#              ##              #",
    "# #### ####### ## ####### #### #",
    "# #  # #     # ## #     # #  # #",
    "# #### ####### ## ####### #### #",
    "#                              #",
    "# #### ## ########## ## ####   #",
    "#      ##     ##     ##        #",
    "###### ### ######## ### ########",
    "#    # ##            ## #      #",
    "#    # ## ########## ## #      #",
    "###### ## #        # ## ########",
    "#         #        #           #",
    "###### ## #        # ## ########",
    "#    # ## ########## ## #      #",
    "#    # ##            ## #      #",
    "###### ## ########## ## ########",
    "#              ##              #",
    "# #### ####### ## ####### #### #",
    "#   ##         ##         ##   #",
    "### ## ## ########## ## ## #####",
    "#      ##     ##     ##        #",
    "# ########### ## ###########   #",
    "# #           ##           #   #",
    "# # ######### ## ######### #   #",
    "# #           ##           #   #",
    "# ########### ## ###########   #",
    "#                              #",
    "# ########## #  # ##########   #",
    "# #        # #  # #        #   #",
    "# #        # #  # #        #   #",
    "################################"
};

void init(){
    player.mx = 1;
    player.my = 1;
    player.wx = player.mx * TILE_SIZE + (TILE_SIZE / 2.0f);
    player.wy = player.my * TILE_SIZE + (TILE_SIZE / 2.0f);
    player.r = PI; 
    player.speed = 150.0f; 
}

void draw(){
    ClearBackground(BLACK);

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            if (map[y][x] == '#') {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
            } else {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RAYWHITE);
            }
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
        }
    }
    DrawCircle((int)player.wx, (int)player.wy, TILE_SIZE / 3.0f, BLUE);
    Vector2 lineEnd = { 
        player.wx + cos(player.r) * (TILE_SIZE * 1.2f), 
        player.wy + sin(player.r) * (TILE_SIZE * 1.2f) 
    };
    DrawLineEx((Vector2){player.wx, player.wy}, lineEnd, 3.0f, RED);
    DrawRectangle(0, 0, 420, 30, Fade(BLACK, 0.7f));
    DrawText(TextFormat("FPS: %d  |  Pos: [%.1f, %.1f]  |  Rot: %.2f", 
             GetFPS(), player.wx, player.wy, player.r), 10, 5, 20, GREEN);
}


void collisions(float dx, float dy){
    float next_wx = player.wx;
    float next_wy = player.wy;
    next_wx += dx;
    next_wy += dy;
    int next_mx = (int)(next_wx / TILE_SIZE);
    int next_my = (int)(next_wy / TILE_SIZE);
    if (map[next_my][next_mx] != '#') {
        player.wx = next_wx;
        player.wy = next_wy;
        player.mx = next_mx;
        player.my = next_my;
    }
}

void inputs(){
    float dt = GetFrameTime();

    if (IsKeyDown(KEY_A)) player.r -= 3.0f * dt;
    if (IsKeyDown(KEY_D)) player.r += 3.0f * dt;
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
    InitWindow(screenWidth, screenHeight, "Raylib Top-Down Map");
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