#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ITEMS            10
#define MAX_NAME_LEN         32
#define MAX_FARMER_NAME_LEN  32
#define MIN_ID               0
#define MAX_ID               (MAX_ITEMS - 1)
#define HP_MAX_DEFAULT       100
#define STAMINA_MAX_DEFAULT  50
#define GOLD_START           0

struct Player {
    char name[MAX_FARMER_NAME_LEN];
    int  hp;
    int  max_hp;
    int  stamina;
    int  max_stamina;
    int  gold;
    int  x;
    int  y;
    int  inventory[MAX_ITEMS];
};

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define FOG_RADIUS   4
#define BASE_FLOOR_GOLD 15 

enum CellType {
    CELL_WALL    = '#',
    CELL_FLOOR   = '.',
    CELL_TREE    = 'T',
    CELL_ROCK    = 'R',
    CELL_EXIT    = 'U',
    CELL_DESCEND = 'D'
};

struct Map {
    char **grid;
    int    width;
    int    height;
    int    floor;
};

#define MAX_LINE_LEN   128
#define START_DAY      1
#define START_HOUR     8
#define HOURS_PER_DAY  24
#define WAKE_UP_HOUR   8
#define SLEEP_BOUNDARY 2

enum MenuAction {
    MENU_EXIT = 0,
    MENU_CREATE_PLAYER,
    MENU_PLAYER_CARD,
    MENU_TIME,
    MENU_INVENTORY,
    MENU_PUT_ITEM,
    MENU_DROP_ITEM,
    MENU_QUICK_ACCESS,
    MENU_SEARCH_ITEM,
    MENU_DIARY,
    MENU_PARSE_INI,
    MENU_ADVENTURE
};

int read_int(const char *prompt, int min, int max) {
    int value, ok;
    do {
        ok = 0;
        printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            printf("Ошибка: нужно ввести число.\n");
            while (getchar() != '\n');
        } else if (value < min || value > max) {
            printf("Ошибка: число должно быть от %d до %d.\n", min, max);
        } else {
            ok = 1;
            while (getchar() != '\n');
        }
    } while (!ok);
    return value;
}

void set_default_names(char names[][MAX_NAME_LEN]) {
    strcpy(names[0], "Пусто"); strcpy(names[1], "Дерево"); strcpy(names[2], "Камень");
    strcpy(names[3], "Семена"); strcpy(names[4], "Железо"); strcpy(names[5], "Уголь");
    strcpy(names[6], "Факел"); strcpy(names[7], "Вода"); strcpy(names[8], "Еда");
    strcpy(names[9], "Инструмент");
}

void load_item_names(char names[][MAX_NAME_LEN]) {
    FILE *file = fopen("items.txt", "r");
    if (file == NULL) {
        printf("Предупреждение: файл items.txt не найден!\n");
        printf("Используются стандартные названия предметов.\n");
        set_default_names(names);
        return;
    }
    char line[MAX_LINE_LEN]; int id; char name[MAX_NAME_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d %31[^\n]", &id, name) == 2) {
            if (id >= 0 && id < MAX_ITEMS) strcpy(names[id], name);
        }
    }
    fclose(file);
}

void read_farmer_name(char *name, int size) {
    while (1) {
        printf("Введите имя фермера (до %d символов): ", size - 1);
        fgets(name, size, stdin);
        name[strcspn(name, "\n")] = '\0';
        if (strlen(name) == 0) {
            printf("Имя не может быть пустым. Попробуйте ещё раз.\n");
            continue;
        }
        return;
    }
}

