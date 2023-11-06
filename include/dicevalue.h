#pragma once
#include "gf2d_sprite.h"

typedef enum DiceValueType {
	Mana,
	Fire,
	Heart
} DiceValueType;

typedef struct DiceValue {
	DiceValueType type;
	int value;
} DiceValue;

/// <summary>
/// Creates a new DiceValue
/// </summary>
/// <returns></returns>
DiceValue dicevalue_new(DiceValueType type, int value);

/// <summary>
/// Activates a dice's effect
/// </summary>
/// <param name="dvalue"></param>
void dicevalue_activate_effect(DiceValue dvalue);

/// <summary>
/// Get the associate sprite for the dice value sprite.
/// </summary>
/// <returns></returns>
SDL_Surface* dicevalue_getsurface(DiceValue dvalue);
