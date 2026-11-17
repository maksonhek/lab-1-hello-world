#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

#define HP_MAX_DEFAULT        100
#define STAMINA_MAX_DEFAULT   50
#define GOLD_START            0
#define BACKPACK_BASE_CAP     20
#define UPGRADE_AMOUNT        10
#define UPGRADE_COST          50

struct Backpack {
    int slots[MAX_ITEMS];
    int current_weight;
    int max_weight;
    enum LoadState state;
};

struct Player {
    char name[MAX_FARMER_NAME_LEN];
    int  hp;
    int  max_hp;
    int  stamina;
    int  max_stamina;
    int  gold;
    int  x;
    int  y;
    struct Backpack backpack;
};

struct Player *player_create(void);
void player_destroy(struct Player *p);
void player_update_weight(struct Player *p, const struct Item items[]);
void player_print_card(const struct Player *p, int day, int hour, const struct Item items[]);
void player_print_inventory(const struct Player *p, const struct Item items[]);
void player_search_item(const struct Player *p, const struct Item items[]);
void player_write_diary(FILE *out, const struct Player *p, int day, int hour, const struct Item items[]);
void player_buy_upgrade(struct Player *p, const struct Item items[]);

#endif 
