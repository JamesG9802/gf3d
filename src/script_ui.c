#include <stdio.h>
#include <math.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_config.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "gf2d_sprite.h"

#include "entity.h"
#include "script.h"

#include "event_manager.h"
#include "engine_utility.h"
#include "engine_time.h"

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
		case MIDDLERIGHT:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 2.0;
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 4.0;
		}	
		return position;
	}
	return vector2d(0, 0);
}

UIData script_ui_newuidata() {
	UIData data = {0};
	data.sprite = NULL;
	data.color = gfc_color(1, 1, 1, 1);
	data.mode = TOPLEFT;
	data.isInteractable = false;
	data.associatedEvent[0] = '\0';
	data.currentFrame = 0;
}

Bool script_ui_ismouseover(Entity* self) {
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

void script_ui_setcolor(Entity* self, Color color) {
	if (!self) return false;
	((UIData*)self->customData)->color = color;
}

void script_ui_setpositionndc(Entity* self, Vector2D position) {
	if (!self || !self->customData) return;
	position = engine_utility_ndctoscreen(position);
}

void script_ui_setanchormode(Entity* self, AnchorMode mode) {
	if (!self || !self->customData) return;
	((UIData*)self->customData)->mode = mode;
}

void script_ui_sethidden(Entity* self, Bool hidden) {
	if (!self) return;
	self->hidden = true;
}

void script_ui_setframenum(Entity* self, Uint32 frameNum) {
	if (!self || !self->customData) return;
	((UIData*)self->customData)->currentFrame = frameNum;
}

//	Uses the 2D render pipeline instead of the 3D one.
static void EntityUIDraw(Entity* self) {
	if (self->customData && ((UIData*)self->customData)->sprite && !self->hidden)
	{
		UIData* data = (UIData*)self->customData;
		Vector2D position = getRenderPosition(self);
		gf2d_sprite_draw(data->sprite,
			vector2d(position.x, position.y),
			vector2d(self->scale.x, self->scale.y),
			vector3d(0, 0, 0),
			data->color,
			data->currentFrame);
	}
}

/// <summary>
/// Called when a script is created.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// </summary>
static void Start(Entity* self, Script* script) {
	self->skipCommonDraw = true;
	self->skipCommonUpdate = true;
	self->draw = &EntityUIDraw;
	Vector2D pos = vector2d(self->position.x, self->position.y);
	pos = engine_utility_ndctoscreen(pos);
	self->position.x = pos.x;
	self->position.y = pos.y;
}

/// <summary>
/// Called every think frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// </summary>
static void Think(Entity* self, Script* script) {

}

/// <summary>
/// Called every update frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// </summary>
static void Update(Entity* self, Script* script) {
	if (!self->customData)
	{
		slog("No data for ui");
		return;
	}
	if (((UIData*)self->customData)->isInteractable && script_ui_ismouseover(self, script))
	{
		if(engine_utility_isleftmousedown())
			((UIData*)self->customData)->color = gfc_color(0.5, 0.5, 0.5, 1);
		else
			((UIData*)self->customData)->color = gfc_color(0.75, 0.75, 0.75, 1);
		if(engine_utility_isleftmousereleased() && ((UIData*)self->customData)->associatedEvent)
			event_manager_fire_event(((UIData*)self->customData)->associatedEvent);
	}
	else
	{
		((UIData*)self->customData)->color = gfc_color(1, 1, 1, 1);
	}
}

/// <summary>
/// Called when a script is destroyed.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// </summary>
static void Destroy(Entity* self, Script* script) {
	if (self->customData) {
		if (((UIData*)self->customData)->sprite)
		{
			gf2d_sprite_free(((UIData*)self->customData)->sprite);
		}
		free(self->customData);
	}
}

/// <summary>
/// Called before Start when a script has initialization arguments to read in.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script_s*">Caller script</param>
/// <param name="SJson*">SJson object. Note that 'name' is a reserved field.</param>
/// </summary>
static void Arguments(Entity* self, Script* script, SJson* json) {
	if (!json) return;
	if(sj_get_string_value(sj_object_get_value(json, "image"))) {
		char* imagePath = sj_get_string_value(sj_object_get_value(json, "image"));
		UIData data = script_ui_newuidata();
		self->customData = malloc(sizeof(UIData));
		if (!self->customData)
		{
			slog("Couldn't allocate memory for ui data");
			slog_sync();
			return;
		}
		memcpy(self->customData, &data, sizeof(UIData));
		((UIData*)self->customData)->sprite = gf2d_sprite_load(imagePath, 0, 0, 1);
	}
	else if (sj_get_string_value(sj_object_get_value(json, "images")) && 
		sj_value_as_vector2d(sj_object_get_value(json, "spriteInfo"),NULL)) {
		char* imagePath = sj_get_string_value(sj_object_get_value(json, "images"));
		UIData data = script_ui_newuidata();
		self->customData = malloc(sizeof(UIData));
		if (!self->customData)
		{
			slog("Couldn't allocate memory for ui data");
			slog_sync();
			return;
		}
		memcpy(self->customData, &data, sizeof(UIData));
		//	Formatted in config with frame width, frame height, and frames per line
		Vector3D spriteInfo;
		sj_value_as_vector2d(sj_object_get_value(json, "spriteInfo"), &spriteInfo);
		((UIData*)self->customData)->sprite = gf2d_sprite_load(imagePath, 
			(int)spriteInfo.x, 
			(int)spriteInfo.y, 
			(int)spriteInfo.z);
	}
	if (sj_get_string_value(sj_object_get_value(json, "anchor"))) {
		if (!self->customData)
			return;
		char* anchorMode = sj_get_string_value(sj_object_get_value(json, "anchor"));
		if(strcmp(anchorMode, "topleft") == 0)
			((UIData*)self->customData)->mode = TOPLEFT;
		else if (strcmp(anchorMode, "topmiddle") == 0)
			((UIData*)self->customData)->mode = TOPMIDDLE;
		else if(strcmp(anchorMode, "center") == 0)
			((UIData*)self->customData)->mode = CENTER;
		else if(strcmp(anchorMode, "middleright") == 0)
			((UIData*)self->customData)->mode = MIDDLERIGHT;
	}
	if (sj_get_bool_value(sj_object_get_value(json, "interactable"), NULL)) {
		if (!self->customData)
			return;
		Bool isInteractable;
		sj_get_bool_value(sj_object_get_value(json, "interactable"), &isInteractable);
		((UIData*)self->customData)->isInteractable = isInteractable;
	}
	if (sj_get_string_value(sj_object_get_value(json, "event"))) {
		if (!self->customData)
			return;
		gfc_line_cpy(((UIData*)self->customData)->associatedEvent, 
			sj_get_string_value(sj_object_get_value(json, "event")));
	}
}
Script* script_new_ui() {
	return script_new("ui", &Start, &Think, &Update, &Destroy, &Arguments);
}

/*eol@eof*/
