#pragma once

#include "entity.h"
#include "script.h"

typedef enum MonsterState {
	MONSTER_IDLE,
	MONSTER_CHASE,
	MONSTER_COMBAT
} MonsterState;

typedef enum MonsterController {
	MONSTER1,
	MONSTER2,
	MONSTER3,
	MONSTER4,
	MONSTER5,
	BOSS1
} MonsterController;

typedef struct MonsterData {
	MonsterState state;
	MonsterController controller;
	int currentHealth;
	int maxHealth;
	double timeDelta;
	int cooldown;
} MonsterData;

/// <summary>
/// Creates a new PlayerData.
/// </summary>
/// <returns></returns>
MonsterData* script_monster_newmonsterdata();

/// <summary>
/// Destroys a playerdata;
/// </summary>
void script_monster_freemonsterdata(Script* script);

/// <summary>
/// Get the monster battling the player
/// </summary>
/// <returns></returns>
Entity* script_monster_getbattlingmonster();

/// <summary>
/// Get the monster data of the monster battling the player
/// </summary>
/// <returns></returns>
MonsterData* script_monster_getbattlingmonsterdata();
