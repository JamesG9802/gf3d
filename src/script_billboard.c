#include <stdio.h>

#include "simple_logger.h"

#include "gf2d_font.h"
#include "gf3d_camera.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "entity.h"
#include "script.h"

#include "script_defs.h"

static float timeDelta = 0;

/// <summary>
/// Called when a script is created.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Start(Entity* self, Script* script) {
	if (script->data == NULL) {
		char a[13] = "Default Text";
		script->data = calloc(sizeof(char), 13);
		if (!script->data)
		{
			slog("Coulnd't allocate data.");
			return;
		}
		memcpy(script->data, a, sizeof(char) * 13);
	}
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
/// Called when a script has initialization arguments to read in.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// <param name="const char*">Array of string arguments</param>
/// <param name="int">Number of arguments</param>
/// </summary>
static void Arguments(Entity* self, Script* script, const char** argv, int argc) {
	if (argc >= 0) {
		script->data = malloc(sizeof(char) * (strlen(argv[0]) + 1));
		strcpy(script->data, argv[0]);
	}
}

Script* script_new_billboard() {
	return script_new(&Start, &Think, &Update, &Destroy, &Arguments);
}

/*eol@eof*/
