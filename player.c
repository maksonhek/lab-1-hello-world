#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"

struct Player *player_create(void) {
    struct Player *p = malloc(sizeof(struct Player));
    if (!p) { printf("Ошибка: не хватило памяти!\n"); return NULL; }
    read_farmer_name(p->name, MAX_FARMER_NAME_LEN);
    p->max_hp = HP_MAX_DEFAULT; p->hp = p->max_hp;
    p->max_stamina = STAMINA_MAX_DEFAULT; p->stamina = p->max_stamina;
    p->gold = GOLD_START; p->x = 0; p->y = 0;
    p->backpack.max_weight = BACKPACK_BASE_CAP;
    p->backpack.current_weight = 0;
    p->backpack.state = LOAD_LIGHT;
    
    for (int i = 0; i < MAX_ITEMS; i++) p->backpack.slots[i] = 0;
    p->backpack.slots[0] = 1; p->backpack.slots[1] = 2; p->backpack.slots[2] = 3;
    p->backpack.slots[3] = 0; p->backpack.slots[4] = 4; p->backpack.slots[5] = 1;
    p->backpack.slots[6] = 3; p->backpack.slots[7] = 5; p->backpack.slots[8] = 0; p->backpack.slots[9] = 4;
    
    printf("Персонаж '%s' создан. Адрес структуры: %p\n", p->name, (void *)p);
    return p;
}

void player_destroy(struct Player *p) {
    if (!p) return;
    printf("Память структуры по адресу %p освобождена\n", (void *)p);
    free(p);
}

void player_update_weight(struct Player *p, const struct Item items[]) {
    int total = 0;
    for (int i = 0; i < MAX_ITEMS; i++) total += items[p->backpack.slots[i]].weight;
    p->backpack.current_weight = total;
    
    if (total > p->backpack.max_weight) p->backpack.state = LOAD_OVERLOAD;
    else if (total > p->backpack.max_weight / 2) p->backpack.state = LOAD_NORMAL;
    else p->backpack.state = LOAD_LIGHT;
}

void player_print_inventory(const struct Player *p, const struct Item items[]) {
    const char *state_str[] = {"Налегке", "Норма", "Перегруз"};
    printf("--- Рюкзак (Вес: %d/%d - %s) ---\n", p->backpack.current_weight, p->backpack.max_weight, state_str[p->backpack.state]);
    for (int i = 0; i < MAX_ITEMS; i++) {
        int id = p->backpack.slots[i];
        printf("Слот %d: [%d] %s (вес: %d)\n", i, id, items[id].name, items[id].weight);
    }
}

void player_buy_upgrade(struct Player *p, const struct Item items[]) {
    if (p->gold >= UPGRADE_COST) {
        p->gold -= UPGRADE_COST;
        p->backpack.max_weight += UPGRADE_AMOUNT;
        player_update_weight(p, items);
        printf("Подсумок куплен! Грузоподъемность: %d.\n", p->backpack.max_weight);
    } else {
        printf("Недостаточно золота! Нужно %d, у вас %d.\n", UPGRADE_COST, p->gold);
    }
}

void player_print_card(const struct Player *p, int day, int hour, const struct Item items[]) {
    time_t now = time(NULL); struct tm *tm_now = localtime(&now);
    char date_buf[32];
    if (tm_now) strftime(date_buf, sizeof(date_buf), "%d.%m.%Y", tm_now);
    else strcpy(date_buf, "(недоступно)");

    const char *state_str[] = {"Налегке", "Норма", "Перегруз"};
    printf("\n=============== КАРТОЧКА ПЕРСОНАЖА ===============\n");
    printf("  Имя:        %s\n  HP:         %d/%d\n", p->name, p->hp, p->max_hp);
    printf("  Stamina:    %d/%d\n  Золото:     %d\n", p->stamina, p->max_stamina, p->gold);
    printf("  Позиция:    (%d, %d)\n", p->x, p->y);
    printf("  Дата:       %s | Время: День %d, %02d:00\n", date_buf, day, hour);
    printf("  Вес:        %d/%d (%s)\n", p->backpack.current_weight, p->backpack.max_weight, state_str[p->backpack.state]);
    printf("  Инвентарь:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        int id = p->backpack.slots[i];
        printf("    Слот %d: [%d] - %s\n", i, id, items[id].name);
    }
    printf("====================================================\n");
}

void player_search_item(const struct Player *p, const struct Item items[]) {
    char search_name[MAX_NAME_LEN];
    read_string("Введите название для поиска: ", search_name, MAX_NAME_LEN);
    if (strlen(search_name) == 0) { printf("Название не может быть пустым.\n"); return; }
    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) if (strcmp(search_name, items[i].name) == 0) { found_id = i; break; }
    if (found_id == -1) { printf("Предмет '%s' не найден.\n", search_name); return; }
    
    int count = 0;
    printf("Предмет '%s' (ID %d) в слотах: ", search_name, found_id);
    for (int i = 0; i < MAX_ITEMS; i++) if (p->backpack.slots[i] == found_id) { printf("%d ", i); count++; }
    if (count == 0) printf("\nНет в рюкзаке.\n");
    else printf("\nВсего: %d шт.\n", count);
}

void player_write_diary(FILE *out, const struct Player *p, int day, int hour, const struct Item items[]) {
    fprintf(out, "\n==================================\n=== Дневник ===\n");
    fprintf(out, "Фермер: %s | Время: День %d, %02d:00\n", p->name, day, hour);
    fprintf(out, "Рюкзак (Вес: %d/%d):\n", p->backpack.current_weight, p->backpack.max_weight);
    for (int i = 0; i < MAX_ITEMS; i++) {
        int id = p->backpack.slots[i];
        fprintf(out, "  Слот %d: [%d] - %s\n", i, id, items[id].name);
    }
    fprintf(out, "==================================\n");
}
