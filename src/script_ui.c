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

#include "script_ui.h"

/// <summary>
/// Using the anchormode, compute the actual x and y position of the sprite.
/// </summary>
/// <returns></returns>
Vector2D getRenderPosition(Entity* self) {
	if (self && self->customData)
	{
		Vector2D position = vector2d(self->position.x, self->position.y);
		switch (((UIData*)self->customData)->mode)
		{
		default:
		case TOPLEFT:
			break;
		case TOPMIDDLE:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 4.0;
			break;
		case CENTER:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 4.0;
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 4.0;
			break;
		}	
		return position;
	}
	return vector2d(0, 0);
}

Bool script_ui_ismouseover(Entity* self, Script* script) {
	if (!self || !self->customData)
		return false;
	int x, y;
	Vector2D position;
	int width, height;
	SDL_GetMouseState(&x, &y);
	position = getRenderPosition(self);
	width = ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 2.0;
	height = ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 2.0;
	return !(x < position.x || x > position.x + width ||
		y < position.y || y > position.y + height);
}

void script_ui_setcolor(Entity* self, Script* script, Color color) {
	if (!self)
		return false;
	((UIData*)self->customData)->color = color;
}

void script_ui_setpositionndc(Entity* self, Script* script, Vector2D position) {
	if (!self || !self->customData)
		return false;
	position = engine_utility_ndctoscreen(position);
}

void script_ui_setanchormode(Entity* self, Script* script, AnchorMode mode) {
	if (!self || !self->customData)
		return false;
	((UIData*)self->customData)->mode = mode;
}

//	Uses the 2D render pipeline instead of the 3D one.
static void EntityUIDraw(Entity* self) {
	if (self->customData && ((UIData*)self->customData)->sprite)
	{
		Vector2D position = getRenderPosition(self);
		gf2d_sprite_draw(((UIData*)self->customData)->sprite,
			vector2d(position.x, position.y),
			vector2d(self->scale.x, self->scale.y),
			vector3d(0, 0, 0),
			((UIData*)self->customData)->color,
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
	if (script_ui_ismouseover(self, script))
	{
		((UIData*)self->customData)->color = gfc_color(0, 1, 1, 1);
	}
	else
	{
		((UIData*)self->customData)->color = gfc_color(1, 1, 1, 1);
	}

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
	if (argc > 0) {
		self->customData = malloc(sizeof(UIData));
		if (!self->customData)
		{
			slog("Couldn't allocate memory for ui data");
			slog_sync();
			return;
		}
		((UIData*)self->customData)->sprite = gf2d_sprite_load(argv[0], 0, 0, 1);
		((UIData*)self->customData)->color = gfc_color(1, 1, 1, 1);
	}
	if (argc > 1) {
		if (!self->customData)
			return;
		if(strcmp(argv[1], "topleft") == 0)
			((UIData*)self->customData)->mode = TOPLEFT;
		else if (strcmp(argv[1], "topmiddle") == 0)
			((UIData*)self->customData)->mode = TOPMIDDLE;
		else if(strcmp(argv[1], "center") == 0)
			((UIData*)self->customData)->mode = CENTER;
	}
}
Script* script_new_ui() {
	return script_new("ui", &Start, &Think, &Update, &Destroy, &Arguments);
}

/*eol@eof*/
