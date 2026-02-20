#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cmath>
#include <unistd.h>

#define TARGET_FPS 60
#define FRAME_TIME_USEC (1000000 / TARGET_FPS)
#define MAP_SIZE 32
#define TILE_SIZE 8

const double todeg=180/M_PI;
const double torad=M_PI/180;


float fov = torad * 70;
int toggle_map = 0;
int height, width, ch;
char *display_buffer;

struct p {
    float wx, wy, wz;
    int mx = 1, my = 1;
    float speed = 50.0;
};

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
    "### ## ## ########## #### ## ###",
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









void init_buf() {
    getmaxyx(stdscr, height, width);
    if (display_buffer) free(display_buffer);
    display_buffer = (char*)malloc(height * width + 1);
}

void init() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    player.wz = 10;
    player.wx = player.mx * TILE_SIZE;
    player.wy = player.my * TILE_SIZE;
}

void insert(int y, int x, char v) {
    if (y >= 0 && y < height && x >= 0 && x < width) 
        display_buffer[width * y + x] = v;
}

void draw() {
    memset(display_buffer, ' ', height * width);
    //insert((int)player.wy, (int)player.wx, '@');
    if(toggle_map == 1){
        for(int i = 0; i<MAP_SIZE; i++)
            for(int j = 0; j<MAP_SIZE; j++) insert(i+1, j, map[i][j]);
        insert(player.my + 1, player.mx, '*');
    }
    mvaddnstr(0, 0, display_buffer, height * width);
}

void collision(float dx, float dy) {
    // We update the world position
    player.wx += dx;
    player.wy += dy;
    
    // Recalculate map grid coordinates
    player.mx = (int)(player.wx / TILE_SIZE);
    player.my = (int)(player.wy / TILE_SIZE);

    // Check collision: map[Y_INDEX][X_INDEX]
    if (map[player.my][player.mx] == '#') {
        player.wx -= dx; // Revert X
        player.wy -= dy; // Revert Y
        player.mx = (int)(player.wx / TILE_SIZE);
        player.my = (int)(player.wy / TILE_SIZE);
    }
}

void inputs() {
    ch = getch();
    if (ch == ERR) return;
    
    float dt = 1.0 / TARGET_FPS; 
    float mov_dist = player.speed * dt;
    
    if (ch == 'd') {
        collision(mov_dist, 0);
    }
    else if (ch == 'a') {
        collision(-1 * mov_dist, 0);
    }
    else if (ch == 's') {
        collision(0, mov_dist);
    }
    else if (ch == 'w') {
        collision(0, -mov_dist);
    }
    else if (ch == 'm') {
        toggle_map = !toggle_map;
    }
}








int main() {
    init();
    init_buf();

    struct timespec start, end;
    float fps = 0.0; // Units per second

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        inputs();

        draw();
        attron(A_BOLD);
        mvprintw(0, 0, "FPS: %2.0f  |  Pos: [%2.1f, %2.1f]", fps, player.wx, player.wy);
        refresh();

        clock_gettime(CLOCK_MONOTONIC, &end);
        long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + 
                       (end.tv_nsec - start.tv_nsec) / 1000;

        insert(0, 0, 'F');
        insert(1, 0, 'P');
        insert(2, 0, 'S');
        if (elapsed < FRAME_TIME_USEC) {
            usleep(FRAME_TIME_USEC - elapsed);
        }
        struct timespec total_end;
        clock_gettime(CLOCK_MONOTONIC, &total_end);
        double total_frame_time = (total_end.tv_sec - start.tv_sec) + 
                                  (total_end.tv_nsec - start.tv_nsec) / 1e9;
        fps = 1.0 / total_frame_time;
    }

    if (display_buffer) free(display_buffer);
    endwin();
    return 0;
}