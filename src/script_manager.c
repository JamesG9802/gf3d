#include <stdio.h>

#include "simple_logger.h"

#include "gfc_hashmap.h"

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
#include "script_inventoryui.h"
#include "script_shopui.h"
#include "script_player.h"

/// <summary>
/// There can only be a single script_manager.
/// </summary>
static Script* script_manager = NULL;
static Entity* current_day_entity = NULL;

void day_to_night(Entity* entity, Script* script) {
	script_manager_getdata()->gamestate = BATTLE;
	script_manager_getdata()->turn = Player;

	//	Delete interactable entity
	if (current_day_entity) entity_free(current_day_entity);

	if (script_manager_getdata()->currentDay > 3) {
		//	Show night indicators
		script_ui_setframenum(
			script_manager_getentity("indicator_time"),
			1
		);
		//	Hide day indicators
		script_ui_sethidden(
			script_manager_getentity("button_timetransition"),
			true
		);
		script_inventoryui_hide(
			script_manager_getentity("indicator_inventory"),
			entity_get_script(script_manager_getentity("indicator_inventory"), "inventoryui")
		);
		script_ui_sethidden(
			script_manager_getentity("ui_diceshop"),
			true
		);
		script_shopui_hide(
			script_manager_getentity("indicator_shop"),
			entity_get_script(script_manager_getentity("indicator_shop"), "shopui")
		);
		Entity* entity = NULL;
		switch (script_manager_getdata()->currentDay) {
		default:
		case 4:
			entity = entity_load_from_prefab("prefabs/enemy1.prefab", NULL);
			break;
		case 5:
			entity = entity_load_from_prefab("prefabs/enemy2.prefab", NULL);
			break;
		case 6:
			entity = entity_load_from_prefab("prefabs/enemy3.prefab", NULL);
			break;
		case 7:
			entity = entity_load_from_prefab("prefabs/enemy4.prefab", NULL);
			break;
		case 8:
			entity = entity_load_from_prefab("prefabs/enemy5.prefab", NULL);
			break;
		case 9:
			entity = entity_load_from_prefab("prefabs/boss1.prefab", NULL);
			break;
		}
		entity->position = vector3d(0, 150, 0);
	}
	//	First three days have no fight
	else {
		event_manager_fire_event("transition_nighttoday");
	}
}
void night_to_day(Entity* entity, Script* script) {
	//	Disable Night time indicators
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
	script_ui_sethidden(
		script_manager_getentity("ui_combatdiceinformation"),
		true
	);
	//	Enable Day time indicators
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
	script_inventoryui_hide(
		script_manager_getentity("indicator_inventory"),
		entity_get_script(script_manager_getentity("indicator_inventory"), "inventoryui")
	);
	script_shopui_newday(
		script_manager_getentity("indicator_shop"),
		entity_get_script(script_manager_getentity("indicator_shop"), "shopui")
	);
	script_ui_sethidden(
		script_manager_getentity("ui_diceshop"),
		false
	);

	//	Spawn daily interactable
	switch (script_manager_getdata()->currentDay % 3) {
	default:
	case 0:
		slog("Seeds");
		current_day_entity = entity_load_from_prefab("prefabs/interactable_seeds.prefab", NULL);
		break;
	case 1:
		slog("Trader");
		current_day_entity = entity_load_from_prefab("prefabs/interactable_trader.prefab", NULL);
		break;
	case 2:
		if (script_manager_getdata()->currentDay < 4) break;	//	do not spawn before the player has even harvested a dice yet.
		slog("Monster");
		current_day_entity = entity_load_from_prefab("prefabs/interactable_monster.prefab", NULL);
		break;
	}

	Inventory* inventory = script_player_getplayerdata()->inventory;
	for (int i = 0; i < gfc_list_get_count(inventory->diceInventory); i++) {
		Dice* dice = gfc_list_get_nth(inventory->diceInventory, i);
		dice->age = dice->age + 1;
		if (dice->age >= dice->maxLifespan)
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
		if (dice->age >= dice->maxLifespan)
		{
			dice->isSeed = true;
			gfc_list_delete_nth(inventory->diceLoadout, i);
			gfc_list_append(inventory->diceSeeds, dice);
			i--;
		}
	}
	script_player_getplayerdata()->currentHealth = script_player_getplayerdata()->maxHealth;
	script_player_getplayerdata()->currentMana = script_player_getplayerdata()->maxMana;
	script_player_getplayer()->position = vector3d(-5.5, -240, 30);
	script_player_getplayer()->rotation = vector3d(-.22, 0, 0);

}
void handle_inventory_toggle(Entity* entity, Script* script) {
	Entity* inventory = script_manager_getentity("indicator_inventory");
	
	script_inventoryui_toggle(inventory, entity_get_script(inventory, "inventoryui"));
}
void handle_shop_toggle(Entity* entity, Script* script) {
	Entity* shop = script_manager_getentity("indicator_shop");

	script_shopui_toggle(shop, entity_get_script(shop, "shopui"));
}
void handle_seed_prompt(Entity* entity, Script* script) {
	Entity* inventory = script_manager_getentity("indicator_inventory");

	script_inventoryui_seedprompt(inventory, entity_get_script(inventory, "inventoryui"));
}

void entercombat(Entity* entity, Script* script) {
	script_manager_getdata()->gamestate = COMBAT;
	script_player_getplayerdata()->selectedDiceIndex = 0;
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
	script_ui_sethidden(
		script_manager_getentity("ui_combatdiceinformation"),
		false
	);
}

void playgame(Entity* entity, Script* script) {
	scene_load("config/game.scene");
}

/// <summary>
/// Register all callbacks for events.
/// </summary>
void script_manager_registerCallbacks(Entity* self) {
	event_manager_register_callback("transition_daytonight", &day_to_night, self, script_manager);
	event_manager_register_callback("transition_nighttoday", &night_to_day, self, script_manager);
	event_manager_register_callback("inventoryToggle", &handle_inventory_toggle, self, script_manager);
	event_manager_register_callback("button_ui_dice_shop", &handle_shop_toggle, self, script_manager);
	event_manager_register_callback("seedPrompt", &handle_seed_prompt, self, script_manager);
	event_manager_register_callback("entercombat", &entercombat, self, script_manager);
	event_manager_register_callback("playgame", &playgame, self, script_manager);
}

/// <summary>
/// Unregister all callbacks for events
/// </summary>
void script_manager_unregisterCallbacks() {
	event_manager_unregister_callback("transition_daytonight", &day_to_night);
	event_manager_unregister_callback("transition_nighttoday", &night_to_day);
	event_manager_unregister_callback("inventoryToggle", &handle_inventory_toggle);
	event_manager_unregister_callback("button_ui_dice_shop", &handle_shop_toggle);
	event_manager_unregister_callback("seedPrompt", &handle_seed_prompt);
	event_manager_unregister_callback("entercombat", &entercombat);
	event_manager_unregister_callback("playgame", &playgame);
}

ManagerData* script_manager_newdata() {
	ManagerData* data = malloc(sizeof(ManagerData));
	if (!data)
	{
		slog("could not allocate memory for game data");
		slog_sync();
		return NULL;
	}
	data->turn = Player;
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
	script_manager = NULL;
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
