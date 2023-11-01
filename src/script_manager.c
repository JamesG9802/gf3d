#include <stdio.h>

#include "simple_logger.h"

#include "gf2d_font.h"
#include "gf3d_camera.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "entity.h"
#include "script.h"

#include "game_state.h"

#include "script_defs.h"

/// <summary>
/// There can only be a single script_manager.
/// </summary>
static Script* script_manager = NULL;

Script* script_manager_get() {
	return script_manager;
}

/// <summary>
/// Called when a script is created.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Start(Entity* self, Script* script) {
	if (script_manager)
	{
		slog("Trying to create a script_manager when it already exists");
	}
	script->data = malloc(sizeof(GameState));
	if (!script->data)
	{
		slog("could not allocate memory for game data");
		slog_sync();
	}
	script_manager = script;
}

/// <summary>
/// Called every think frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Think(Entity* self, Script* script) {
}

/// <summary>
/// Called every update frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Update(Entity* self, Script* script) {
}

/// <summary>
/// Called when a script is destroyed.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Destroy(Entity* self, Script* script) {
	
}

/// <summary>
/// Called when a script has initialization arguments to read in.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// <param name="const char*">Array of string arguments</param>
/// <param name="int">Number of arguments</param>
/// </summary>
static void Arguments(Entity* self, Script* script, SJson* json) {
}

Script* script_new_manager() {
	return script_new("manager", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