void read_string(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    if (strchr(buffer, '\n') == NULL) {
        while (getchar() != '\n');
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

void parse_ini_file(void) {
    FILE *in_file = fopen("input.txt", "r");
    if (in_file == NULL) { printf("Ошибка: не удалось открыть input.txt!\n"); return; }
    FILE *out_file = fopen("output.txt", "w");
    if (out_file == NULL) { printf("Ошибка: не удалось создать output.txt!\n"); fclose(in_file); return; }
    char line[MAX_LINE_LEN]; int loaded = 0, skipped = 0;
    while (fgets(line, sizeof(line), in_file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0 || line[0] == '#') { skipped++; continue; }
        char *equals_pos = strchr(line, '=');
        if (equals_pos == NULL) { skipped++; continue; }
        *equals_pos = '\0';
        char *key = line; char *value = equals_pos + 1;
        if (strlen(key) == 0 || strlen(value) == 0) { skipped++; continue; }
        loaded++;
        fprintf(stdout, "Параметр: [%s] | Значение: [%s]\n", key, value);
        fprintf(out_file, "Параметр: [%s] | Значение: [%s]\n", key, value);
    }
    fprintf(stdout, "\nУспешно считано настроек: %d\n", loaded);
    fprintf(stdout, "Отброшено строк: %d\n", skipped);
    fprintf(out_file, "\nУспешно считано настроек: %d\n", loaded);
    fprintf(out_file, "Отброшено строк: %d\n", skipped);
    fclose(in_file); fclose(out_file);
    printf("\nРезультат также сохранен в output.txt\n");
}

struct Player *player_create(void) {
    struct Player *p = malloc(sizeof(struct Player));
    if (!p) { printf("Ошибка: не хватило памяти для персонажа!\n"); return NULL; }
    read_farmer_name(p->name, MAX_FARMER_NAME_LEN);
    p->max_hp = HP_MAX_DEFAULT; p->hp = p->max_hp;
    p->max_stamina = STAMINA_MAX_DEFAULT; p->stamina = p->max_stamina;
    p->gold = GOLD_START; p->x = 0; p->y = 0;
    for (int i = 0; i < MAX_ITEMS; i++) p->inventory[i] = 0;
    p->inventory[0] = 1; p->inventory[1] = 2; p->inventory[2] = 3;
    p->inventory[3] = 0; p->inventory[4] = 4; p->inventory[5] = 1;
    p->inventory[6] = 3; p->inventory[7] = 5; p->inventory[8] = 0; p->inventory[9] = 4;
    printf("Персонаж '%s' создан. Адрес структуры: %p\n", p->name, (void *)p);
    return p;
}

void player_destroy(struct Player *p) {
    if (!p) return;
    printf("Память структуры по адресу %p освобождена\n", (void *)p);
    free(p);
}

void player_print_card(const struct Player *p, int day, int hour, char names[][MAX_NAME_LEN]) {
    time_t now = time(NULL); struct tm *tm_now = localtime(&now);
    char date_buf[32];
    if (tm_now) strftime(date_buf, sizeof(date_buf), "%d.%m.%Y", tm_now);
    else strcpy(date_buf, "(недоступно)");

    printf("\n=============== КАРТОЧКА ПЕРСОНАЖА ===============\n");
    printf("  Имя:        %s\n", p->name);
    printf("  HP:         %d/%d\n", p->hp, p->max_hp);
    printf("  Stamina:    %d/%d\n", p->stamina, p->max_stamina);
    printf("  Золото:     %d\n", p->gold);
    printf("  Позиция:    (%d, %d)\n", p->x, p->y);
    printf("  Дата:       %s\n", date_buf);
    printf("  Игровое вр.: День %d, %02d:00\n", day, hour);
    printf("  Инвентарь:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("    Слот %d: [%d] - %s\n", i, p->inventory[i], names[p->inventory[i]]);
    }
    printf("====================================================\n");
}

void player_print_inventory(const struct Player *p, char names[][MAX_NAME_LEN]) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("Слот %d: [%d] - %s\n", i, p->inventory[i], names[p->inventory[i]]);
    }
}

void player_search_item(const struct Player *p, char names[][MAX_NAME_LEN]) {
    char search_name[MAX_NAME_LEN];
    read_string("Введите название предмета для поиска: ", search_name, MAX_NAME_LEN);
    if (strlen(search_name) == 0) { printf("Название не может быть пустым.\n"); return; }
    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(search_name, names[i]) == 0) { found_id = i; break; }
    }
    if (found_id == -1) { printf("Предмет с названием '%s' не найден в каталоге.\n", search_name); return; }
    int count = 0;
    printf("Предмет '%s' (ID %d) находится в слотах: ", search_name, found_id);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (p->inventory[i] == found_id) { printf("%d ", i); count++; }
    }
    if (count == 0) printf("\nВ рюкзаке нет ни одного предмета с таким названием.\n");
    else           printf("\nВсего найдено: %d шт.\n", count);
}

