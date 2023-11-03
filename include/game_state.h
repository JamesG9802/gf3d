#pragma once

/// <summary>
/// Represents the current state of the game.
/// </summary>
typedef enum {
	/// <summary>
	/// Used when the game is in an invalid state.
	/// </summary>
	INVALID,
	/// <summary>
	/// Used when the game is in the growing phase when plants can be managed
	/// </summary>
	GROW,
	/// <summary>
	/// Used when the game is in the combat phase when the player can walk around and engage enemies.
	/// </summary>
	COMBAT,
	/// <summary>
	/// Used when the game is in the battle phase when the player is actively battling enemies.
	/// </summary>
	BATTLE
} GameState;

/// <summary>
/// Handles the transition of the game state.
/// </summary>
/// <param name="current"></param>
/// <param name="next"></param>
void game_state_handletransition(GameState current, GameState next);

/// <summary>
/// Gets the current state of the game.
/// </summary>
/// <returns></returns>
GameState game_state_getstate();
