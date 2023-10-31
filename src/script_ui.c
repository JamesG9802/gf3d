#include <stdio.h>
#include <math.h>

#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include <gf2d_sprite.h>

#include "entity.h"
#include "script.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "entity_bounds.h"
#include "script_defs.h"

typedef struct UIData {
	Sprite* sprite;
} UIData;



//	Uses the 2D render pipeline instead of the 3D one.
static void EntityUIDraw(Entity* self) {
	if (self->customData && ((UIData*)self->customData)->sprite)
	{
		gf2d_sprite_draw(((UIData*)self->customData)->sprite,
			vector2d(self->position.x, self->position.y),
			vector2d(self->scale.x, self->scale.y),
			vector3d(0, 0, 0),
			gfc_color(1, 1, 1, 1),
			0);
	}
}

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	self->skipCommonDraw = true;
	self->skipCommonUpdate = true;
	self->draw = &EntityUIDraw;
	Vector2D pos = vector2d(self->position.x, self->position.y);
	pos = engine_utility_ndctoscreen(pos);
	self->position.x = pos.x;
	self->position.y = pos.y;
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {

}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {

}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
	if (self->customData) {
		if (((UIData*)self->customData)->sprite)
		{
			gf2d_sprite_free(((UIData*)self->customData)->sprite);
		}
		free(self->customData);
	}
}
static void Arguments(Entity* self, Script* script, const char** argv, int argc) {
	if (argc >= 0) {
		self->customData = malloc(sizeof(UIData));
		if (!self->customData)
		{
			slog("Couldn't allocate memory for ui data");
			slog_sync();
			return;
		}
		((UIData*)self->customData)->sprite = gf2d_sprite_load(argv[0], 0, 0, 1);
	}
}
Script* script_new_ui() {
	return script_new("ui", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
