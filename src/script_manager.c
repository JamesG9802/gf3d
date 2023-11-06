#include <stdio.h>

#include "simple_logger.h"

#include "gfc_hashmap.h"

#include "gf2d_font.h"
#include "gf3d_camera.h"

#include "engine_utility.h"
#include "engine_time.h"
#include "event_manager.h"

#include "entity.h"
#include "script.h"

#include "game_state.h"

#include "script_defs.h"
#include "script_manager.h"
#include "script_ui.h"
#include "script_inventoryui.h"
#include "script_player.h"

/// <summary>
/// There can only be a single script_manager.
/// </summary>
static Script* script_manager = NULL;

void day_to_night(Entity* entity, Script* script) {
	script_manager_getdata()->gamestate = BATTLE;
	if (script_manager_getdata()->currentDay > 3) {
		script_ui_sethidden(
			script_manager_getentity("button_timetransition"),
			true
		);
		script_ui_setframenum(
			script_manager_getentity("indicator_time"),
			1
		);
		script_ui_sethidden(
			script_manager_getentity("indicator_health"),
			false
		);
		script_ui_sethidden(
			script_manager_getentity("indicator_mana"),
			false
		);
		script_ui_sethidden(
			script_manager_getentity("indicator_enemyhealth"),
			false
		);
		Entity* entity = entity_load_from_prefab("prefabs/enemy1.prefab", NULL);
		entity->position = vector3d(0, 100, 0);
	}
	//	First three days have no fight
	else {
		event_manager_fire_event("transition_nighttoday");
	}
}
void night_to_day(Entity* entity, Script* script) {
	script_ui_sethidden(
		script_manager_getentity("indicator_health"),
		true
	);
	script_ui_sethidden(
		script_manager_getentity("indicator_mana"),
		true
	);
	script_ui_sethidden(
		script_manager_getentity("indicator_enemyhealth"),
		true
	);
	script_manager_getdata()->currentDay = script_manager_getdata()->currentDay + 1;
	script_manager_getdata()->gamestate = GROW;
	script_ui_sethidden(
		script_manager_getentity("button_timetransition"),
		false
	);
	script_ui_setframenum(
		script_manager_getentity("indicator_time"),
		0
	);

	Inventory* inventory = script_player_getplayerdata()->inventory;
	for (int i = 0; i < gfc_list_get_count(inventory->diceInventory); i++) {
		Dice* dice = gfc_list_get_nth(inventory->diceInventory, i);
		dice->age = dice->age + 1;
		if (dice->age >= dice->maxLifespan);
		{
			dice->isSeed = true;
			gfc_list_delete_nth(inventory->diceInventory, i);
			gfc_list_append(inventory->diceSeeds, dice);
			i--;
		}
	}
	for (int i = 0; i < gfc_list_get_count(inventory->diceLoadout); i++) {
		Dice* dice = gfc_list_get_nth(inventory->diceLoadout, i);
		dice->age = dice->age + 1;
		if (dice->age >= dice->maxLifespan);
		{
			dice->isSeed = true;
			gfc_list_delete_nth(inventory->diceLoadout, i);
			gfc_list_append(inventory->diceSeeds, dice);
			i--;
		}
	}
	script_player_getplayerdata()->currentHealth = script_player_getplayerdata()->maxHealth;
	script_player_getplayerdata()->currentMana = script_player_getplayerdata()->maxMana;
}
void handle_inventory_toggle(Entity* entity, Script* script) {
	Entity* inventory = script_manager_getentity("indicator_inventory");
	
	script_inventoryui_toggle(inventory, entity_get_script(inventory, "inventoryui"));
}

void handle_seed_prompt(Entity* entity, Script* script) {
	Entity* inventory = script_manager_getentity("indicator_inventory");

	script_inventoryui_seedprompt(inventory, entity_get_script(inventory, "inventoryui"));
}

void entercombat(Entity* entity, Script* script) {
	script_manager_getdata()->gamestate = COMBAT;
	script_player_getplayerdata()->selectedDiceIndex = 0;
}

/// <summary>
/// Register all callbacks for events.
/// </summary>
void script_manager_registerCallbacks(Entity* self) {
	event_manager_register_callback("transition_daytonight", &day_to_night, self, script_manager);
	event_manager_register_callback("transition_nighttoday", &night_to_day, self, script_manager);
	event_manager_register_callback("inventoryToggle", &handle_inventory_toggle, self, script_manager);
	event_manager_register_callback("seedPrompt", &handle_seed_prompt, self, script_manager);
	event_manager_register_callback("entercombat", &entercombat, self, script_manager);
}

/// <summary>
/// Unregister all callbacks for events
/// </summary>
void script_manager_unregisterCallbacks() {
	event_manager_unregister_callback("transition_daytonight", &day_to_night);
	event_manager_unregister_callback("transition_nighttoday", &night_to_day);
	event_manager_unregister_callback("inventoryToggle", &handle_inventory_toggle);
	event_manager_unregister_callback("seedPrompt", &handle_seed_prompt);
	event_manager_unregister_callback("entercombat", &entercombat);
}

ManagerData* script_manager_newdata() {
	ManagerData* data = malloc(sizeof(ManagerData));
	if (!data)
	{
		slog("could not allocate memory for game data");
		slog_sync();
		return NULL;
	}
	data->currentDay = 1;
	data->gamestate = GROW;
	data->entities = gfc_hashmap_new();

	return data;
}

void script_manager_freedata(Script* script) {
	if (!script->data) return;
	script_manager_unregisterCallbacks();
	gfc_hashmap_free(((ManagerData*)script->data)->entities);
	free(script->data);
}

Script* script_manager_get() {
	return script_manager;
}

ManagerData* script_manager_getdata() {
	return ((ManagerData*)(script_manager->data));
}

GameState script_manager_getgamestate() {
	if (script_manager && script_manager->data)
		return ((ManagerData*)script_manager->data)->gamestate;
	return INVALID;
}

MetaState script_manager_getmetastate() {
	if (script_manager && script_manager->data)
		return ((ManagerData*)script_manager->data)->metastate;
	return INVALID_META;
}

void script_manager_setgamestate(GameState gamestate) {
	if (script_manager && script_manager->data)
		((ManagerData*)script_manager->data)->gamestate = gamestate;
}
void script_manager_setmetastate(MetaState metastate) {
	if (script_manager && script_manager->data)
		return ((ManagerData*)script_manager->data)->metastate = metastate;
}


void script_manager_flagentity(char* name, Entity* entity) {
	if (!script_manager || !name || !entity) return;
	gfc_hashmap_insert(((ManagerData*)script_manager->data)->entities, name, entity);
}
void script_manager_unflagentity(char* name) {
	if (!script_manager || !name ) return;
	gfc_hashmap_delete_by_key(((ManagerData*)script_manager->data)->entities, name);
}

Entity* script_manager_getentity(char* name) {
	return gfc_hashmap_get(((ManagerData*)script_manager->data)->entities, name);
}

/// <summary>
/// Called when a script is created.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Start(Entity* self, Script* script) {
	if (script_manager)
	{
		slog("Trying to create a script_manager when it already exists");
		return;
	}
	script->data = script_manager_newdata();
	script_manager_registerCallbacks(self);
	script_manager = script;
}

/// <summary>
/// Called every think frame.
/// <param name="Entity*">Attached entity</param>
/// <param name="Script*">Caller script</param>
/// </summary>
static void Think(Entity* self, Script* script) {
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
	script_manager_freedata(script);
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

Script* script_new_manager() {
	return script_new("manager", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
