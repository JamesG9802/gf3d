#pragma once

typedef enum InventoryUIState {
	HIDDEN,
	START,
	VIEWDICE,
} InventoryUIState;

typedef struct InventoryUIData {
	InventoryUIState state;
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


