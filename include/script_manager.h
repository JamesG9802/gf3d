#pragma once
#include "gfc_hashmap.h"

#include "entity.h"
#include "script.h"

#include "game_state.h"
#include "meta_state.h"

typedef enum CombatTurn {
	Player,
	Monster
} CombatTurn;

typedef struct ManagerData {
	/// <summary>
	/// A map<char*, Entity> of string names to entities.
	/// </summary>
	HashMap* entities;

	/// <summary>
	/// The current turn in the battle
	/// </summary>
	CombatTurn turn;

	/// <summary>
	/// The current day of the game.
	/// </summary>
	Sint32 currentDay;

	/// <summary>
	/// The gamestate of the game.
	/// </summary>
	GameState gamestate;

	/// <summary>
	/// The metastate of the game.
	/// </summary>
	MetaState metastate;
} ManagerData;

/// <summary>
/// Creates a new data
/// </summary>
/// <returns></returns>
ManagerData* script_manager_newdata();

/// <summary>
/// Frees the manager's data
/// </summary>
void script_manager_freedata(Script* script);

void script_manager_hidedayandshownight();
void script_manager_hidenightandshowday();

/// <summary>
/// Gets the currrent script manager.
/// </summary>
/// <returns></returns>
Script* script_manager_get();

/// <summary>
/// Gets the current script manager's data
/// </summary>
/// <returns></returns>
ManagerData* script_manager_getdata();

/// <summary>
/// Get the current game state
/// </summary>
/// <returns></returns>
GameState script_manager_getgamestate();

/// <summary>
/// Gets the current meta state
/// </summary>
/// <returns></returns>
MetaState script_manager_getmetastate();

/// <summary>
/// Set the current game state.
/// </summary>
/// <param name="gamestate"></param>
void script_manager_setgamestate(GameState gamestate);

/// <summary>
/// Set the current meta state.
/// </summary>
/// <param name="metastate"></param>
void script_manager_setmetastate(MetaState metastate);

/// <summary>
/// Stores the pointer to the entity with its name as the key.
/// </summary>
/// <param name="name"></param>
/// <param name="entity"></param>
/// <returns></returns>
void script_manager_flagentity(char* name, Entity* entity);

/// <summary>
/// Removes the pointer to the entity.
/// </summary>
/// <param name="name"></param>
void script_manager_unflagentity(char* name);

/// <summary>
/// Get an entity if it has been flagged by the manager before.
/// </summary>
/// <param name="name"></param>
/// <returns></returns>
Entity* script_manager_getentity(char* name);
