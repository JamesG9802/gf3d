#pragma once
/// <summary>
/// Represents the current meta state of the game, as in if it is playing normally, paused, in menu, etc.
/// </summary>
typedef enum {
	/// <summary>
	/// Used when the game is in an invalid metastate
	/// </summary>
	INVALID_META,

	/// <summary>
	/// Used when the game is playing normally.
	/// </summary>
	OK,
	/// <summary>
	/// Used when the game is in a menu.
	/// </summary>
	INMENU,
	/// <summary>
	/// Used when the game is in paused.
	/// </summary>
	PAUSED
} MetaState;
