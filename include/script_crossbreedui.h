#pragma once

#include "entity.h"
#include "script.h"

typedef enum CrossbreedUIState {
	CrossbreedUIState_HIDDEN,
	CrossbreedUIState_VISIBLE,
} CrossbreedUIState;

typedef struct CrossbreedUIData {
	CrossbreedUIState state;
	int leftDiceIndex;
	int rightDiceIndex;
} CrossbreedUIData;

/// <summary>
/// Creates a new crossbreed ui data
/// </summary>
/// <returns></returns>
CrossbreedUIData* script_crossbreedui_newdata();

/// <summary>
/// Frees a new crossnbreed ui data
/// </summary>
void script_crossbreedui_freedata(Script* script);

/// <summary>
/// Toggles the crossbreed ui between different states.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_crossbreedui_toggle(Entity* entity, Script* script);

/// <summary>
/// Hides the crossbreed ui.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
void script_crossbreedui_hide(Entity* entity, Script* script);
