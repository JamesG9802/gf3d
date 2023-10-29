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
	slog("THECK");
	slog_sync();
	if (script->data == NULL) {
		script->data = calloc(sizeof(char), 5);
		memcpy(&script->data, "Lmao", sizeof(char) * 5);
		slog("CHECK");
		slog_sync();
	}
	slog("WHECK");
	slog_sync();
	engine_utility_settexture(
		self,
		engine_utility_createtexturefromtext(
			gf2d_font_get_by_tag(FT_H1),
			"kmao",
			self->color
		)
	);
}

/// <summary>
/// Called every think frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Think(Entity* self, Script* script) {
	/*
	const char strings[5][10];
	char string[10];
	strcpy(strings[0], "ok");
	strcpy(strings[1], "alright");
	strcpy(strings[2], "yeah");
	strcpy(strings[3], "carrot");
	strcpy(strings[4], "🥲");

	timeDelta += engine_time_delta();
	if (timeDelta > 5)
	{
		timeDelta = 0;
	}
	strcpy(string, strings[(int)timeDelta]);
	engine_utility_settexture(
		self,
		engine_utility_createtexturefromtext(
			gf2d_font_get_by_tag(FT_H1),
			string,
			self->color
		)
	);
	*/
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
	//if (argc >= 0) {
	//	script->data = malloc(sizeof(char) * (strlen(argv[0]) + 1));
	//	strcpy(&script->data, argv[0]);
	//}
}

Script* script_new_billboard() {
	return script_new(&Start, &Think, &Update, &Destroy, &Arguments);
}

/*eol@eof*/
