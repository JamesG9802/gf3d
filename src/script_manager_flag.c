#include <stdio.h>
#include <math.h>

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "script_defs.h"

#include "script_manager.h"

/// <summary>
/// Called before Start when a script has initialization arguments to read in.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// <param name="SJson*">SJson object. Note that 'name' is a reserved field.</param>
/// </summary>
static void Arguments(Entity* self, Script* script, SJson* json) {
	if (!json) return;
	slog(sj_get_string_value(sj_object_get_value(json, "id")));
	if (sj_get_string_value(sj_object_get_value(json, "id"))) {
		char* id = sj_get_string_value(sj_object_get_value(json, "id"));
		script_manager_flagentity(id, self);
	}
}
Script* script_new_manager_flag() {
	return script_new("manager_flag", NULL, NULL, NULL, NULL, &Arguments);
}

/*eol@eof*/
