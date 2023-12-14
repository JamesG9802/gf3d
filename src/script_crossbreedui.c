#include <stdio.h>

#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_list.h"

#include "gf2d_font.h"
#include "gf3d_camera.h"

#include "engine_utility.h"
#include "engine_time.h"
#include "event_manager.h"

#include "entity.h"
#include "scene.h"
#include "script.h"

#include "game_state.h"

#include "script_defs.h"
#include "script_manager.h"
#include "script_ui.h"
#include "script_crossbreedui.h"
#include "script_player.h"

#include "dice.h"

void update_sprite(Entity* entity, Script* script, int direction, int child) {
	if (!entity || !script || !script->data || !entity->children || gfc_list_get_count(entity->children) <= child)	return;
	CrossbreedUIData* data = script->data;

	int index = child == 0 ? data->leftDiceIndex : data->rightDiceIndex;
	int blockedIndex = child == 0 ? data->rightDiceIndex : data->leftDiceIndex;	//	both left and right cannot look at the same index.
	Bool success = false;
	List* available_dice_seeds = script_player_getplayerdata()->inventory->diceSeeds;
	if (!available_dice_seeds) return;

	//	wrap index back to (0, length -1)
	if (index < 0)
		index = gfc_list_get_count(available_dice_seeds) - 1;
	else if (index >= gfc_list_get_count(available_dice_seeds))
		index = 0;

	if (index != blockedIndex) {	//	simple case where left and right won't look at the same dice
		dice_to_ui(gfc_list_get_nth(available_dice_seeds, index), gfc_list_get_nth(entity->children, child));
		success = true;
	}
	else {	// left and right looking at the same dice, try to resolve
		index += direction;
		//	Wrap around
		if (index < 0)
			index = gfc_list_get_count(available_dice_seeds) - 1;
		else if (index >= gfc_list_get_count(available_dice_seeds))
			index = 0;

		//	If it is still equal to the blocked index, then there are not enough dice to crossbreed.
		//	Otherwise, it is safe
		if (index != blockedIndex)
		{
			dice_to_ui(gfc_list_get_nth(available_dice_seeds, index), gfc_list_get_nth(entity->children, child));
			success = true;
		}
	}


	if (success) {
		if (child == 0)
			data->leftDiceIndex = index;
		else
			data->rightDiceIndex = index;
	}
	else {
		data->leftDiceIndex = -1;
		data->rightDiceIndex = -1;

		if (((Entity*)gfc_list_get_nth(entity->children, 0))->customData) {
			Sprite* sprite = gf2d_sprite_load_image("images/default.png");
			gf2d_sprite_free(((UIData*)((Entity*)gfc_list_get_nth(entity->children, 0))->customData)->sprite);
			((UIData*)((Entity*)gfc_list_get_nth(entity->children, 0))->customData)->sprite = sprite;
		}
		if (((Entity*)gfc_list_get_nth(entity->children, 1))->customData) {
			Sprite* sprite = gf2d_sprite_load_image("images/default.png");
			gf2d_sprite_free(((UIData*)((Entity*)gfc_list_get_nth(entity->children, 1))->customData)->sprite);
			((UIData*)((Entity*)gfc_list_get_nth(entity->children, 1))->customData)->sprite = sprite;
		}
	}
}

CrossbreedUIData* script_crossbreedui_newdata() {
	CrossbreedUIData* data = malloc(sizeof(CrossbreedUIData));
	if (!data) return NULL;

	data->state = CrossbreedUIState_HIDDEN;
	data->leftDiceIndex = -1;
	data->rightDiceIndex = -1;

	return data;
}

void script_crossbreedui_freedata(Script* script) {
	if (!script || !script->data) return;
	free(script->data);
}

