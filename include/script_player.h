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
/// Destroys a playerdata;
/// </summary>
void script_player_freeplayerdata(Script* script);

/// <summary>
/// Returns the current player.
/// </summary>
/// <returns></returns>
Entity* script_player_getplayer();

/// <summary>
/// Returns the current player's player data.
/// </summary>
/// <returns></returns>
PlayerData* script_player_getplayerdata();