void player_write_diary(FILE *out, const struct Player *p, int day, int hour, char names[][MAX_NAME_LEN]) {
    fprintf(out, "\n==================================\n");
    fprintf(out, "=== Запись в дневник фермера ===\n");
    fprintf(out, "Фермер: %s\n", p->name);
    fprintf(out, "Время:  День %d, %02d:00\n", day, hour);
    fprintf(out, "Инвентарь:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        fprintf(out, "  Слот %d: [%d] - %s\n", i, p->inventory[i], names[p->inventory[i]]);
    }
    fprintf(out, "==================================\n");
}

struct Map *map_create(int width, int height, int floor) {
    struct Map *m = malloc(sizeof(struct Map));
    if (!m) { printf("Ошибка: не хватило памяти для структуры карты!\n"); return NULL; }
    m->width = width; m->height = height; m->floor = floor;
    m->grid = malloc(height * sizeof(char *));
    if (!m->grid) { free(m); return NULL; }
    for (int i = 0; i < height; i++) {
        m->grid[i] = malloc(width * sizeof(char));
        if (!m->grid[i]) {
            for (int j = 0; j < i; j++) free(m->grid[j]);
            free(m->grid); free(m); return NULL;
        }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                m->grid[y][x] = CELL_WALL;
            } else {
                int chance = rand() % 100;
                if      (chance < 15) m->grid[y][x] = CELL_TREE;
                else if (chance < 25) m->grid[y][x] = CELL_ROCK;
                else                  m->grid[y][x] = CELL_FLOOR;
            }
        }
    }
    while (1) {
        int dx = rand() % (width - 2) + 1;
        int dy = rand() % (height - 2) + 1;
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
        int x = rand() % (m->width - 2) + 1;
        int y = rand() % (m->height - 2) + 1;
        if (m->grid[y][x] == CELL_FLOOR) {
            m->grid[y][x] = CELL_EXIT; p->x = x; p->y = y; return;
        }
    }
}

void map_print(const struct Map *m, const struct Player *p) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            int dx = x - p->x; int dy = y - p->y;
            if (dx * dx + dy * dy <= FOG_RADIUS * FOG_RADIUS) {
                if (x == p->x && y == p->y) printf("@ ");
                else                         printf("%c ", m->grid[y][x]);
            } else {
                printf("? ");
            }
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
        p->x = new_x; p->y = new_y;
        return 1; 
    } else {
        printf("Туда нельзя идти! Там препятствие.\n");
        return 0; 
    }
}

