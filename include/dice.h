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
	List* sideWeights;
	int maxLifespan;
	int manaCost;
} Dice;

/// <summary>
/// Creates a new dice object.
/// </summary>
/// <returns></returns>
Dice* dice_new(Bool isSeed, int age, int sideCount, List* sideValues, List* sideWeights, int maxLifespan, int manacost);

/// <summary>
/// Frees a dice object.
/// </summary>
/// <param name="dice"></param>
void dice_free(Dice* dice);
