#pragma once

#include "gfc_list.h"
#include "gfc_types.h"

#include "dicevalue.h"

/// <summary>
/// A dice object that is grown and used in combat.
/// </summary>
typedef struct Dice {
	Bool isSeed;
	//	Time until destruction
	int age;
	//	Breedable Traits
	int sideCount;
	List* sideValues;
	double sideWeights;
	int maxLifespan;
	int manaCost;
} Dice;
