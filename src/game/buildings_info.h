/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#ifndef GAME_BUILDINGS_INFO_H
#define GAME_BUILDINGS_INFO_H

#include <base/vmath.h>
#define MAX_INT 2147483647

enum {
	B_REACTOR=0,
	B_SPAWN,
	B_AMMO1,
	B_HEALTH,
	
	B_REPEATER,
	B_TURRET1,
	B_SHIELD,
	B_AMMO2,
	
	B_TELEPORT,
	B_ARMOR,
	B_AMMO3,
	B_TURRET2
};

enum {
	EFFECT_FALLOUT=0,
	EFFECT_HEALING,
	EFFECT_RAMPAGE,
	EFFECT_FREEZE,
	EFFECT_BOMB
};

typedef struct _building_desc {
	const char *name;
	const char *desc;
	int price;
	int maxhealth;
	float width;
	float height;
} buildings_info_t;

typedef struct _ents_desc {
	const char *name;
	const char *desc;
	int price;
	int tech;
} ents_info_t;

extern buildings_info_t buildings_info[];
extern ents_info_t ents_info[];

#endif
