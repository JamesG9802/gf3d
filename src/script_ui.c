#include <stdio.h>
#include <math.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_config.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "entity.h"
#include "script.h"

#include "event_manager.h"
#include "engine_utility.h"
#include "engine_time.h"

#include "script_defs.h"

#include "script_ui.h"
#include "script_manager.h"
#include "script_player.h"

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
		case TOPRIGHT:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 2.0;
			break;
		case MIDDLELEFT:
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 4.0;
			break;
		case CENTER:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 4.0;
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 4.0;
			break;
		case MIDDLERIGHT:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 2.0;
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 4.0;
			break;
		case BOTTOMLEFT:
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 2.0;
			break;
		case BOTTOMMIDDLE:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 4.0;
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 2.0;
			break;
		case BOTTOMRIGHT:
			position.x -= ((UIData*)self->customData)->sprite->frameWidth * self->scale.x / 2.0;
			position.y -= ((UIData*)self->customData)->sprite->frameHeight * self->scale.y / 2.0;

		}	
		return position;
	}
	return vector2d(0, 0);
}

UIData script_ui_newuidata() {
	UIData data = {0};
	data.sprite = NULL;
	data.color = gfc_color(1, 1, 1, 1);
	data.currentText = NULL;
	data.text = NULL;
	data.wildcard = NULL;
	data.positionNDC = vector2d(0, 0);
	data.mode = TOPLEFT;
	data.isInteractable = false;
	data.associatedEvent[0] = '\0';
	data.currentFrame = 0;
	return data;
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

void script_ui_updateposition(Entity* self) {
	if (!self || !self->customData) return;
	Vector2D positionNDC = ((UIData*)self->customData)->positionNDC;
	Vector2D position;
	//	If don't have a parent that is a UI, assume that the entire screen is the parent
	if (!self->parent || !entity_get_script(self->parent, "ui") || !self->parent->customData)
	{
		position = engine_utility_ndctoscreen(positionNDC);
	}
	else
	{
		Vector2D parentPosition = getRenderPosition(self->parent);
		int width, height;
		width = ((UIData*)self->parent->customData)->sprite->frameWidth * self->parent->scale.x / 2.0;
		height = ((UIData*)self->parent->customData)->sprite->frameHeight * self->parent->scale.y / 2.0;
		position = vector2d(parentPosition.x + (positionNDC.x + 1) * width / 2.0, 
			parentPosition.y + (positionNDC.y - 1) * height / -2.0);
	}
	self->position.x = position.x;
	self->position.y = position.y;
}
void script_ui_setcolor(Entity* self, Color color) {
	if (!self) return false;
	((UIData*)self->customData)->color = color;
}

void script_ui_setpositionndc(Entity* self, Vector2D position) {
	if (!self || !self->customData) return;

	((UIData*)self->customData)->positionNDC.x = position.x;
	((UIData*)self->customData)->positionNDC.y = position.y;

	script_ui_updateposition(self);
}

void script_ui_setanchormode(Entity* self, AnchorMode mode) {
	if (!self || !self->customData) return;
	((UIData*)self->customData)->mode = mode;
}

void script_ui_sethidden(Entity* self, Bool hidden) {
	if (!self) return;
	self->hidden = hidden;
}

void script_ui_setframenum(Entity* self, Uint32 frameNum) {
	if (!self || !self->customData) return;
	((UIData*)self->customData)->currentFrame = frameNum;
}

Bool script_ui_gethidden(Entity* self) {
	if (!self) return false;
	return self->hidden;
}

//	Uses the 2D render pipeline instead of the 3D one.
static void EntityUIDraw(Entity* self) {
	if (self->customData && ((UIData*)self->customData)->sprite && !self->hidden)
	{
		UIData* data = (UIData*)self->customData;

		if (data->text)
		{
			Vector2D renderPos = getRenderPosition(self);
			gf2d_draw_rect_filled(gfc_rect(renderPos.x - 5, renderPos.y, data->sprite->frameWidth + 30, 
				data->sprite->frameHeight + 30), gfc_color8(255, 255, 255, 255));
		}

		gf2d_sprite_draw(data->sprite,
			getRenderPosition(self),
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
	if (((UIData*)self->customData)->text) {
		//	Text and image are exclusive, text takes priority
		if (((UIData*)self->customData)->sprite) {
			gf2d_sprite_free(((UIData*)self->customData)->sprite);
		}
		((UIData*)self->customData)->sprite = gf2d_sprite_from_surface(
			TTF_RenderText_Blended(
				gf2d_font_get_by_tag(FT_H1)->font,
				((UIData*)self->customData)->text,
				gfc_color_to_sdl(((UIData*)self->customData)->color)
			),
			0, 0, 0);
	}
	self->skipCommonDraw = true;
	self->skipCommonUpdate = true;
	self->draw = &EntityUIDraw;
	Vector2D pos = vector2d(self->position.x, self->position.y);
	script_ui_setpositionndc(self, pos);
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

	//	for now wildcards are hardcoded
	if (((UIData*)self->customData)->text && ((UIData*)self->customData)->wildcard) {
		char* text = ((UIData*)self->customData)->text;
		char* currentText = ((UIData*)self->customData)->currentText;
		char* wildCard = ((UIData*)self->customData)->wildcard;

		char newstring[1024];
		if (strcmp(wildCard, "currentDay") == 0)
		{
			sprintf(newstring, "%s%d", text, script_manager_getdata()->currentDay);
			if (currentText && strcmp(currentText, newstring) == 0) {
				return;
			}
		}
		else if (strcmp(wildCard, "health") == 0)
		{
			sprintf(newstring, "%s%d/%d", text, 
				script_player_getplayerdata()->currentHealth, 
				script_player_getplayerdata()->maxHealth);
			if (currentText && strcmp(currentText, newstring) == 0) {
				return;
			}
		}
		else if (strcmp(wildCard, "mana") == 0)
		{
			sprintf(newstring, "%s%d/%d", text,
				script_player_getplayerdata()->currentMana,
				script_player_getplayerdata()->maxMana);
			if (currentText && strcmp(currentText, newstring) == 0) {
				return;
			}
		}
		else return;

		free(currentText);
		((UIData*)self->customData)->currentText = malloc(sizeof(char) * (strlen(newstring) + 1));
		if (((UIData*)self->customData)->sprite) {
			gf2d_sprite_free(((UIData*)self->customData)->sprite);
		}
		((UIData*)self->customData)->sprite = gf2d_sprite_from_surface(
			TTF_RenderText_Blended(
				gf2d_font_get_by_tag(FT_H1)->font,
				newstring,
				gfc_color_to_sdl(((UIData*)self->customData)->color)
			),
			0, 0, 0);
	}


	if (!self->hidden && ((UIData*)self->customData)->isInteractable && script_ui_ismouseover(self, script))
	{
		if(engine_utility_isleftmousedown())
			((UIData*)self->customData)->color = gfc_color(0.5, 0.5, 0.5, 1);
		else
			((UIData*)self->customData)->color = gfc_color(0.75, 0.75, 0.75, 1);
		if(engine_utility_isleftmousereleased() && ((UIData*)self->customData)->associatedEvent)
			event_manager_fire_event(((UIData*)self->customData)->associatedEvent);
	}
	else if (((UIData*)self->customData)->isInteractable && !script_ui_ismouseover(self, script))
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
		if (((UIData*)self->customData)->text)
		{
			free(((UIData*)self->customData)->text);
		}
		if (((UIData*)self->customData)->wildcard)
		{
			free(((UIData*)self->customData)->wildcard);
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
	if (!self || !json) return;
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
		sj_value_as_vector3d(sj_object_get_value(json, "spriteInfo"), &spriteInfo);
		((UIData*)self->customData)->sprite = gf2d_sprite_load(imagePath, 
			(int)spriteInfo.x, 
			(int)spriteInfo.y, 
			(int)spriteInfo.z);
	}
	else if (sj_get_string_value(sj_object_get_value(json, "text"))) {
		UIData data = script_ui_newuidata();
		self->customData = malloc(sizeof(UIData));
		if (!self->customData)
		{
			slog("Couldn't allocate memory for ui data");
			slog_sync();
			return;
		}
		memcpy(self->customData, &data, sizeof(UIData));

		char* text = ((UIData*)self->customData)->text;
		char* currentText = ((UIData*)self->customData)->currentText;

		text = malloc(sizeof(char) * (strlen(sj_get_string_value(sj_object_get_value(json, "text"))) + 1));
		if (text)
			strcpy(text, sj_get_string_value(sj_object_get_value(json, "text")));

		currentText = malloc(sizeof(char) * (strlen(sj_get_string_value(sj_object_get_value(json, "text"))) + 1));
		if (currentText)
			strcpy(currentText, sj_get_string_value(sj_object_get_value(json, "text")));
		((UIData*)self->customData)->text = text;
		((UIData*)self->customData)->currentText = currentText;
	}
	if (sj_get_string_value(sj_object_get_value(json, "text")) &&
		sj_get_string_value(sj_object_get_value(json, "wildcard"))) {
		char* wildcard = ((UIData*)self->customData)->wildcard;
		wildcard = malloc(sizeof(char) * (strlen(sj_get_string_value(sj_object_get_value(json, "wildcard"))) + 1));
		if (wildcard)
			strcpy(wildcard, sj_get_string_value(sj_object_get_value(json, "wildcard")));
		((UIData*)self->customData)->wildcard = wildcard;
	}
	if (sj_get_string_value(sj_object_get_value(json, "anchor"))) {
		if (!self->customData)
			return;
		char* anchorMode = sj_get_string_value(sj_object_get_value(json, "anchor"));
		if(strcmp(anchorMode, "topleft") == 0)
			((UIData*)self->customData)->mode = TOPLEFT;
		else if (strcmp(anchorMode, "topmiddle") == 0)
			((UIData*)self->customData)->mode = TOPMIDDLE;
		else if (strcmp(anchorMode, "topright") == 0)
			((UIData*)self->customData)->mode = TOPRIGHT;
		else if (strcmp(anchorMode, "middleleft") == 0)
			((UIData*)self->customData)->mode = MIDDLELEFT;
		else if (strcmp(anchorMode, "center") == 0)
			((UIData*)self->customData)->mode = CENTER;
		else if(strcmp(anchorMode, "middleright") == 0)
			((UIData*)self->customData)->mode = MIDDLERIGHT;
		else if (strcmp(anchorMode, "bottomleft") == 0)
			((UIData*)self->customData)->mode = BOTTOMLEFT;
		else if (strcmp(anchorMode, "bottommiddle") == 0)
			((UIData*)self->customData)->mode = BOTTOMMIDDLE;
		else if(strcmp(anchorMode, "bottomright") == 0)
			((UIData*)self->customData)->mode = BOTTOMRIGHT;
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
	if (sj_get_bool_value(sj_object_get_value(json, "hidden"), NULL)) {
		if (!self->customData)
			return;
		Bool isHidden;
		sj_get_bool_value(sj_object_get_value(json, "hidden"), &isHidden);
		self->hidden = isHidden;
	}
	if (sj_value_as_vector4d(sj_object_get_value(json, "color"), NULL)) {
		if (!self->customData)
			return;
		Vector4D color;
		sj_value_as_vector4d(sj_object_get_value(json, "color"), &color);
		((UIData*)self->customData)->color = gfc_color(color.x, color.y, color.z, color.w);
	}
}
Script* script_new_ui() {
	return script_new("ui", &Start, &Think, &Update, &Destroy, &Arguments);
}

/*eol@eof*/
