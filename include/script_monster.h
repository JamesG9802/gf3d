#pragma once

#include "entity.h"
#include "script.h"

typedef enum MonsterState {
	MONSTER_IDLE,
	MONSTER_CHASE,
	MONSTER_COMBAT
} MonsterState;

typedef struct MonsterData {
	MonsterState state;
	double timeDelta;
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