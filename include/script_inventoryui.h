#pragma once

typedef enum InventoryUIState {
	HIDDEN,
	START,
	VIEWDICE,
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


