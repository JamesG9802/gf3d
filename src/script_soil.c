#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "entity.h"

#include "engine_utility.h"
#include "event_manager.h"

#include "script.h"
#include "script_defs.h"
#include "script_soil.h"
#include "script_inventoryui.h"
#include "script_billboard.h"
#include "script_manager.h"
#include "script_player.h"

static void cancel_plant(Entity* entity, Script* script);

static void plant_dice(Entity* entity, Script* script) {
	Entity* inventory = script_manager_getentity("indicator_inventory");
	((SoilData*)script->data)->dice =
		script_inventoryui_getselecteddice(inventory, entity_get_script(inventory, "inventoryui"));
	//	If no dice exists
	if (((SoilData*)script->data)->dice == NULL)
	{
		cancel_plant(entity, script);
		return;
	}
	script_inventoryui_deleteselecteddice(inventory, entity_get_script(inventory, "inventoryui"));
	((SoilData*)script->data)->state = GROWING;
	((SoilData*)script->data)->daysRemaining = 3;
	entity->color = gfc_color(1, 0, 0, 1);
	event_manager_unregister_callback("acceptseed", &plant_dice);
	event_manager_unregister_callback("rejectseed", &cancel_plant);
}
static void cancel_plant(Entity* entity, Script* script) {
	((SoilData*)script->data)->state = IDLE;
	entity->color = gfc_color(1, 1, 1, 1);

	event_manager_unregister_callback("acceptseed", &plant_dice);
	event_manager_unregister_callback("rejectseed", &cancel_plant);
}
static void newday(Entity* entity, Script* script) {
	if (((SoilData*)script->data)->state == GROWING) {
		((SoilData*)script->data)->daysRemaining = ((SoilData*)script->data)->daysRemaining - 1;
		entity->color = gfc_color(1 * (((SoilData*)script->data)->daysRemaining /3.0), 
			(3 - ((SoilData*)script->data)->daysRemaining) / 3.0, 0, 1);
		if (((SoilData*)script->data)->daysRemaining == 0) {
			((SoilData*)script->data)->state = READY;
		}
	}
}

SoilData* script_soil_newdata() {
	SoilData* data= malloc(sizeof(SoilData));
	if (!data) return NULL;
	data->state = IDLE;
	data->dice = NULL;
	data->daysRemaining = 0;
	return data;
}

/// <summary>
/// Frees a soil data for a script
/// </summary>
/// <param name="data"></param>
void script_soil_freedata(Script* script) {
	if (!script || !script->data) return;
	dice_free(((SoilData*)script->data)->dice);
	free(script->data);
}


/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	script->data = script_soil_newdata();
	self->selectedColor = gfc_color(0, 1, 0, 1);
	self->color = gfc_color(1, 1, 1, 1);
	event_manager_register_callback("transition_nighttoday", &newday, self, script);
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
	if (script_manager_getgamestate() != GROW) return;
	self->selected = (((SoilData*)script->data)->state != GROWING) && engine_utility_ismouseover(self, NULL);

	if (((SoilData*)script->data)->state == IDLE && self->selected && engine_utility_isleftmousereleased()) {
		event_manager_fire_event("seedPrompt");
		event_manager_register_callback("acceptseed", &plant_dice, self, script);
		event_manager_register_callback("rejectseed", &cancel_plant, self, script);
		((SoilData*)script->data)->state = CLICKED;
		self->color = gfc_color(0, 1, 1, 1);
	}
	if (((SoilData*)script->data)->state == READY && self->selected && engine_utility_isleftmousereleased()) {
		if (((SoilData*)script->data)->dice == NULL) {
			slog("Somehow, this soil has been planted with no dice.");
			slog_sync();
			return;
		}
		gfc_list_append(script_player_getplayerdata()->inventory->diceInventory, ((SoilData*)script->data)->dice);
		((SoilData*)script->data)->state = IDLE;
		self->color = gfc_color(1, 1, 1, 1);
		((SoilData*)script->data)->dice = NULL;
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
	script_soil_freedata(script);
	event_manager_unregister_callback("transition_nighttoday", &newday);
}

static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_soil() {
	return script_new("soil", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
