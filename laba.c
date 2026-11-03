#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>  

#define MAX_ITEMS     10 
#define MAX_NAME_LEN  32 
#define MAX_LINE_LEN  128 
#define MAX_FARMER_NAME_LEN 32
#define MIN_ID 0
#define MAX_ID 9
#define INVENTORY_SIZE 10   
#define START_DAY      1     
#define START_HOUR     8    
#define HOURS_PER_DAY  24    
#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define FOG_RADIUS 4


int read_int(const char *prompt, int min, int max) {
    int value;
    int ok;   

    do {
        ok = 0;  

        printf("%s", prompt);

        if (scanf("%d", &value) != 1) {
            printf("Ошибка: нужно ввести число.\n");
            while (getchar() != '\n');   
        }
        else if (value < min || value > max) {
            printf("Ошибка: число должно быть от %d до %d.\n", min, max);
        }
        else {
            ok = 1;  
                while (getchar() != '\n'); 
        }
    } while (ok == 0);

    return value;
}

void set_default_names(char names[][MAX_NAME_LEN]) {
    strcpy(names[0], "Пусто");
    strcpy(names[1], "Дерево");
    strcpy(names[2], "Камень");
    strcpy(names[3], "Семена");
    strcpy(names[4], "Железо");
    strcpy(names[5], "Уголь");
    strcpy(names[6], "Факел");
    strcpy(names[7], "Вода");
    strcpy(names[8], "Еда");
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

    char line[MAX_LINE_LEN];
    int id;
    char name[MAX_NAME_LEN];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d %31[^\n]", &id, name) == 2) {
            if (id >= 0 && id < MAX_ITEMS) {
                strcpy(names[id], name);
            }
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

void search_item_by_name(const int inv[], int size, char names[][MAX_NAME_LEN]) {
    char search_name[MAX_NAME_LEN];
    
    read_string("Введите название предмета для поиска: ", search_name, MAX_NAME_LEN);

    if (strlen(search_name) == 0) {
        printf("Название не может быть пустым.\n");
        return;
    }

    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(search_name, names[i]) == 0) {
            found_id = i;
            break;
        }
    }

    if (found_id == -1) {
        printf("Предмет с названием '%s' не найден в каталоге.\n", search_name);
        return;
    }

    int count = 0;
    printf("Предмет '%s' (ID %d) находится в слотах: ", search_name, found_id);
    
    for (int i = 0; i < size; i++) {
        if (inv[i] == found_id) {
            printf("%d ", i);
            count++;
        }
    }

    if (count == 0) {
        printf("\nВ рюкзаке нет ни одного предмета с таким названием.\n");
    } else {
        printf("\nВсего найдено: %d шт.\n", count);
    }
}

void print_inventory(const int inv[], int size, char names[][MAX_NAME_LEN]) {
    for (int i = 0; i < size; i++) {
        printf("Слот %d: [%d] - %s\n", i, inv[i], names[inv[i]]);
    }
}

void write_diary_entry(FILE *out, const char *farmer_name, int day, int hour, 
                       const int inv[], int size, char names[][MAX_NAME_LEN]) {
    fprintf(out, "\n==================================\n");
    fprintf(out, "=== Запись в дневник фермера ===\n");
    fprintf(out, "Фермер: %s\n", farmer_name);
    fprintf(out, "Время:  День %d, %02d:00\n", day, hour);
    
    fprintf(out, "Инвентарь:\n");
    for (int i = 0; i < size; i++) {
        fprintf(out, "  Слот %d: [%d] - %s\n", i, inv[i], names[inv[i]]);
    }
    fprintf(out, "==================================\n");
}

void parse_ini_file(void) {
    FILE *in_file = fopen("input.txt", "r");
    if (in_file == NULL) {
        printf("Ошибка: не удалось открыть input.txt!\n");
        return;
    }

    FILE *out_file = fopen("output.txt", "w"); 
    if (out_file == NULL) {
        printf("Ошибка: не удалось создать output.txt!\n");
        fclose(in_file);
        return;
    }

    char line[MAX_LINE_LEN];
    int loaded = 0;  
    int skipped = 0; 

    while (fgets(line, sizeof(line), in_file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0 || line[0] == '#') {
            skipped++;
            continue;
        }

        char *equals_pos = strchr(line, '=');
        if (equals_pos == NULL) {
            skipped++; 
            continue;
        }

        *equals_pos = '\0'; 
        char *key = line;
        char *value = equals_pos + 1; 

        if (strlen(key) == 0 || strlen(value) == 0) {
            skipped++;
            continue;
        }

        loaded++;
        fprintf(stdout, "Параметр: [%s] | Значение: [%s]\n", key, value);
        fprintf(out_file, "Параметр: [%s] | Значение: [%s]\n", key, value);
    }

    fprintf(stdout, "\nУспешно считано настроек: %d\n", loaded);
    fprintf(stdout, "Отброшено строк: %d\n", skipped);
    
    fprintf(out_file, "\nУспешно считано настроек: %d\n", loaded);
    fprintf(out_file, "Отброшено строк: %d\n", skipped);

    fclose(in_file);
    fclose(out_file);
    
    printf("\nРезультат также сохранен в output.txt\n");
}

