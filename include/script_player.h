#pragma once

#include "gfc_list.h"
#include "gfc_audio.h"

#include "entity.h"

#include "inventory.h"

typedef struct PlayerData {
	Inventory* inventory;
	Entity* diceEntity;	//	entity representing the dice
	Sound* soundDice;		//	sound effect for dice
	int selectedDiceIndex;	//	current dice selected from loadout
	int currentHealth;
	int maxHealth;
	int currentMana;
	int maxMana;
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