void play_adventure(struct Player *p, int *day, int *hour) {
    int depth  = 1;
    int width  = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
    int height = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
    struct Map *m = map_create(width, height, depth);
    if (!m) return;
    map_spawn_hero(m, p);

    char dir;
    while (1) {
        printf("\n[ HUD ] HP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00\n",
               p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold, m->floor, *hour);
               
        printf("--- Подземелье (Размер: %dx%d) ---\n", m->width, m->height);
        map_print(m, p);
        printf("\nДвижение (W/A/S/D), Q - выход (только на 'U'): ");
        
        if (scanf(" %c", &dir) != 1) { while (getchar() != '\n'); continue; }

        if (dir == 'Q' || dir == 'q') {
            if (m->grid[p->y][p->x] == CELL_EXIT) {
                printf("\nВы благополучно вернулись домой с %d этажа!\n", m->floor);
                printf("Всё заработанное золото осталось при вас.\n");
                break; 
            } else {
                printf("Выход на поверхность доступен только с лестницы 'U'!\n");
                continue;
            }
        }

        if (map_move_hero(m, p, dir) == 1) {
            p->stamina -= 1; 
        }

        if (p->stamina <= 0) {
            printf("\n!!! Вы выбились из сил и упали без сознания !!!\n");
            printf("Спасательная служба доставила вас домой, забрав половину золота за услуги.\n");
            p->stamina = 0;
            p->gold /= 2;
            printf("У вас осталось золота: %d\n", p->gold);
            map_destroy(m);
            return; 
        }

        if (m->grid[p->y][p->x] == CELL_DESCEND) {
            printf("\nВы спустились на следующий этаж!\n");
            p->stamina -= 5;   
            *hour += 1;        
            if (*hour >= 24) { *hour = 0; *day += 1; }

            if (p->stamina <= 0 || *hour == SLEEP_BOUNDARY) {
                printf("\n!!! Вы упали без сил !!!\n");
                if (p->stamina <= 0) printf("Причина: полное истощение.\n");
                if (*hour == SLEEP_BOUNDARY) printf("Причина: наступила глубокая ночь (02:00).\n");
                printf("Спасательная служба доставила вас домой, забрав половину золота.\n");
                if (p->stamina < 0) p->stamina = 0;
                p->gold /= 2;
                printf("У вас осталось золота: %d\n", p->gold);
                map_destroy(m);
                return;
            }

            map_destroy(m);
            depth++;
            int reward = BASE_FLOOR_GOLD * depth;
            p->gold += reward;
            printf("Награда за исследование %d этажа: +%d золота. (Всего: %d)\n", depth, reward, p->gold);

            width  = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
            height = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
            m = map_create(width, height, depth);
            if (!m) return;
            map_spawn_hero(m, p);
        }
    }
    map_destroy(m); 
}

int hours_until_2am(int current_hour) {
    if (current_hour < SLEEP_BOUNDARY) return SLEEP_BOUNDARY - current_hour;
    if (current_hour == SLEEP_BOUNDARY) return 0;
    return (HOURS_PER_DAY - current_hour) + SLEEP_BOUNDARY;
}

void menu_time(struct Player *p, int *day, int *hour) {
    while (1) {
        printf("\n--- ВРЕМЯ И ОТДЫХ ---\n");
        printf("Текущее время: День %d, %02d:00\n", *day, *hour);
        printf("[0] Назад\n[1] Часы\n[2] Подождать\n[3] Лечь спать\n");
        int choice = read_int("Выберите действие: ", 0, 3);
        
        if (choice == 0) break;
        if (choice == 1) {
            printf("Сейчас День %d, %02d:00\n", *day, *hour);
        }
        else if (choice == 2) {
            int max_wait = hours_until_2am(*hour);
            if (max_wait == 0) { printf("Сейчас 02:00. Пора спать, ждать нельзя!\n"); continue; }
            printf("Максимально можно подождать: %d ч. (до 02:00)\n", max_wait);
            int hours = read_int("Сколько часов хотите подождать? ", 1, max_wait);
            
            *hour += hours;
            *day  += *hour / HOURS_PER_DAY;
            *hour  = *hour % HOURS_PER_DAY;
            printf("Прошло %d ч. Время: День %d, %02d:00\n", hours, *day, *hour);
        }
        else if (choice == 3) {
            if (*hour < SLEEP_BOUNDARY || *hour >= WAKE_UP_HOUR) *day += 1;
            *hour = WAKE_UP_HOUR;
            printf("Фермер выспался. Время: День %d, %02d:00\n", *day, *hour);
            if (p != NULL) {
                p->hp = p->max_hp; p->stamina = p->max_stamina;
                printf("Здоровье и выносливость восстановлены!\n");
            }
        }
    }
}

static void print_menu(const struct Player *player) {
    printf("\n %s THE GAME\n", player ? player->name : "(персонаж не создан)");
    printf("[0] Выход\n[1] Создать / пересоздать\n[2] Карточка\n[3] Время и отдых\n");
    printf("[4] Инвентарь\n[5] Положить предмет\n[6] Выбросить предмет\n[7] Быстрый доступ\n");
    printf("[8] Поиск предмета\n[9] Дневник\n[10] Расшифровать настройки\n[11] Путешествовать\n");
    printf("==================================\n");
}

