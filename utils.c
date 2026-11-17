#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

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

void read_string(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    if (strchr(buffer, '\n') == NULL) while (getchar() != '\n');
    buffer[strcspn(buffer, "\n")] = '\0';
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

void load_items(struct Item items[]) {
    int def_weights[MAX_ITEMS] = {0, 5, 15, 1, 20, 10, 2, 3, 1, 4};
    char *def_names[MAX_ITEMS] = {"Пусто", "Дерево", "Камень", "Семена", "Железо", "Уголь", "Факел", "Вода", "Еда", "Инструмент"};
    
    for (int i = 0; i < MAX_ITEMS; i++) {
        strcpy(items[i].name, def_names[i]);
        items[i].weight = def_weights[i];
    }

    FILE *file = fopen("items.txt", "r");
    if (file == NULL) {
        printf("Предупреждение: файл items.txt не найден. Используются стандартные предметы.\n");
        return;
    }

    char line[MAX_LINE_LEN]; int id; char name[MAX_NAME_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d %31[^\n]", &id, name) == 2) {
            if (id >= 0 && id < MAX_ITEMS) strcpy(items[id].name, name);
        }
    }
    fclose(file);
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
    fprintf(stdout, "\nУспешно считано: %d\nОтброшено строк: %d\n", loaded, skipped);
    fprintf(out_file, "\nУспешно считано: %d\nОтброшено строк: %d\n", loaded, skipped);
    fclose(in_file); fclose(out_file);
    printf("\nРезультат сохранен в output.txt\n");
}
