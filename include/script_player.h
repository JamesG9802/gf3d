#pragma once

#include "gfc_list.h"

#include "entity.h"

#include "inventory.h"

typedef struct PlayerData {
	Inventory* inventory;
} PlayerData;

/// <summary>
/// Creates a new PlayerData.
/// </summary>
/// <returns></returns>
PlayerData script_player_newplayerdata();

/// <summary>
/// Returns the current player.
/// </summary>
/// <returns></returns>
Entity* script_player_getplayer();
