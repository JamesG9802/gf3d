#include <stdio.h>

#include "simple_logger.h"

#include "gf2d_font.h"
#include "gf3d_camera.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "entity.h"
#include "script.h"

#include "script_defs.h"

void script_billboard_settext(Entity* self, Script* script, const char* text) {
	if (!self || !script || !text) {
		return;
	}
	if (script->data)
	{
		free(script->data);
	}
	script->data = malloc(sizeof(char) * (strlen(text) + 1));

	if (!script->data)
	{
		slog("Coulnd't allocate data.");
		return;
	}
	strcpy(script->data, text);
}

void script_billboard_updatetexture(Entity* self, Script* script) {
	int width, height;
	engine_utility_settexture(
		self,
		engine_utility_createtexturefromtext(
			gf2d_font_get_by_tag(FT_H1),
			script->data,
			self->color,
			&width,
			&height
		),
		width,
		height,
		50
	);
}

/// <summary>
/// Called when a script is created.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Start(Entity* self, Script* script) {
	if (script->data == NULL) {
		script_billboard_settext(self, script, "Default Text");
	}
	script_billboard_updatetexture(self, script);
}

/// <summary>
/// Called every think frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Think(Entity* self, Script* script) {
	//self->selected = engine_utility_ismouseover(self, NULL);
}

/// <summary>
/// Called every update frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Update(Entity* self, Script* script) {
	gf3d_camera_get_rotation(&self->rotation);
}

/// <summary>
/// Called when a script is destroyed.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Destroy(Entity* self, Script* script) {
	free(script->data);
}

/// <summary>
/// Called before Start when a script has initialization arguments to read in.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// <param name="SJson*">SJson object. Note that 'name' is a reserved field.</param>
/// </summary>
static void Arguments(Entity* self, Script* script, SJson* json) {
	if (json && sj_get_string_value(sj_object_get_value(json, "text"))) {
		script_billboard_settext(self, script, sj_get_string_value(sj_object_get_value(json, "text")));
	}
}

Script* script_new_billboard() {
	return script_new("billboard", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
