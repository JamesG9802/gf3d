#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_input.h"

#include "gf3d_camera.h"

#include "engine_time.h"
#include "engine_utility.h"
#include "event_manager.h"

#include "entity.h"
#include "script.h"
#include "script_defs.h"

#include "dice.h"
#include "dicevalue.h"

#include "inventory.h"

#include "script_player.h"
#include "script_manager.h"

Entity* player = NULL;

void createDiceEntity() {
    PlayerData* data = script_player_getplayerdata();
    if (gfc_list_get_count(data->inventory->diceLoadout) == 0) {
        slog("You don't have dice in your loadout");
        return;
    }
    Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
    if (!dice)
    {
        slog("Couldn't find dice");
        return;
    }
    switch (dice->sideCount) {
    case 4:
        data->diceEntity = entity_load_from_prefab("prefabs/dice4.prefab", player);
        break;
    default:
    case 6:
        data->diceEntity = entity_load_from_prefab("prefabs/dice6.prefab", player);
        break;
    }
    Entity* diceInfoWindow = script_manager_getentity("ui_combatdiceinformation");
    if (!diceInfoWindow) return;
    dice_to_ui(dice, diceInfoWindow);
}

PlayerData script_player_newplayerdata() {
    PlayerData playerData = {0};
    playerData.diceEntity = NULL;
    playerData.selectedDiceIndex = 0;
    playerData.currentHealth = 30;
    playerData.maxHealth = 30;
    playerData.currentMana = 10;
    playerData.maxMana = 10;
    playerData.inventory = inventory_new();
    {
        Dice* dice;
        DiceValue* diceValues = malloc(sizeof(DiceValue) * 6);
        diceValues[0] = dicevalue_new(Mana, 4);
        diceValues[1] = dicevalue_new(Mana, 4);
        diceValues[2] = dicevalue_new(Mana, 4);
        diceValues[3] = dicevalue_new(Mana, 5);
        diceValues[4] = dicevalue_new(Mana, 5);
        diceValues[5] = dicevalue_new(Mana, 5);
        double* sideWeights = malloc(sizeof(double) * 6);
        sideWeights[0] = 1;
        sideWeights[1] = 1;
        sideWeights[2] = 1;
        sideWeights[3] = 1;
        sideWeights[4] = 1;
        sideWeights[5] = 1;
        dice = dice_new(true, 0, 6, diceValues, sideWeights, 10, 0);
        gfc_list_append(playerData.inventory->diceSeeds, dice);
    }
    {
        Dice* dice;
        DiceValue* diceValues = malloc(sizeof(DiceValue) * 6);
        diceValues[0] = dicevalue_new(Fire, 20);
        diceValues[1] = dicevalue_new(Fire, 15);
        diceValues[2] = dicevalue_new(Fire, 10);
        diceValues[3] = dicevalue_new(Fire, 10);
        diceValues[4] = dicevalue_new(Heart, 8);
        diceValues[5] = dicevalue_new(Heart, 8);
        double* sideWeights = malloc(sizeof(double) * 6);
        sideWeights[0] = 1;
        sideWeights[1] = 1;
        sideWeights[2] = 1;
        sideWeights[3] = 1;
        sideWeights[4] = 1;
        sideWeights[5] = 1;
        dice = dice_new(true, 0, 6, diceValues, sideWeights, 10, 0);
        gfc_list_append(playerData.inventory->diceSeeds, dice);
    }
    {
        Dice* dice;
        DiceValue* diceValues = malloc(sizeof(DiceValue) * 4);
        diceValues[0] = dicevalue_new(Fire, 12);
        diceValues[1] = dicevalue_new(Fire, 12);
        diceValues[2] = dicevalue_new(Fire, 10);
        diceValues[3] = dicevalue_new(Fire, 10);
        double* sideWeights = malloc(sizeof(double) * 4);
        sideWeights[0] = 1;
        sideWeights[1] = 1;
        sideWeights[2] = 1;
        sideWeights[3] = 1;
        dice = dice_new(true, 0, 4, diceValues, sideWeights, 10, 0);
        gfc_list_append(playerData.inventory->diceSeeds, dice);
    }
    return playerData;
}

void script_player_freeplayerdata(Script* script) {
    if (script && script->data)
    {
        //  No need to free diceEntity because all entities will be cleaned up with their parent
        inventory_free(((PlayerData*)(script->data))->inventory);
        free(script->data);
    }
}

Entity* script_player_getplayer() {
    return player;
}
PlayerData* script_player_getplayerdata() {
    if(entity_get_script(player, "player"))
        return entity_get_script(player, "player")->data;
    return NULL;
}
/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    if (!player)
    {
        player = self;
    }
    else
    {
        slog("A player already exists.");
        return;
    }
    self->hidden = false;
    script->data = malloc(sizeof(PlayerData));
    if (!script->data) {
        slog("Couldn't allocate memory for player data.");
        return;
    }
    PlayerData playerData = script_player_newplayerdata();
    memcpy(script->data, &playerData, sizeof(PlayerData));
    Vector3D position;
    vector3d_add(position, self->position, vector3d(0, 0, 50));
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(self->rotation);
}

