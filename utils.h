#ifndef UTILS_H
#define UTILS_H

#define MAX_ITEMS            10
#define MAX_NAME_LEN         32
#define MAX_FARMER_NAME_LEN  32
#define MAX_LINE_LEN         128
#define MIN_ID               0
#define MAX_ID               (MAX_ITEMS - 1)

enum LoadState {
    LOAD_LIGHT = 0,    
    LOAD_NORMAL,        
    LOAD_OVERLOAD       
};

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
    MENU_SHOP,
    MENU_ADVENTURE
};

struct Item {
    char name[MAX_NAME_LEN];
    int  weight;
};

int read_int(const char *prompt, int min, int max);
void read_string(const char *prompt, char *buffer, int size);
void read_farmer_name(char *name, int size);
void load_items(struct Item items[]);
void parse_ini_file(void);

#endif
