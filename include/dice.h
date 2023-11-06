#pragma once

#include "gfc_list.h"
#include "gfc_types.h"

#include "entity.h"

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
	DiceValue* sideValues;
	double* sideWeights;
	int maxLifespan;
	int manaCost;
} Dice;

/// <summary>
/// Creates a new dice object.
/// </summary>
/// <returns></returns>
Dice* dice_new(Bool isSeed, int age, int sideCount, DiceValue* sideValues, double* sideWeights, int maxLifespan, int manacost);

/// <summary>
/// Frees a dice object.
/// </summary>
/// <param name="dice"></param>
void dice_free(Dice* dice);

/// <summary>
/// Create a random dice as a reward;
/// </summary>
/// <param name="manacost"></param>
/// <returns></returns>
Dice* dice_seed_reward(int manacost);

/// <summary>
/// Randomly mutates and sets up a dice seed to become a real dice.
/// </summary>
/// <param name="dice"></param>
void dice_harvest(Dice* dice);

/// <summary>
/// Roll the dice and activate a random effect
/// </summary>
/// <param name="dice"></param>
void dice_activate_effect(Dice* dice);

/// <summary>
/// Sets the dice's sides as the texture for the entity.
/// </summary>
/// <param name="texture"></param>
void dice_to_texture(Dice* dice, Entity* entity);

/// <summary>
/// Sets the dice's sides as the sprite for the entity
/// </summary>
/// <param name="dice"></param>
/// <param name="entity"></param>
void dice_to_ui(Dice* dice, Entity* entity);

/// <summary>
/// Sets up to four dice's information as the sprite for the entity.
/// </summary>
void dice_to_ui_simplified(List* dices, int diceIndex, int selectedDiceIndex, Entity* entity);
