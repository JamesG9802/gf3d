#pragma once

#include "entity.h"

typedef struct Script_s Script_s;

/// <summary>
/// A script describes components that attach to entities.
/// </summary>
typedef struct Script_s {

	/// <summary>
	/// Called when a script is created.
	/// <param name="Entity*">Attached entity</param>
	/// <param name="Script_s*">Caller script</param>
	/// </summary>
	void (*Start)(Entity*, Script_s*);

	/// <summary>
	/// Called every think frame.
	/// <param name="Entity*">Attached entity</param>
	/// <param name="Script_s*">Caller script</param>
	/// </summary>
	void (*Think)(Entity*, Script_s*);

	/// <summary>
	/// Called every update frame.
	/// <param name="Entity*">Attached entity</param>
	/// <param name="Script_s*">Caller script</param>
	/// </summary>
	void (*Update)(Entity*, Script_s*);

	/// <summary>
	/// Called when a script is destroyed.
	/// <param name="Entity*">Attached entity</param>
	/// <param name="Script_s*">Caller script</param>
	/// </summary>
	void (*Destroy)(Entity*, Script_s*);

	/// <summary>
	/// Called before Start when a script has initialization arguments to read in.
	/// <param name="Entity*">Attached entity</param>
	/// <param name="Script_s*">Caller script</param>
	/// <param name="const char*">Array of string arguments</param>
	/// <param name="int">Number of arguments</param>
	/// </summary>
	void (*Arguments)(Entity*, Script_s*, const char**, int);
	void* data;
} Script;


/**
 * @brief DO NOT use this to create a script if you do not know what you are doing. This is used to help create base scripts.
 * @return a new generic script object.
 */
Script* script_new(
	void(*Start)(Entity*, Script_s*),
	void(*Think)(Entity*, Script_s*),
	void(*Update)(Entity*, Script_s*),
	void(*Destroy)(Entity*, Script_s*),
	void(*Arguments)(Entity*, Script_s*, const char**, int)
);

/*eol@eof*/
