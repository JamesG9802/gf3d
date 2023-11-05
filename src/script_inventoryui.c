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

#include "engine_time.h"

#include "script_defs.h"
#include "script_manager.h"

#include "script_player.h"
#include "script_ui.h"
#include "script_inventoryui.h"

InventoryUIData* script_inventoryui_newdata() {
    InventoryUIData* data = malloc(sizeof(InventoryUIData));
    if (!data)   return NULL;
    data->state = HIDDEN;
    data->diceIndex = 0;
    return data;
}
void script_inventoryui_freedata(Script* script) {
    if (script) free(script->data);
}

void script_inventoryui_toggle(Entity* entity, Script* script) {
    if (!entity || !script) return;
    switch (((InventoryUIData*)script->data)->state) {

    //  If menus are hidden, show them
    case HIDDEN:
        ((InventoryUIData*)script->data)->state = START;
        script_ui_sethidden(entity, false);

        //  If the player is properly set up, set the child window to the first dice.
        if (script_player_getplayerdata() && script_player_getplayerdata()->inventory &&
            gfc_list_get_count(script_player_getplayerdata()->inventory->diceInventory) > 0)
        {
            script_ui_sethidden(gfc_list_get_nth(entity->children, 0), false);
            dice_to_ui(
                gfc_list_get_nth(script_player_getplayerdata()->inventory->diceInventory, 0),
                gfc_list_get_nth(entity->children, 0));
        }
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
/*
    Dice* dice;
    DiceValue* diceValues = malloc(sizeof(DiceValue) * 6);
    diceValues[0] = dicevalue_new(Mana, 1);
    diceValues[1] = dicevalue_new(Mana, 2);
    diceValues[2] = dicevalue_new(Mana, 3);
    diceValues[3] = dicevalue_new(Fire, 1);
    diceValues[4] = dicevalue_new(Fire, 2);
    diceValues[5] = dicevalue_new(Fire, 3);
    double* sideWeights = malloc(sizeof(double) * 6);
    sideWeights[0] = 1;
    sideWeights[1] = 2;
    sideWeights[2] = 3;
    sideWeights[3] = 4;
    sideWeights[4] = 5;
    sideWeights[5] = 6;
    dice = dice_new(false, 0, 6, diceValues, sideWeights, 30, 10);

    dice_to_ui(dice, script_manager_getentity("indicator_inventory"));
*/
    script->data = script_inventoryui_newdata();
//    Entity* childWindow = entity_load_from_prefab("prefabs/diceinventoryui.prefab", self);
//    script_ui_updateposition(childWindow);
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
