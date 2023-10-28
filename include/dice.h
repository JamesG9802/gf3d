#pragma once
#include "dicevalue.h"
/// <summary>
/// A dice object that is grown and used in combat.
/// </summary>
typedef struct Dice {
	int age;
	//	Breedable Traits
	int sideCount;
	DiceValue* sideValues;
	double sideWeights;
	int maxLifespan;
	int manaCost;
} Dice;
