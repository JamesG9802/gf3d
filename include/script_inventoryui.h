#pragma once
#include "dice.h"

typedef enum InventoryUIState {
	HIDDEN,
	START,
	VIEWDICE,
	SEEDPROMPT	//	used when the inventory is specifically showing just the seeds
} InventoryUIState;

typedef enum DiceInventoryType {
	SEEDS,
	INVENTORY,
	LOADOUT
} DiceInventoryType;

typedef struct InventoryUIData {
	InventoryUIState state;

	/// <summary>
	/// The current dice inventory from the player we are looking at.
	/// </summary>
	DiceInventoryType currentType;
	
	/// <summary>
	/// The current index in the dice inventory that we are looking at.
	/// </summary>
	int diceIndex;

	/// <summary>
	/// Similar to diceIndex, but diceIndex represents the page into the dice inventory when in simplified mode.
	/// selectedDiceIndex will always show the correct Dice.
	/// </summary>
	int selectedDiceIndex;
} InventoryUIData;

/// <summary>
/// Creates a new inventory ui data
/// </summary>
/// <returns></returns>
InventoryUIData* script_inventoryui_newdata();

/// <summary>
/// Frees a new inventory ui data
/// </summary>
void script_inventoryui_freedata(Script* script);

/// <summary>
/// Shows the inventory ui.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_inventoryui_toggle(Entity* entity, Script* script);

/// <summary>
/// Shows the inventory ui specifically so the player can choose a seed
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_inventoryui_seedprompt(Entity* entity, Script* script);

/// <summary>
/// Gets the currently selected dice.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
Dice* script_inventoryui_getselecteddice(Entity* entity, Script* script);

/// <summary>
/// Removes the currently selected dice
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_inventoryui_deleteselecteddice(Entity* entity, Script* script);