/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
    if (script_manager_getgamestate() == GROW) {
        PlayerData* data = script_player_getplayerdata();
        if (data->diceEntity) {
            entity_free(data->diceEntity);
            data->diceEntity = NULL;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_ESCAPE))
        {
            event_manager_fire_event("inventoryToggle");
        }
    }
    if(script_manager_getgamestate() == BATTLE) {
        PlayerData* data = script_player_getplayerdata();
        if (data->diceEntity) {
            entity_free(data->diceEntity);
            data->diceEntity = NULL;
        }
        Vector3D forward, move = { 0 };
        Vector2D w, mouse;
        int mx, my;
        SDL_GetRelativeMouseState(&mx, &my);
        const Uint8* keys;
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

        mouse.x = mx;
        mouse.y = my;

        move = vector3d_get_from_angles(self->rotation);
        move.z = 0;
        vector3d_scale(move, move, 200 * engine_time_delta());
        if (keys[SDL_SCANCODE_W])
        {
            vector3d_add(self->position, self->position, move);
        }
        if (keys[SDL_SCANCODE_S])
        {
            vector3d_add(self->position, self->position, -move);
        }
        move = vector3d_get_from_angles(self->rotation);
        move.z = 0;
        vector3d_scale(move, move, 200 * engine_time_delta());
        vector3d_rotate_about_z(&move, -GFC_HALF_PI);
        vector3d_normalize(&move);
        if (keys[SDL_SCANCODE_D])
        {
            vector3d_add(self->position, self->position, move);
        }
        if (keys[SDL_SCANCODE_A])
        {
            vector3d_add(self->position, self->position, -move);
        }
        if (keys[SDL_SCANCODE_SPACE])self->position.z += 1;
        if (keys[SDL_SCANCODE_Z])self->position.z -= 1;

    //    if (keys[SDL_SCANCODE_UP])self->rotation.x += 0.00750;
    //    if (keys[SDL_SCANCODE_DOWN])self->rotation.x -= 0.00750;
        if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.00750;
        if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.00750;
        if (keys[SDL_SCANCODE_KP_RIGHTBRACE])self->rotation.y -= 0.00750;
        if (keys[SDL_SCANCODE_KP_LEFTBRACE])self->rotation.y += 0.00750;

        if (self->rotation.x >= (GFC_HALF_PI * .9f)) self->rotation.x = GFC_HALF_PI * .9f;
        if (self->rotation.x <= -(GFC_HALF_PI * .9f))self->rotation.x = -GFC_HALF_PI * .9f;
    }
    if (script_manager_getdata()->turn == Player && script_manager_getgamestate() == COMBAT) {
        Bool changed = false;
        PlayerData* data = script_player_getplayerdata();
        if(!data->diceEntity)
            createDiceEntity();
        if (gfc_input_keycode_released(SDL_SCANCODE_LEFT)) {
            data->selectedDiceIndex = data->selectedDiceIndex - 1;
            changed = true;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_RIGHT)) {
            data->selectedDiceIndex = data->selectedDiceIndex + 1;
            changed = true;
        }
        if (data->selectedDiceIndex < 0) {
            data->selectedDiceIndex = gfc_list_get_count(data->inventory->diceLoadout) - 1;
        }
        else if (data->selectedDiceIndex >= gfc_list_get_count(data->inventory->diceLoadout)) {
            data->selectedDiceIndex = 0;
        }

        if (changed) {
            if (!data->diceEntity) return;
            entity_free(data->diceEntity);
            createDiceEntity();
        }

    }
}

/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {
    if (!self)return;

    if (script_manager_getgamestate() == GROW)
    {
        Vector3D position;
        vector3d_add(position, self->position, vector3d(0, 0, 100));
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(self->rotation);
    }
    else if (script_manager_getgamestate() == BATTLE)
    {
        Vector3D position;
        vector3d_add(position, self->position, vector3d(0, 0, 300));
        Vector3D rotation = vector3d(-1.2, self->rotation.y, self->rotation.z);
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(rotation);
    }
    else if (script_manager_getdata()->turn == Player && script_manager_getgamestate() == COMBAT) {
        Vector3D position = vector3d(-15, 50, 15);
        vector3d_rotate_about_z(&position, self->rotation.z + GFC_HALF_PI);
        vector3d_add(position, position, self->position);
        Vector3D rotation = vector3d(self->rotation.x, self->rotation.y, self->rotation.z - GFC_HALF_PI);
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(rotation);

        PlayerData* data = script_player_getplayerdata();
        if (data->diceEntity) {
            Vector3D position = engine_utility_mouseprojectray();
            vector3d_scale(position, position, 40);
            Vector3D cameraPos = { 0 };
            gf3d_camera_get_position(&cameraPos);
            vector3d_add(position, position, cameraPos);
            data->diceEntity->position = vector3d(position.x, position.y, position.z);

            if (gfc_list_get_count(data->inventory->diceLoadout) == 0) {
                slog("You don't have dice in your loadout");
                return;
            }
            Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
            if (!dice)
            {
                slog("Couldn't find dice");
                return;
            }
            dice_to_texture(dice, data->diceEntity);
        }
        if (engine_utility_isleftmousereleased()) {
            Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
            if (!dice || !data->diceEntity) return;
            dice_activate_effect(dice);
            entity_free(data->diceEntity);
            data->diceEntity = NULL;
            script_manager_getdata()->turn = Monster;
        }
    }
}

/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
    script_player_freeplayerdata(script);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_player() {
	return script_new("player", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
