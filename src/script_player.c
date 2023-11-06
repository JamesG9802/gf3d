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
static int thirdPersonMode = 0;


PlayerData script_player_newplayerdata() {
    PlayerData playerData = {0};
    playerData.currentHealth = 20;
    playerData.maxHealth = 20;
    playerData.currentMana = 10;
    playerData.maxMana = 10;
    playerData.inventory = inventory_new();
    {
        Dice* dice;
        DiceValue* diceValues = malloc(sizeof(DiceValue) * 6);
        diceValues[0] = dicevalue_new(Mana, 1);
        diceValues[1] = dicevalue_new(Mana, 2);
        diceValues[2] = dicevalue_new(Mana, 3);
        diceValues[3] = dicevalue_new(Mana, 4);
        diceValues[4] = dicevalue_new(Mana, 5);
        diceValues[5] = dicevalue_new(Mana, 6);
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
        diceValues[0] = dicevalue_new(Fire, 4);
        diceValues[1] = dicevalue_new(Fire, 4);
        diceValues[2] = dicevalue_new(Fire, 3);
        diceValues[3] = dicevalue_new(Fire, 3);
        diceValues[4] = dicevalue_new(Heart, 2);
        diceValues[5] = dicevalue_new(Heart, 3);
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
        diceValues[0] = dicevalue_new(Fire, 4);
        diceValues[1] = dicevalue_new(Fire, 4);
        diceValues[2] = dicevalue_new(Fire, 3);
        diceValues[3] = dicevalue_new(Fire, 3);
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
        if (gfc_input_keycode_released(SDL_SCANCODE_ESCAPE))
        {
            event_manager_fire_event("inventoryToggle");
        }
    }
    if(script_manager_getgamestate() == BATTLE) {
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

        if (thirdPersonMode)
        {
            if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
            if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);
        }

        if (keys[SDL_SCANCODE_F3])
        {
            self->hidden = !self->hidden;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_BACKSPACE))
        {
            self->rotation.x = 0;
            self->rotation.z = 0;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_HOME))
        {
            thirdPersonMode = !thirdPersonMode;
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
    else if (script_manager_getgamestate() == COMBAT) {
        Vector3D position = vector3d(-15, 50, 0);
        vector3d_rotate_about_z(&position, self->rotation.z + GFC_HALF_PI);
        vector3d_add(position, position, self->position);
        Vector3D rotation = vector3d(self->rotation.x, self->rotation.y, self->rotation.z - GFC_HALF_PI);
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(rotation);
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