static void ensure_player(const struct Player *player) {
    if (player == NULL) printf("Внимание: сначала создайте персонажа (пункт 1).\n");
}

int main(void) {
    int current_day  = START_DAY;
    int current_hour = START_HOUR;
    srand((unsigned)time(NULL));
    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    load_item_names(item_names);
    struct Player *player = NULL;

    while (1) {
        print_menu(player);
        int choice = read_int("Ваш выбор: ", 0, 11);

        switch ((enum MenuAction)choice) {
        case MENU_EXIT:
            printf("Выход из игры.\n");
            player_destroy(player);
            return 0;

        case MENU_CREATE_PLAYER: {
            if (player != NULL) {
                int confirm = read_int("Персонаж уже существует. Удалить? (1-да, 0-нет): ", 0, 1);
                if (!confirm) { printf("Отменено.\n"); break; }
                player_destroy(player); player = NULL;
            }
            player = player_create();
            break;
        }

        case MENU_PLAYER_CARD:
            if (player == NULL) { ensure_player(player); break; }
            player_print_card(player, current_day, current_hour, item_names);
            break;

        case MENU_TIME:
            menu_time(player, &current_day, &current_hour);
            break;

        case MENU_INVENTORY:
            if (player == NULL) { ensure_player(player); break; }
            player_print_inventory(player, item_names);
            break;

        case MENU_PUT_ITEM: {
            if (player == NULL) { ensure_player(player); break; }
            player_print_inventory(player, item_names);
            int slot = read_int("Слот (0-9): ", 0, MAX_ITEMS - 1);
            int id   = read_int("ID предмета (0-9): ", MIN_ID, MAX_ID);
            player->inventory[slot] = id;
            printf("В слот %d положили %s.\n", slot, item_names[id]);
            break;
        }

        case MENU_DROP_ITEM: {
            if (player == NULL) { ensure_player(player); break; }
            player_print_inventory(player, item_names);
            int slot = read_int("Слот для очистки: ", 0, MAX_ITEMS - 1);
            if (player->inventory[slot] == 0) printf("Слот уже пуст.\n");
            else {
                printf("Выброшен %s.\n", item_names[player->inventory[slot]]);
                player->inventory[slot] = 0;
            }
            break;
        }

        case MENU_QUICK_ACCESS: {
            if (player == NULL) { ensure_player(player); break; }
            player_print_inventory(player, item_names);
            int id = read_int("ID предмета (1-9): ", MIN_ID + 1, MAX_ID);
            int found = -1;
            for (int i = 0; i < MAX_ITEMS; i++) if (player->inventory[i] == id) { found = i; break; }
            if (found == -1) printf("Предмет не найден в инвентаре.\n");
            else if (found == 0) printf("Предмет уже в слоте 0.\n");
            else {
                int temp = player->inventory[0];
                player->inventory[0] = player->inventory[found];
                player->inventory[found] = temp;
                printf("Обменяли слот 0 и слот %d.\n", found);
            }
            break;
        }

        case MENU_SEARCH_ITEM:
            if (player == NULL) { ensure_player(player); break; }
            player_search_item(player, item_names);
            break;

        case MENU_DIARY: {
            if (player == NULL) { ensure_player(player); break; }
            player_write_diary(stdout, player, current_day, current_hour, item_names);
            FILE *file = fopen("diary.txt", "a");
            if (file) { player_write_diary(file, player, current_day, current_hour, item_names); fclose(file); }
            break;
        }

        case MENU_PARSE_INI:
            parse_ini_file();
            break;

        case MENU_ADVENTURE:
            if (player == NULL) {
                ensure_player(player);
                break;
            }
            if (player->stamina <= 0) {
                printf("У вас совсем нет сил на приключения. Отдохните! (Пункт 3)\n");
                break;
            }
            if (current_hour == SLEEP_BOUNDARY) {
                printf("Сейчас 02:00 ночи. Фермер должен спать!\n");
                break;
            }
            play_adventure(player, &current_day, &current_hour);
            break;

        default: break;
        }
    }
    return 0;
}
