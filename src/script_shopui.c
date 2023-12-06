#include <stdio.h>
#include <stdlib.h>

#include "gfc_input.h"

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

#include "engine_time.h"
#include "engine_utility.h"

#include "entity_bounds.h"
#include "script_defs.h"

#include "script_manager.h"
#include "script_ui.h"
#include "script_shopui.h"

/// <summary>
/// Set the buyable dice sprites for the children
/// </summary>
script_shopui_setsprites(Entity* self) {
	for (int i = 0; i < gfc_list_get_count(self->children); i++) {
		Entity* child = gfc_list_get_nth(self->children, i);
		if (!child->customData) {
			slog("Missing UIData for child");
			continue;
		}
		if(((UIData*)child->customData)->sprite)
			gf2d_sprite_free(((UIData*)child->customData)->sprite);
		((UIData*)child->customData)->sprite = gf2d_sprite_load("images/panel.png", 0, 0, 0);
	}
}

void script_shopui_register_callbacks(Entity* self, Script* script) {

}

ShopUIData* script_shopui_newdata() {
	ShopUIData* data = malloc(sizeof(ShopUIData));
	if (!data) {
		slog("Couldn't allocate data for shop ui.");
		return NULL;
	}
	data->state = ShopUIState_HIDDEN;
	return data;
}

void script_shopui_freedata(Script* script) {
	if (script && script->data)
		free(script->data);
}

void script_shopui_toggle(Entity* entity, Script* script) {
	if (!entity || !script->data || !script) return;
	switch (((ShopUIData*)script->data)->state) {
	case ShopUIState_HIDDEN:
		if (script_manager_getmetastate() == INMENU)
		{
			//  You aren't supposed to be able to open shop if another menu is open.
			return;
		}

		((ShopUIData*)script->data)->state = ShopUIState_VISIBLE;
		script_ui_sethidden(entity, false);
		for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
			script_ui_sethidden(gfc_list_get_nth(entity->children, i), false);
		}
		script_manager_setmetastate(INMENU);
		break;
	case ShopUIState_VISIBLE:
		((ShopUIData*)script->data)->state = ShopUIState_HIDDEN;
		script_ui_sethidden(entity, true);
		for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
			script_ui_sethidden(gfc_list_get_nth(entity->children, i), true);
		}
		script_manager_setmetastate(OK);
		break;
	}
}

void script_shopui_hide(Entity* entity, Script* script) {
	if (!entity || !script) return;
	((ShopUIData*)script->data)->state = ShopUIState_VISIBLE;
	script_inventoryui_toggle(entity, script);
}
/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	script->data = script_shopui_newdata();
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
	script_shopui_freedata(script);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_shopui() {
	return script_new("shopui", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
