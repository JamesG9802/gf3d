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
#include "script_player.h"
#include "script_ui.h"
#include "script_shopui.h"

/// <summary>
/// Set the buyable dice sprites for the children
/// </summary>
void set_sprites(Entity* self, Script* script) {
	if (!self || !script || !script->data) return;

	for (int i = 0; i < gfc_list_get_count(self->children); i++) {
		Entity* child = gfc_list_get_nth(self->children, i);
		if (!child->customData) {
			slog("Missing UIData for child");
			continue;
		}
		dice_to_ui(&((ShopUIData*)script->data)->diceInventory[i], child);
	}
}

/// <summary>
/// Updates the shop inventory with random dice from the config file.
/// </summary>
/// <param name="script"></param>
void update_shop(Script* script) {
	if (!script || !script->data) return;
	List* dices = dice_list_load("config/diceshop.json");

	//	If the number of dice available to pick from is >= 4 
	//	we can delete a dice to prevent duplicates
	Bool largeEnough = gfc_list_get_count(dices) >= 4;

	for (int i = 0; i < 4; i++) {
		int index = (int)(gfc_random() * gfc_list_get_count(dices));
		memcpy(
			&((ShopUIData*)script->data)->diceInventory[i],
			gfc_list_get_nth(dices, index),
			sizeof(Dice)
		);
		if (largeEnough) {
			gfc_list_delete_nth(dices, index);
		}
	}
	gfc_list_delete(dices);
}

/// <summary>
/// Adds a dice to the player's inventory and locks the shop for three days.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
/// <param name="number"></param>
void buy_dice(Entity* entity, Script* script, int number) {
	if (!entity || !script || !script->data || number < 0 || number > 3) return;
	Dice* dice = malloc(sizeof(Dice));
	memcpy(dice, &((ShopUIData*)script->data)->diceInventory[number], sizeof(Dice));

	if (dice->isSeed) {
		gfc_list_append(script_player_getplayerdata()->inventory->diceSeeds, dice);
	}
	else {
		gfc_list_append(script_player_getplayerdata()->inventory->diceInventory, dice);
	}

	((ShopUIData*)script->data)->daysLocked = 3;
	script_shopui_hide(entity, script);
}

void buy_dice_1(Entity* entity, Script* script) { buy_dice(entity, script, 0); }
void buy_dice_2(Entity* entity, Script* script) { buy_dice(entity, script, 1); }
void buy_dice_3(Entity* entity, Script* script) { buy_dice(entity, script, 2); }
void buy_dice_4(Entity* entity, Script* script) { buy_dice(entity, script, 3); }

void script_shopui_register_callbacks(Entity* self, Script* script) {
	//	horrifying code
	event_manager_register_callback("button_ui_dice_shop_buy1", &buy_dice_1, self, script);
	event_manager_register_callback("button_ui_dice_shop_buy2", &buy_dice_2, self, script);
	event_manager_register_callback("button_ui_dice_shop_buy3", &buy_dice_3, self, script);
	event_manager_register_callback("button_ui_dice_shop_buy4", &buy_dice_4, self, script);
}
void script_shopui_unregister_callbacks() {
	event_manager_unregister_callback("button_ui_dice_shop_buy1", &buy_dice_1);
	event_manager_unregister_callback("button_ui_dice_shop_buy2", &buy_dice_2);
	event_manager_unregister_callback("button_ui_dice_shop_buy3", &buy_dice_3);
	event_manager_unregister_callback("button_ui_dice_shop_buy4", &buy_dice_4);
}

ShopUIData* script_shopui_newdata() {
	ShopUIData* data = malloc(sizeof(ShopUIData));
	if (!data) {
		slog("Couldn't allocate data for shop ui.");
		return NULL;
	}
	data->state = ShopUIState_HIDDEN;
	data->daysLocked = 0;
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
		//  You aren't supposed to be able to open shop if another menu is open 
		//	OR if the shop is still locked
		if (script_manager_getmetastate() == INMENU ||
			((ShopUIData*)script->data)->daysLocked > 0)
		{
			return;
		}

		((ShopUIData*)script->data)->state = ShopUIState_VISIBLE;
		script_ui_sethidden(entity, false);
		set_sprites(entity, script);
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
	if (!entity || !script || !script->data) return;
	((ShopUIData*)script->data)->state = ShopUIState_VISIBLE;
	script_shopui_toggle(entity, script);
}

void script_shopui_newday(Entity* entity, Script* script) {
	if (!entity || !script || !script->data) return;
	script_shopui_hide(entity, script);

	if (((ShopUIData*)script->data)->daysLocked > 0)	
		((ShopUIData*)script->data)->daysLocked -= 1;
	if (((ShopUIData*)script->data)->daysLocked == 0) {
		update_shop(script);
	}
}

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	script->data = script_shopui_newdata();
	update_shop(script);
	script_shopui_register_callbacks(self, script);
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
	script_shopui_unregister_callbacks();
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_shopui() {
	return script_new("shopui", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
