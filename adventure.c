#include <stdio.h>
#include <stdlib.h>
#include "adventure.h"

void play_adventure(struct Player *p, int *day, int *hour) {
    int depth  = 1;
    int width  = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
    int height = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
    struct Map *m = map_create(width, height, depth);
    if (!m) return;
    map_spawn_hero(m, p);

    char dir;
    while (1) {
        const char *state_str[] = {"Налегке", "Норма", "Перегруз"};
        printf("\n[ HUD ] HP: %d/%d | Stamina: %d/%d | Золото: %d | Вес: %d/%d (%s) | Этаж: %d | Время: %02d:00\n",
               p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold,
               p->backpack.current_weight, p->backpack.max_weight, state_str[p->backpack.state],
               m->floor, *hour);
               
        map_print(m, p);
        printf("\nДвижение (W/A/S/D), Q - выход: ");
        if (scanf(" %c", &dir) != 1) { while (getchar() != '\n'); continue; }

        if (dir == 'Q' || dir == 'q') {
            if (m->grid[p->y][p->x] == CELL_EXIT) { printf("\nВы дома! Золото сохранено.\n"); break; }
            else { printf("Выход только с 'U'!\n"); continue; }
        }

        if (map_move_hero(m, p, dir) == 1) {
            int cost = (p->backpack.state == LOAD_OVERLOAD) ? 3 : 1;
            p->stamina -= cost; 
        }

        if (p->stamina <= 0) {
            printf("\n!!! Вы упали без сил !!!\nСпасатели забрали половину золота.\n");
            p->stamina = 0; p->gold /= 2;
            map_destroy(m); return;
        }

        if (m->grid[p->y][p->x] == CELL_DESCEND) {
            printf("\nВы спустились на следующий этаж!\n");
            p->stamina -= 5; *hour += 1;
            if (*hour >= 24) { *hour = 0; *day += 1; }
            
            if (p->stamina <= 0 || *hour == SLEEP_BOUNDARY) {
                printf("\n!!! Обморок (усталость или ночь) !!!\nСпасатели забрали половину золота.\n");
                if (p->stamina < 0) p->stamina = 0;
                p->gold /= 2; map_destroy(m); return;
            }

            map_destroy(m); depth++;
            int reward = BASE_FLOOR_GOLD * depth;
            p->gold += reward;
            printf("Награда за %d этаж: +%d золота. (Всего: %d)\n", depth, reward, p->gold);

            width  = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
            height = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
            m = map_create(width, height, depth);
            if (!m) return;
            map_spawn_hero(m, p);
        }
    }
    map_destroy(m);
}