char **create_map(int width, int height) {
    // 1. Выделяем память под массив указателей (строки карты)
    char **map = (char**)malloc(height * sizeof(char*));
    if (map == NULL) {
        printf("Ошибка: не хватило памяти для карты!\n");
        return NULL;
    }

    // 2. Выделяем память под каждую строку
    for (int i = 0; i < height; i++) {
        map[i] = (char*)malloc(width * sizeof(char));
        if (map[i] == NULL) {
            printf("Ошибка: не хватило памяти для строки карты!\n");
            return NULL; 
        }
    }

    // 3. Заполняем карту
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Внешние границы — это стены
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                map[y][x] = '#';
            } 
            else {  // <--- ВОТ ЭТОТ ELSE БЫЛ ПРОПУЩЕН!
                // Внутри — случайные препятствия или пустота
                int chance = rand() % 100; 
                if (chance < 15) {
                    map[y][x] = 'T'; // 15% шанс, что будет дерево
                } else if (chance < 25) {
                    map[y][x] = 'R'; // еще 10% шанс, что будет камень
                } else {
                    map[y][x] = '.'; // Остальное — свободная земля
                }
            }
        } // <--- Закрывает внутренний цикл for (x)
    } // <--- Закрывает внешний цикл for (y)
	    while (1) {
        int dx = rand() % (width - 2) + 1;
        int dy = rand() % (height - 2) + 1;
        if (map[dy][dx] == '.') {
            map[dy][dx] = 'D';
            break;
        }
    }

    return map;
} // <--- Закрывает функцию

void free_map(char **map, int height) {
    if (map == NULL) return;

    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}


void spawn_hero(char **map, int width, int height, int *hero_x, int *hero_y) {
    while (1) {
        int x = rand() % (width - 2) + 1;
        int y = rand() % (height - 2) + 1;
        
        if (map[y][x] == '.') {
            map[y][x] = 'U'; // Генерируем лестницу вверх
            *hero_x = x;
            *hero_y = y;
            break;
        }
    }
}

void print_map(char **map, int width, int height, int hero_x, int hero_y) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Вычисляем расстояние от героя до клетки
            int dx = x - hero_x;
            int dy = y - hero_y;
            
            // Если расстояние в пределах радиуса видимости
            if (dx*dx + dy*dy <= FOG_RADIUS * FOG_RADIUS) {
                if (x == hero_x && y == hero_y) {
                    printf("@ "); // Рисуем героя
                } else {
                    printf("%c ", map[y][x]); // Рисуем карту
                }
            } else {
                printf("? "); // Туман войны
            }
        }
        printf("\n");
    }
}

void move_hero(char **map, int width, int height, int *hero_x, int *hero_y, char dir) {
    int new_x = *hero_x;
    int new_y = *hero_y;

    if (dir == 'W' || dir == 'w') new_y--;
    else if (dir == 'S' || dir == 's') new_y++;
    else if (dir == 'A' || dir == 'a') new_x--;
    else if (dir == 'D' || dir == 'd') new_x++;
    else return;

    if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height) return;

    // Теперь можно ходить по '.', 'U' и 'D'
    if (map[new_y][new_x] == '.' || map[new_y][new_x] == 'U' || map[new_y][new_x] == 'D') {
        *hero_x = new_x;
        *hero_y = new_y;
    } else {
        printf("Туда нельзя идти! Там препятствие.\n");
    }
}

void play_adventure(void) {
    int depth = 1; // Счетчик глубины
    int width = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
    int height = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;

    char **map = create_map(width, height);
    if (map == NULL) return;

    int hero_x, hero_y;
    spawn_hero(map, width, height, &hero_x, &hero_y);

    char dir;
    while (1) {
        printf("\n--- Подземелье (Глубина: %d, Размер: %dx%d) ---\n", depth, width, height);
        print_map(map, width, height, hero_x, hero_y);
        printf("\nДвижение (W/A/S/D), Q - выход (только на 'U'): ");
        
        if (scanf(" %c", &dir) != 1) {
            while (getchar() != '\n');
            continue;
        }

        // Обработка выхода
        if (dir == 'Q' || dir == 'q') {
            if (map[hero_y][hero_x] == 'U') {
                printf("Вы благополучно сбежали на поверхность с %d этажа!\n", depth);
                break;
            } else {
                printf("Выход на поверхность доступен только с лестницы 'U'!\n");
                continue;
            }
        }

        // Двигаем героя
        move_hero(map, width, height, &hero_x, &hero_y, dir);

        // Проверка на спуск вниз
        if (map[hero_y][hero_x] == 'D') {
            printf("\nВы спустились на следующий этаж!\n");
            // Удаляем старую карту
            free_map(map, height);
            depth++;

            // Генерируем новую карту
            width = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
            height = rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1) + MIN_MAP_SIZE;
            map = create_map(width, height);
            if (map == NULL) return;
            
            // Спавним героя на новой карте
            spawn_hero(map, width, height, &hero_x, &hero_y);
        }
    }

    // Очистка памяти при выходе
    free_map(map, height);
}

