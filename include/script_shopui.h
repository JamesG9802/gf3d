#pragma once

#include "dice.h"
#include "entity.h"
#include "script.h"

typedef enum ShopUIState {
	ShopUIState_HIDDEN,
	ShopUIState_VISIBLE,
} ShopUIState;

typedef struct ShopUIData {
	ShopUIState state;
	int daysLocked;
	Dice diceInventory[4];
} ShopUIData;

/// <summary>
/// Creates a new inventory ui data
/// </summary>
/// <returns></returns>
ShopUIData* script_shopui_newdata();

/// <summary>
/// Frees a new inventory ui data
/// </summary>
void script_shopui_freedata(Script* script);

/// <summary>
/// Toggles the shop ui between different states.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_shopui_toggle(Entity* entity, Script* script);

/// <summary>
/// Hides the shop ui.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_shopui_hide(Entity* entity, Script* script);

/// <summary>
/// Updates the shop ui for a new day.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_shopui_newday(Entity* entity, Script* script);
