#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "player.h"
#include "adventure.h"

#define START_DAY      1
#define START_HOUR     8
#define HOURS_PER_DAY  24
#define WAKE_UP_HOUR   8

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
        int choice = read_int("Действие: ", 0, 3);
        
        if (choice == 0) break;
        if (choice == 1) printf("Сейчас День %d, %02d:00\n", *day, *hour);
        else if (choice == 2) {
            int max_wait = hours_until_2am(*hour);
            if (max_wait == 0) { printf("Сейчас 02:00. Пора спать!\n"); continue; }
            int hours = read_int("Сколько часов ждать? ", 1, max_wait);
            *hour += hours; *day += *hour / HOURS_PER_DAY; *hour %= HOURS_PER_DAY;
            printf("Время: День %d, %02d:00\n", *day, *hour);
        }
        else if (choice == 3) {
            if (*hour < SLEEP_BOUNDARY || *hour >= WAKE_UP_HOUR) *day += 1;
            *hour = WAKE_UP_HOUR;
            printf("Утро: День %d, %02d:00\n", *day, *hour);
            if (p != NULL) {
                p->hp = p->max_hp; p->stamina = p->max_stamina;
                printf("Здоровье и выносливость восстановлены!\n");
            }
        }
    }
}

void print_menu(const struct Player *player) {
    printf("\n %s THE GAME\n", player ? player->name : "(персонаж не создан)");
    printf("[0] Выход\n[1] Создать / пересоздать\n[2] Карточка\n[3] Время и отдых\n");
    printf("[4] Рюкзак\n[5] Положить предмет\n[6] Выбросить предмет\n[7] Быстрый доступ\n");
    printf("[8] Поиск предмета\n[9] Дневник\n[10] Расшифровать настройки\n[11] Купить подсумок\n[12] Путешествовать\n");
    printf("==================================\n");
}

int main(void) {
    int current_day  = START_DAY;
    int current_hour = START_HOUR;
    srand((unsigned)time(NULL));
    
    struct Item items[MAX_ITEMS];
    load_items(items);
    struct Player *player = NULL;

    while (1) {
        print_menu(player);
        int choice = read_int("Ваш выбор: ", 0, 12);

        switch ((enum MenuAction)choice) {
        case MENU_EXIT:
            printf("Выход из игры.\n");
            player_destroy(player);
            return 0;

        case MENU_CREATE_PLAYER: {
            if (player != NULL) {
                if (!read_int("Удалить персонажа? (1-да, 0-нет): ", 0, 1)) break;
                player_destroy(player); player = NULL;
            }
            player = player_create();
            if (player) player_update_weight(player, items);
            break;
        }

        case MENU_PLAYER_CARD:
            if (player) player_print_card(player, current_day, current_hour, items);
            else printf("Сначала создайте персонажа!\n");
            break;

        case MENU_TIME: menu_time(player, &current_day, &current_hour); break;

        case MENU_INVENTORY:
            if (player) player_print_inventory(player, items);
            else printf("Сначала создайте персонажа!\n");
            break;

        case MENU_PUT_ITEM: {
            if (!player) { printf("Сначала создайте персонажа!\n"); break; }
            player_print_inventory(player, items);
            int slot = read_int("Слот (0-9): ", 0, MAX_ITEMS - 1);
            int id   = read_int("ID предмета (0-9): ", MIN_ID, MAX_ID);
            player->backpack.slots[slot] = id;
            player_update_weight(player, items);
            printf("В слот %d положили %s.\n", slot, items[id].name);
            break;
        }

        case MENU_DROP_ITEM: {
            if (!player) { printf("Сначала создайте персонажа!\n"); break; }
            player_print_inventory(player, items);
            int slot = read_int("Слот для очистки: ", 0, MAX_ITEMS - 1);
            if (player->backpack.slots[slot] == 0) printf("Слот уже пуст.\n");
            else {
                printf("Выброшен %s.\n", items[player->backpack.slots[slot]].name);
                player->backpack.slots[slot] = 0;
                player_update_weight(player, items);
            }
            break;
        }

        case MENU_QUICK_ACCESS: {
            if (!player) { printf("Сначала создайте персонажа!\n"); break; }
            player_print_inventory(player, items);
            int id = read_int("ID предмета (1-9): ", MIN_ID + 1, MAX_ID);
            int found = -1;
            for (int i = 0; i < MAX_ITEMS; i++) if (player->backpack.slots[i] == id) { found = i; break; }
            if (found == -1) printf("Нет такого предмета.\n");
            else if (found == 0) printf("Уже в слоте 0.\n");
            else {
                int temp = player->backpack.slots[0];
                player->backpack.slots[0] = player->backpack.slots[found];
                player->backpack.slots[found] = temp;
                player_update_weight(player, items);
                printf("Обменяли слоты 0 и %d.\n", found);
            }
            break;
        }

        case MENU_SEARCH_ITEM:
            if (player) player_search_item(player, items);
            else printf("Сначала создайте персонажа!\n");
            break;

        case MENU_DIARY: {
            if (!player) { printf("Сначала создайте персонажа!\n"); break; }
            player_write_diary(stdout, player, current_day, current_hour, items);
            FILE *file = fopen("diary.txt", "a");
            if (file) { player_write_diary(file, player, current_day, current_hour, items); fclose(file); }
            break;
        }

        case MENU_PARSE_INI: parse_ini_file(); break;
        
        case MENU_SHOP:
            if (player) player_buy_upgrade(player, items);
            else printf("Сначала создайте персонажа!\n");
            break;

        case MENU_ADVENTURE:
            if (!player) { printf("Сначала создайте персонажа!\n"); break; }
            if (player->stamina <= 0) { printf("Нет сил на приключения. Отдохните!\n"); break; }
            if (current_hour == SLEEP_BOUNDARY) { printf("02:00 ночи. Фермер должен спать!\n"); break; }
            play_adventure(player, &current_day, &current_hour);
            break;
        }
    }
    return 0;
}