void script_crossbreedui_toggle(Entity* entity, Script* script) {
	if (!entity || !script->data || !script) return;
	switch (((CrossbreedUIData*)script->data)->state) {
	case CrossbreedUIState_HIDDEN:
		//  You aren't supposed to be able to open shop if another menu is open
		if (script_manager_getmetastate() == INMENU)	return;
		for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
			script_ui_sethidden(gfc_list_get_nth(entity->children, i), false);
		}
		((CrossbreedUIData*)script->data)->state = CrossbreedUIState_VISIBLE;
		script_ui_sethidden(entity, false);
		script_manager_setmetastate(INMENU);

		((CrossbreedUIData*)script->data)->leftDiceIndex = -1;
		((CrossbreedUIData*)script->data)->rightDiceIndex = -1;

		if (((Entity*)gfc_list_get_nth(entity->children, 0))->customData) {
			Sprite* sprite = gf2d_sprite_load_image("images/default.png");
			gf2d_sprite_free(((UIData*)((Entity*)gfc_list_get_nth(entity->children, 0))->customData)->sprite);
			((UIData*)((Entity*)gfc_list_get_nth(entity->children, 0))->customData)->sprite = sprite;
		}
		if (((Entity*)gfc_list_get_nth(entity->children, 1))->customData) {
			Sprite* sprite = gf2d_sprite_load_image("images/default.png");
			gf2d_sprite_free(((UIData*)((Entity*)gfc_list_get_nth(entity->children, 1))->customData)->sprite);
			((UIData*)((Entity*)gfc_list_get_nth(entity->children, 1))->customData)->sprite = sprite;
		}
		break;
	case CrossbreedUIState_VISIBLE:
		((CrossbreedUIData*)script->data)->state = CrossbreedUIState_HIDDEN;
		script_ui_sethidden(entity, true);
		for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
			script_ui_sethidden(gfc_list_get_nth(entity->children, i), true);
		}
		script_manager_setmetastate(OK);
		break;
	}
}

void script_crossbreedui_hide(Entity* entity, Script* script) {
	if (!entity || !script || !script->data) return;
	((CrossbreedUIData*)script->data)->state = CrossbreedUIState_VISIBLE;
	script_crossbreedui_toggle(entity, script);
}

/// <summary>
/// Called when a script is created.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Start(Entity* self, Script* script) {
	script->data = script_crossbreedui_newdata();
}

/// <summary>
/// Called every think frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Think(Entity* self, Script* script) {
	if (((CrossbreedUIData*)script->data)->state != CrossbreedUIState_VISIBLE)
		return;
	const Uint8* keys;
	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
	
	Bool leftChanged = false, rightChanged = false;
	int direction = 0;
	//	control left dice selector
	if (gfc_input_keycode_released(SDL_SCANCODE_A)) {
		((CrossbreedUIData*)script->data)->leftDiceIndex -= 1;
		leftChanged = true;
		direction = -1;
	}
	else if (gfc_input_keycode_released(SDL_SCANCODE_D)) {
		((CrossbreedUIData*)script->data)->leftDiceIndex += 1;
		leftChanged = true;
		direction = 1;
	}
	//	control right dice selector
	if (gfc_input_keycode_released(SDL_SCANCODE_LEFT)) {
		((CrossbreedUIData*)script->data)->rightDiceIndex -= 1;
		rightChanged = true;
		direction = -1;
		
	}
	else if (gfc_input_keycode_released(SDL_SCANCODE_RIGHT)) {
		((CrossbreedUIData*)script->data)->rightDiceIndex += 1;
		rightChanged = true;
		direction = 1;
	}
	if (leftChanged)
		update_sprite(self, script, direction, 0);
	if (rightChanged)
		update_sprite(self, script, direction, 1);

	//	so you cant crossbreed a dice you cannot see yet
	if (!(leftChanged || rightChanged) && gfc_input_keycode_released(SDL_SCANCODE_SPACE)) {
		if (((CrossbreedUIData*)script->data)->leftDiceIndex != -1 && ((CrossbreedUIData*)script->data)->rightDiceIndex != -1) {
			int leftIndex = ((CrossbreedUIData*)script->data)->leftDiceIndex;
			int rightIndex = ((CrossbreedUIData*)script->data)->rightDiceIndex;
			List* dices = script_player_getplayerdata()->inventory->diceSeeds;

			Dice* child = dice_seed_crossbreed(gfc_list_get_nth(dices, leftIndex), gfc_list_get_nth(dices, rightIndex));

			//	Delete the farthest index first so the list does not change index
			if (leftIndex > rightIndex)
			{
				gfc_list_delete_nth(dices, leftIndex);
				gfc_list_delete_nth(dices, rightIndex);
			}
			else {
				gfc_list_delete_nth(dices, rightIndex);
				gfc_list_delete_nth(dices, leftIndex);
			}
			gfc_list_append(dices, child);
			script_crossbreedui_hide(self, script);
		}
	}
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
	script_crossbreedui_freedata(script);
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

Script* script_new_crossbreedui() {
	return script_new("crossbreedui", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
