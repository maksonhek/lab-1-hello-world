#include <stdio.h>

#define MIN_ID 0
#define MAX_ID 9
#define INVENTORY_SIZE 10   
#define START_DAY      1     
#define START_HOUR     8    
#define HOURS_PER_DAY  24    

const char *item_name(int id) {
    switch (id) {
        case 0: return "Пусто";
        case 1: return "Дерево";
        case 2: return "Камень";
        case 3: return "Семена";
        case 4: return "Железо";
        case 5: return "Уголь";
        case 6: return "Факел";
        case 7: return "Вода";
        case 8: return "Еда";
        case 9: return "Инструмент";
        default: return "Неизвестно";
    }
}

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
        }
    } while (ok == 0);

    return value;
}

int main(void) {
    int current_day  = START_DAY;
    int current_hour = START_HOUR;

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
        printf("\n THE GAME\n");
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Быстрый доступ\n");
        printf("==================================\n");
        printf("Ваш выбор: ");

        choice = read_int("Ваш выбор: ", 0, 6);

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
                for (int i = 0; i < INVENTORY_SIZE; i++) {
                printf("Слот %d: [%d]", i, inventory[i]);
                if (inventory[i] != 0) {
                printf(" (%s)", item_name(inventory[i]));
                }
                printf("\n");
            }
            break;
            case 4: {
            printf("Сейчас в инвентаре:\n");
            for (int i = 0; i < INVENTORY_SIZE; i++) {
                printf("Слот %d: [%d]", i, inventory[i]);
                if (inventory[i] != 0) {
                  printf(" (%s)", item_name(inventory[i]));
             }
                printf("\n");
            }

            int slot = read_int("Введите индекс слота (0-9): ", 0, INVENTORY_SIZE - 1);
            int id   = read_int("Введите ID предмета (0-9): ", MIN_ID, MAX_ID);

            inventory[slot] = id;
            printf("В слот %d положили [%d] (%s).\n", slot, id, item_name(id));
            break;
            }
            case 5: {
            printf("Сейчас в инвентаре:\n");
            for (int i = 0; i < INVENTORY_SIZE; i++) {
                printf("Слот %d: [%d]", i, inventory[i]);
                if (inventory[i] != 0) {
                 printf(" (%s)", item_name(inventory[i]));
             }
             printf("\n"); 
            }

            int slot = read_int("Введите индекс слота для очистки (0-9): ",
                                0, INVENTORY_SIZE - 1);

            if (inventory[slot] == 0) {
                printf("Слот %d уже пуст.\n", slot);
            } else {
                printf("Из слота %d выброшен предмет [%d] (%s).\n",
                       slot, inventory[slot], item_name(inventory[slot]));
                inventory[slot] = 0;
            }
            break;
            }
            case 6:{
                printf("Инвентарь до:\n");
                for (int i = 0; i < INVENTORY_SIZE; i++) {
                    printf("Слот %d: [%d]", i, inventory[i]);
                    if (inventory[i] != 0) {
                        printf(" (%s)", item_name(inventory[i]));
                    }
                    printf("\n");
                }

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
                    printf("Предмет [%d] (%s) не найден в инвентаре.\n",
                           id, item_name(id));
                } else if (found == 0) {
                    printf("Предмет [%d] (%s) уже находится в слоте 0.\n",
                           id, item_name(id));
                } else {
                    int temp = inventory[0];
                    inventory[0] = inventory[found];
                    inventory[found] = temp;

                    printf("Обменяли слот 0 и слот %d.\n", found);
                    printf("Инвентарь после:\n");
                    for (int i = 0; i < INVENTORY_SIZE; i++) {
                        printf("Слот %d: [%d]", i, inventory[i]);
                        if (inventory[i] != 0) {
                            printf(" (%s)", item_name(inventory[i]));
                        }
                        printf("\n");
                    }
                }
                break;
            }

            default:
                printf("Нет такого пункта меню.\n");
                break;
        }
    

    }

    return 0;
}

