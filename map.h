#ifndef MAP_H
#define MAP_H

#include "player.h"

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define FOG_RADIUS   4

enum CellType {
    CELL_WALL    = '#', CELL_FLOOR   = '.',
    CELL_TREE    = 'T', CELL_ROCK    = 'R',
    CELL_EXIT    = 'U', CELL_DESCEND = 'D'
};

struct Map {
    char **grid;
    int    width;
    int    height;
    int    floor;
};

struct Map *map_create(int width, int height, int floor);
void map_destroy(struct Map *m);
void map_spawn_hero(struct Map *m, struct Player *p);
void map_print(const struct Map *m, const struct Player *p);
int map_move_hero(struct Map *m, struct Player *p, char dir);

#endif 
