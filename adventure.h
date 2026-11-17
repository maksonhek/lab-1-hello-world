#ifndef ADVENTURE_H
#define ADVENTURE_H

#include "player.h"
#include "map.h"

#define BASE_FLOOR_GOLD 15
#define SLEEP_BOUNDARY 2

void play_adventure(struct Player *p, int *day, int *hour);

#endif 
