#include <stdio.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"
#include "dice.h"
#include "dicevalue.h"

#include "engine_utility.h"
#include "engine_time.h"
#include "event_manager.h"

#include "script_defs.h"
#include "script_manager.h"

#include "script_player.h"
#include "script_ui.h"
#include "script_inventoryui.h"

List* get_current_dice_inventory(Script* script) {
    if (!script || !script->data) return NULL;

    List* dices = NULL;
    switch (((InventoryUIData*)script->data)->currentType) {
    case SEEDS:
        dices = script_player_getplayerdata()->inventory->diceSeeds;
        break;
    case INVENTORY:
        dices = script_player_getplayerdata()->inventory->diceInventory;
        break;
    case LOADOUT:
        dices = script_player_getplayerdata()->inventory->diceLoadout;
        break;
    }
    return dices;
}

/// <summary>
/// Sets the sprite for the dice window UI.
/// </summary>
/// <param name="script"></param>
static void set_dice_sprite(Script* script) {
    Entity* diceWindow = script_manager_getentity("ui_diceinformation");
    if (script && script->data && gfc_list_get_count(get_current_dice_inventory(script)) == 0) {
        gf2d_sprite_free(((UIData*)diceWindow->customData)->sprite);
        ((UIData*)diceWindow->customData)->sprite = gf2d_sprite_load("images/panel.png", 0, 0, 0);
        return;
    }

    if (!script_player_getplayerdata() || !script_player_getplayerdata()->inventory 
        || !diceWindow || !script || !script->data)  return;

    int diceIndex = ((InventoryUIData*)script->data)->diceIndex;
    List* dices = get_current_dice_inventory(script);
    if (!dices || diceIndex < 0 || diceIndex >= gfc_list_get_count(dices))
    {
        gf2d_sprite_free(((UIData*)diceWindow->customData)->sprite);
        ((UIData*)diceWindow->customData)->sprite = gf2d_sprite_load("images/default.png", 0, 0, 0);
        return;
    }
    //  Shows up to four dice at once.
    if (((InventoryUIData*)script->data)->state == START)
    {
        dice_to_ui_simplified(dices, diceIndex, diceWindow);
    }
    //  Shows the dice information for a specific dice
    else if (((InventoryUIData*)script->data)->state == VIEWDICE) {
        dice_to_ui(gfc_list_get_nth(dices, diceIndex), diceWindow);
    }
}

static void button_ui_show_seeds(Entity* entity, Script* script) {
    if (!entity || !script || !script->data) return;
    ((InventoryUIData*)script->data)->diceIndex = 0;
    ((InventoryUIData*)script->data)->currentType = SEEDS;
    set_dice_sprite(script);
}

static void button_ui_show_inventory(Entity* entity, Script* script) {
    if (!entity || !script || !script->data) return;
    ((InventoryUIData*)script->data)->diceIndex = 0;
    ((InventoryUIData*)script->data)->currentType = INVENTORY;
    set_dice_sprite(script);
}

static void button_ui_show_loadout(Entity* entity, Script* script) {
    if (!entity || !script || !script->data) return;
    ((InventoryUIData*)script->data)->diceIndex = 0;
    ((InventoryUIData*)script->data)->currentType = LOADOUT;
    set_dice_sprite(script);
}

/// <summary>
/// When the left arrow is clicked in the UI, cycle downwards and loop back to the beginning when past 0.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
static void button_ui_left_arrow(Entity* entity, Script* script) {
    if (!entity || !script || !script->data) return;
    List* dices = get_current_dice_inventory(script);
    if (!dices) return;
    ((InventoryUIData*)script->data)->diceIndex = ((InventoryUIData*)script->data)->diceIndex -= 4;
    if (((InventoryUIData*)script->data)->diceIndex < 0)
    {
        if ((Sint32)gfc_list_get_count(dices) - 4 < 0)
        {
            ((InventoryUIData*)script->data)->diceIndex = 0;
        }
        else
        {
            ((InventoryUIData*)script->data)->diceIndex = gfc_list_get_count(dices) - 4;
        }
    }
    set_dice_sprite(script);
}

/// <summary>
/// When the right arrow is clicked in the UI, cycle upwards and loop back to the beginning when past length - 1.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
static void button_ui_right_arrow(Entity* entity, Script* script) {
    if (!entity || !script || !script->data) return;
    List* dices = get_current_dice_inventory(script);
    if (!dices) return;
    ((InventoryUIData*)script->data)->diceIndex = ((InventoryUIData*)script->data)->diceIndex += 4;
    if (((InventoryUIData*)script->data)->diceIndex >= gfc_list_get_count(dices))
        ((InventoryUIData*)script->data)->diceIndex = 0;
    set_dice_sprite(script);
}

/// <summary>
/// Register inventory ui events
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
static void script_inventoryui_registerCallbacks(Entity* self, Script* script) {
    event_manager_register_callback("button_ui_show_seeds", &button_ui_show_seeds, self, script);
    event_manager_register_callback("button_ui_show_inventory", &button_ui_show_inventory, self, script);
    event_manager_register_callback("button_ui_show_loadout", &button_ui_show_loadout, self, script);
    event_manager_register_callback("button_ui_left_arrow", &button_ui_left_arrow, self, script);
    event_manager_register_callback("button_ui_right_arrow", &button_ui_right_arrow, self, script);
}

/// <summary>
/// Unregister inventory ui events
/// </summary>
static void script_inventoryui_unregisterCallbacks() {
    event_manager_unregister_callback("button_ui_show_seeds", &button_ui_show_seeds);
    event_manager_unregister_callback("button_ui_show_inventory", &button_ui_show_inventory);
    event_manager_unregister_callback("button_ui_show_loadout", &button_ui_show_loadout);
    event_manager_unregister_callback("button_ui_left_arrow", &button_ui_left_arrow);
    event_manager_unregister_callback("button_ui_right_arrow", &button_ui_right_arrow);
}

InventoryUIData* script_inventoryui_newdata() {
    InventoryUIData* data = malloc(sizeof(InventoryUIData));
    if (!data)   return NULL;
    data->state = HIDDEN;
    data->currentType = SEEDS;
    data->diceIndex = 0;
    return data;
}
void script_inventoryui_freedata(Script* script) {
    if (script) free(script->data);
    script_inventoryui_unregisterCallbacks();
}

void script_inventoryui_toggle(Entity* entity, Script* script) {
    if (!entity || !script) return;
    switch (((InventoryUIData*)script->data)->state) {

    //  If menus are hidden, show them
    case HIDDEN:
        ((InventoryUIData*)script->data)->state = START;
        script_ui_sethidden(entity, false);
        for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
            script_ui_sethidden(gfc_list_get_nth(entity->children, i), false);
        }
        set_dice_sprite(script);
        script_manager_setmetastate(INMENU);
        break;
    //  If menus are showing, hide them
    case VIEWDICE:
    case START:
        ((InventoryUIData*)script->data)->state = HIDDEN;
        script_ui_sethidden(entity, true);
        for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
            script_ui_sethidden(gfc_list_get_nth(entity->children, i), true);
        }
        script_manager_setmetastate(OK);
        break;
    }
}

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    script->data = script_inventoryui_newdata();
    script_inventoryui_registerCallbacks(self, script);
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
    script_inventoryui_freedata(script);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_inventoryui() {
    return script_new("inventoryui", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