int main(void) {
    int current_day  = START_DAY;
    int current_hour = START_HOUR;

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    load_item_names(item_names);

	char farmer_name[MAX_FARMER_NAME_LEN];
	read_farmer_name(farmer_name, MAX_FARMER_NAME_LEN);

    int inventory[INVENTORY_SIZE] = {0};
    inventory[0] = 1;  
    inventory[1] = 2;   
    inventory[2] = 3;   
    inventory[3] = 0;   
    inventory[4] = 4;   
    inventory[5] = 1;  
    inventory[6] = 3;   
    inventory[7] = 5;   
    inventory[8] = 0;   
    inventory[9] = 4;   

    int choice;

    while (1) {
        printf("\n %s THE GAME\n", farmer_name);
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Быстрый доступ\n");
        printf("[7] Поиск предмета по названию\n");
        printf("[8] Записать состояние в дневник\n");
        printf("[9] Расшифровать старые записи (настройки)\n");
        printf("[10] Путешествовать\n");
        printf("==================================\n");

        choice = read_int("Ваш выбор: ", 0, 10);

        switch (choice) {
        case 0:
            printf("Выход из игры.\n");
        return 0;

        case 1:
            printf("Текущее время: День %d, %02d:00\n",
               current_day, current_hour);
        break;

        case 2: {
                int hours = read_int("Сколько часов хотите поработать? ", 0, 1000000);
                current_hour += hours;
                current_day  += current_hour / HOURS_PER_DAY;
                current_hour  = current_hour % HOURS_PER_DAY;

                printf("Прошло %d ч. Текущее время: День %d, %02d:00\n",
                       hours, current_day, current_hour);
                break;
            }

			case 3:
                printf("Инвентарь:\n");
                print_inventory(inventory, INVENTORY_SIZE, item_names);
                break;
			
            case 4: {
                printf("Сейчас в инвентаре:\n");
                print_inventory(inventory, INVENTORY_SIZE, item_names);

                int slot = read_int("Введите индекс слота (0-9): ", 0, INVENTORY_SIZE - 1);
                int id   = read_int("Введите ID предмета (0-9): ", MIN_ID, MAX_ID);

                inventory[slot] = id;
                printf("В слот %d положили [%d] - %s.\n", slot, id, item_names[id]);
                break;
            }

            case 5: {
                printf("Сейчас в инвентаре:\n");
                print_inventory(inventory, INVENTORY_SIZE, item_names);

                int slot = read_int("Введите индекс слота для очистки (0-9): ",
                                    0, INVENTORY_SIZE - 1);

                if (inventory[slot] == 0) {
                    printf("Слот %d уже пуст.\n", slot);
                } else {
                    printf("Из слота %d выброшен предмет [%d] - %s.\n",
                           slot, inventory[slot], item_names[inventory[slot]]);
                    inventory[slot] = 0;
                }
                break;
            }

            case 6: {
                printf("Инвентарь до:\n");
                print_inventory(inventory, INVENTORY_SIZE, item_names);

                int id = read_int("Введите ID предмета для быстрого доступа (1-9): ",
                                  MIN_ID + 1, MAX_ID);

                int found = -1;
                for (int i = 0; i < INVENTORY_SIZE; i++) {
                    if (inventory[i] == id) {
                        found = i;
                        break;
                    }
                }

                if (found == -1) {
                    printf("Предмет [%d] - %s не найден в инвентаре.\n",
                           id, item_names[id]);
                } else if (found == 0) {
                    printf("Предмет [%d] - %s уже находится в слоте 0.\n",
                           id, item_names[id]);
                } else {
                    int temp = inventory[0];
                    inventory[0] = inventory[found];
                    inventory[found] = temp;

                    printf("Обменяли слот 0 и слот %d.\n", found);
                    printf("Инвентарь после:\n");
                    print_inventory(inventory, INVENTORY_SIZE, item_names);
                }
                break; 
            }
            case 7:
                search_item_by_name(inventory, INVENTORY_SIZE, item_names);
                break;
            case 8: {
                printf("Сохранение в дневник...\n");
                
                write_diary_entry(stdout, farmer_name, current_day, current_hour, 
                                  inventory, INVENTORY_SIZE, item_names);
                                  
                FILE *file = fopen("diary.txt", "a");
               
                if (file == NULL) {
                    printf("Ошибка: не удалось открыть файл diary.txt для записи!\n");
                } else {
                    write_diary_entry(file, farmer_name, current_day, current_hour, 
                                      inventory, INVENTORY_SIZE, item_names);
                    
                    fclose(file);
                    printf("Успешно сохранено в diary.txt!\n");
                }
                break;
            }
             case 9:
                parse_ini_file();
                break;
             case 10:
                play_adventure();
                break;   
        }
    

    }

    return 0;
}

