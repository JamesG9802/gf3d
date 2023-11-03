#pragma once

#include "gfc_list.h"

#include "dice.h"

/// <summary>
/// Contains all items the player has
/// </summary>
typedef struct Inventory {
	/// <summary>
	/// Dice the player has available to plant
	/// </summary>
	List* diceSeeds;
	
	/// <summary>
	/// Dice the player has in their inventory.
	/// </summary>
	List* diceInventory;

	/// <summary>
	/// Dice the player has equipped
	/// </summary>
	List* diceLoadout;
} Inventory;

/// <summary>
/// Make a new inventory.
/// </summary>
/// <returns></returns>
Inventory* inventory_new();

/// <summary>
/// Safely free an inventory
/// </summary>
/// <param name="inventory"></param>
void inventory_free(Inventory* inventory);
