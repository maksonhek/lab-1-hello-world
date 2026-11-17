#include <stdio.h>
#include <stdlib.h>
#include "map.h"

struct Map *map_create(int width, int height, int floor) {
    struct Map *m = malloc(sizeof(struct Map));
    if (!m) return NULL;
    m->width = width; m->height = height; m->floor = floor;
    m->grid = malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) m->grid[i] = malloc(width * sizeof(char));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) m->grid[y][x] = CELL_WALL;
            else {
                int chance = rand() % 100;
                if      (chance < 15) m->grid[y][x] = CELL_TREE;
                else if (chance < 25) m->grid[y][x] = CELL_ROCK;
                else                  m->grid[y][x] = CELL_FLOOR;
            }
        }
    }
    while (1) {
        int dx = rand() % (width - 2) + 1; int dy = rand() % (height - 2) + 1;
        if (m->grid[dy][dx] == CELL_FLOOR) { m->grid[dy][dx] = CELL_DESCEND; break; }
    }
    return m;
}

void map_destroy(struct Map *m) {
    if (!m) return;
    for (int i = 0; i < m->height; i++) free(m->grid[i]);
    free(m->grid);
    printf("Память структуры по адресу %p освобождена\n", (void *)m);
    free(m);
}

void map_spawn_hero(struct Map *m, struct Player *p) {
    while (1) {
        int x = rand() % (m->width - 2) + 1; int y = rand() % (m->height - 2) + 1;
        if (m->grid[y][x] == CELL_FLOOR) { m->grid[y][x] = CELL_EXIT; p->x = x; p->y = y; return; }
    }
}

void map_print(const struct Map *m, const struct Player *p) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            int dx = x - p->x; int dy = y - p->y;
            if (dx * dx + dy * dy <= FOG_RADIUS * FOG_RADIUS) {
                if (x == p->x && y == p->y) printf("@ ");
                else printf("%c ", m->grid[y][x]);
            } else printf("? ");
        }
        printf("\n");
    }
}

int map_move_hero(struct Map *m, struct Player *p, char dir) {
    int new_x = p->x; int new_y = p->y;
    if      (dir == 'W' || dir == 'w') new_y--;
    else if (dir == 'S' || dir == 's') new_y++;
    else if (dir == 'A' || dir == 'a') new_x--;
    else if (dir == 'D' || dir == 'd') new_x++;
    else return 0;

    if (new_x < 0 || new_x >= m->width || new_y < 0 || new_y >= m->height) return 0;
    if (m->grid[new_y][new_x] == CELL_FLOOR || m->grid[new_y][new_x] == CELL_EXIT || m->grid[new_y][new_x] == CELL_DESCEND) {
        p->x = new_x; p->y = new_y; return 1;
    } else { printf("Туда нельзя идти!\n"); return 0; }
}