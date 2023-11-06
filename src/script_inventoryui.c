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
    int selectedDiceIndex = ((InventoryUIData*)script->data)->selectedDiceIndex;
    List* dices = get_current_dice_inventory(script);
    if (!dices || diceIndex < 0 || diceIndex >= gfc_list_get_count(dices))
    {
        gf2d_sprite_free(((UIData*)diceWindow->customData)->sprite);
        ((UIData*)diceWindow->customData)->sprite = gf2d_sprite_load("images/default.png", 0, 0, 0);
        return;
    }
    //  Shows up to four dice at once.
    if (((InventoryUIData*)script->data)->state == START || ((InventoryUIData*)script->data)->state == SEEDPROMPT)
    {
        dice_to_ui_simplified(dices, diceIndex, selectedDiceIndex, diceWindow);
    }
    //  Shows the dice information for a specific dice
    else if (((InventoryUIData*)script->data)->state == VIEWDICE) {
        dice_to_ui(gfc_list_get_nth(dices, selectedDiceIndex), diceWindow);
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

    //  Looking at multiple simplified dice information, so move by pages of 4 dice at a time.
    if (((InventoryUIData*)script->data)->state == START) {
        ((InventoryUIData*)script->data)->diceIndex = ((InventoryUIData*)script->data)->diceIndex - 4;
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
    }
    //  Looking at dice information, so move by 1 dice at a time
    else if (((InventoryUIData*)script->data)->state == VIEWDICE) {
        ((InventoryUIData*)script->data)->diceIndex = ((InventoryUIData*)script->data)->diceIndex - 1;
        if (((InventoryUIData*)script->data)->diceIndex < 0)
            ((InventoryUIData*)script->data)->diceIndex = gfc_list_get_count(dices) - 1;
    }
    ((InventoryUIData*)script->data)->selectedDiceIndex = ((InventoryUIData*)script->data)->diceIndex;
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
    //  Looking at multiple simplified dice information, so move by pages of 4 dice at a time.
    if (((InventoryUIData*)script->data)->state == START) {
        ((InventoryUIData*)script->data)->diceIndex = ((InventoryUIData*)script->data)->diceIndex + 4;
        if (((InventoryUIData*)script->data)->diceIndex >= gfc_list_get_count(dices))
            ((InventoryUIData*)script->data)->diceIndex = 0;
    }
    //  Looking at dice information, so move by 1 dice at a time
    else if (((InventoryUIData*)script->data)->state == VIEWDICE) {
        ((InventoryUIData*)script->data)->diceIndex = ((InventoryUIData*)script->data)->diceIndex + 1;
        if (((InventoryUIData*)script->data)->diceIndex >= gfc_list_get_count(dices))
            ((InventoryUIData*)script->data)->diceIndex = 0;
    }
    ((InventoryUIData*)script->data)->selectedDiceIndex = ((InventoryUIData*)script->data)->diceIndex;
    set_dice_sprite(script);
}

/// <summary>
/// When the dice information arrow is clicked in the UI, toggle between simplified and detailed dice view mode.
/// </summary>
/// <param name="entity"></param>
/// <param name="script"></param>
static void button_ui_dice_information(Entity* entity, Script* script) {
    if (!entity | !script || !script->data) return;
    if (((InventoryUIData*)script->data)->state == START)
        ((InventoryUIData*)script->data)->state = VIEWDICE;
    else if (((InventoryUIData*)script->data)->state = VIEWDICE)
        ((InventoryUIData*)script->data)->state = START;
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
    event_manager_register_callback("button_ui_dice_information", &button_ui_dice_information, self, script);
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
    event_manager_unregister_callback("button_ui_dice_information", &button_ui_dice_information);
}

InventoryUIData* script_inventoryui_newdata() {
    InventoryUIData* data = malloc(sizeof(InventoryUIData));
    if (!data)   return NULL;
    data->state = HIDDEN;
    data->currentType = SEEDS;
    data->diceIndex = 0;
    data->selectedDiceIndex = 0;
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
        ((InventoryUIData*)script->data)->diceIndex = 0;
        ((InventoryUIData*)script->data)->selectedDiceIndex = 0;
        ((InventoryUIData*)script->data)->currentType = SEEDS;
        script_manager_setmetastate(INMENU);
        set_dice_sprite(script);
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
    //  If in seed prompt, cancel soil accepting
    case SEEDPROMPT:
        ((InventoryUIData*)script->data)->state = HIDDEN;
        script_ui_sethidden(entity, true);
        for (int i = 0; i < gfc_list_get_count(entity->children); i++) {
            script_ui_sethidden(gfc_list_get_nth(entity->children, i), true);
        }
        script_manager_setmetastate(OK);
        event_manager_fire_event("rejectseed");
        break;
    }
}

