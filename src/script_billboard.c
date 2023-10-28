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

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
	engine_utility_settexture(
		self,
		engine_utility_createtexturefromtext(
			gf2d_font_get_by_tag(FT_H1),
			"Lmao",
			self->color
		)
	);

}

/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {
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
}

/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
	gf3d_camera_get_rotation(&self->rotation);
}

/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
}

Script* script_new_billboard() {
	return script_new(&Start, &Think, &Update, &Destroy);
}

/*eol@eof*/
