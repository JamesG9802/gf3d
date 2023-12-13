#include <stdio.h>
#include <SDL_image.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"
#include "dice.h"
#include "dicevalue.h"

#include "engine_time.h"
#include "engine_utility.h"

#include "script_defs.h"
#include "script_manager.h"
#include "script_player.h"
#include "script_shopui.h"
#include "dice.h"

typedef struct InteractableData {
	int type;
} InteractableData;

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	self->selectedColor = gfc_color(1, 0, 0, 1);
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
	if (!self || !script || !script->data || script_manager_getmetastate() == INMENU) return;
	
	int type = ((InteractableData*)script->data)->type;

	switch (type) {
	default:
	case 0:
		if (vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 50) {
			self->selected = true;
		}
		else {
			self->selected = false;
		}
		if (engine_utility_isleftmousereleased() && self->selected) {
			gfc_list_append(
				((PlayerData*)entity_get_script(script_player_getplayer(), "player")->data)
				->inventory->diceSeeds,
				dice_seed_reward(script_manager_getdata()->currentDay * 2));
			entity_free(self);
		}
		break;
	case 1:
		if (vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 75) {
			self->selected = true;
		}
		else {
			self->selected = false;
		}
		if (engine_utility_isleftmousereleased() && self->selected) {
			Entity* shopui = script_manager_getentity("indicator_shop");
			Script* shopscript = entity_get_script(shopui, "shopui");
			if (shopui && shopscript)
			{
				((ShopUIData*)shopscript->data)->daysLocked = 0;
				script_shopui_update_shop(shopscript, true);
				script_shopui_hide(shopui, shopscript);
				script_shopui_toggle(shopui, shopscript);
			}
			entity_free(self);
		}
		break;
	case 2:
		if (vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 50) {
			self->selected = true;
		}
		else {
			self->selected = false;
		}
		if (engine_utility_isleftmousereleased() && self->selected) {
			entity_free(self);
			entity_load_from_prefab("prefabs/enemy_s1.prefab", NULL);
		}
		break;
	}
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
	if (!script || !script->data) return;
	free(script->data);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
	if (!script || !json) return;
	if (sj_get_string_value(sj_object_get_value(json, "type"))) {
		if (strcmp(sj_get_string_value(sj_object_get_value(json, "type")), "seeds") == 0) {
			script->data = malloc(sizeof(InteractableData));
			((InteractableData*)script->data)->type = 0;
		}
		else if (strcmp(sj_get_string_value(sj_object_get_value(json, "type")), "trader") == 0) {
			script->data = malloc(sizeof(InteractableData));
			((InteractableData*)script->data)->type = 1;
		}
		else if (strcmp(sj_get_string_value(sj_object_get_value(json, "type")), "monster") == 0) {
			script->data = malloc(sizeof(InteractableData));
			((InteractableData*)script->data)->type = 2;
		}
	}
}
Script* script_new_interactable() {
    return script_new("interactable", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