void script_inventoryui_seedprompt(Entity* entity, Script* script) {
    if (!entity || !script) return;
    script_inventoryui_toggle(entity, script);
    ((InventoryUIData*)script->data)->state = SEEDPROMPT;
    button_ui_show_seeds(entity, script);
    script_ui_sethidden(script_manager_getentity("button_ui_showinventory"), true);
    script_ui_sethidden(script_manager_getentity("button_ui_showloadout"), true);
    script_ui_sethidden(script_manager_getentity("button_ui_diceinformation"), true);
}

Dice* script_inventoryui_getselecteddice(Entity* entity, Script* script) {
    if (!entity || !script) return NULL;
    List* dices = get_current_dice_inventory(script);
    if (!dices) return NULL;

    return gfc_list_get_nth(dices, ((InventoryUIData*)script->data)->selectedDiceIndex);
}

void script_inventoryui_deleteselecteddice(Entity* entity, Script* script) {
    if (!entity || !script) return NULL;
    List* dices = get_current_dice_inventory(script);
    if (!dices) return NULL;
    gfc_list_delete_nth(dices, ((InventoryUIData*)script->data)->selectedDiceIndex);
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
    if (((InventoryUIData*)script->data)->state == START || ((InventoryUIData*)script->data)->state == SEEDPROMPT)
    {
        Bool changingSprite = false;
        int diceIndex = ((InventoryUIData*)script->data)->diceIndex;
        int selectedDiceIndex = ((InventoryUIData*)script->data)->selectedDiceIndex;
        if (gfc_input_keycode_released(SDL_SCANCODE_LEFT)) {
            selectedDiceIndex = selectedDiceIndex - 1;
            changingSprite = true;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_RIGHT)) {
            selectedDiceIndex = selectedDiceIndex + 1;
            changingSprite = true;
        }
        if (changingSprite) {
            List* dices = get_current_dice_inventory(script);
            //  Can't select a dice past the current page or if there aren't any more dice
            if (selectedDiceIndex >= diceIndex + 3 || selectedDiceIndex >= (Sint32)gfc_list_get_count(dices))
                ((InventoryUIData*)script->data)->selectedDiceIndex = diceIndex;
            //  Can't select a dice before the current page
            else if (selectedDiceIndex < diceIndex) {
                int count = 0;
                for (int i = diceIndex; i < gfc_list_get_count(dices) && count < 4; i++) {
                    ((InventoryUIData*)script->data)->selectedDiceIndex = i;
                    count++;
                }
            }
            else
            {
                ((InventoryUIData*)script->data)->selectedDiceIndex = selectedDiceIndex;
            }
            set_dice_sprite(script);
        }
    }
    if (((InventoryUIData*)script->data)->state == SEEDPROMPT) {
        if (gfc_input_keycode_released(SDL_SCANCODE_RETURN)) {
            event_manager_fire_event("acceptseed");
            script_inventoryui_toggle(self, script);
        }
    }
    if (((InventoryUIData*)script->data)->state == START && ((InventoryUIData*)script->data)->currentType == INVENTORY) {
        if (gfc_input_keycode_released(SDL_SCANCODE_RETURN)) {
            int selectedDiceIndex = ((InventoryUIData*)script->data)->selectedDiceIndex;
            List* inventoryDices = get_current_dice_inventory(script);
            if (gfc_list_get_count(inventoryDices) == 0 || 
                selectedDiceIndex < 0 || 
                selectedDiceIndex >= gfc_list_get_count(inventoryDices) ||
                gfc_list_get_count(script_player_getplayerdata()->inventory->diceLoadout) >= 4)
                return;

            Dice* dice = gfc_list_get_nth(inventoryDices, selectedDiceIndex);
            gfc_list_delete_nth(inventoryDices, selectedDiceIndex);
            if (!dice)
            {
                slog("Something went wrong adding dice to loadout.");
                return;
            }
            gfc_list_append(script_player_getplayerdata()->inventory->diceLoadout, dice);
            script_inventoryui_toggle(self, script);
        }
    }
    else if (((InventoryUIData*)script->data)->state == START && ((InventoryUIData*)script->data)->currentType == LOADOUT) {
        if (gfc_input_keycode_released(SDL_SCANCODE_RETURN)) {
            int selectedDiceIndex = ((InventoryUIData*)script->data)->selectedDiceIndex;
            List* loadoutDices = get_current_dice_inventory(script);
            if (gfc_list_get_count(loadoutDices) == 0 ||
                selectedDiceIndex < 0 ||
                selectedDiceIndex >= gfc_list_get_count(loadoutDices))
                return;

            Dice* dice = gfc_list_get_nth(loadoutDices, selectedDiceIndex);
            gfc_list_delete_nth(loadoutDices, selectedDiceIndex);
            if (!dice)
            {
                slog("Something went wrong adding dice to inventory.");
                return;
            }
            gfc_list_append(script_player_getplayerdata()->inventory->diceInventory, dice);
            script_inventoryui_toggle(self, script);
        }
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
    script_inventoryui_freedata(script);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_inventoryui() {
    return script_new("inventoryui", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
