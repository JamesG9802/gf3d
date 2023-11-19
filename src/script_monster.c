#include <stdio.h>
#include <SDL_image.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"
#include "dice.h"
#include "dicevalue.h"

#include "engine_time.h"
#include "event_manager.h"

#include "script_defs.h"
#include "script_manager.h"
#include "script_monster.h"
#include "script_player.h"

static Entity* battlingmonster = NULL;

MonsterData* script_monster_newmonsterdata() {
    MonsterData* data = malloc(sizeof(MonsterData));
    if (!data) return NULL;
    data->state = MONSTER_IDLE;
    data->controller = MONSTER1;
    data->timeDelta = 0;
    data->currentHealth = 30;
    data->maxHealth = 30;
    data->cooldown = 0;
    return data;
}

void script_monster_freemonsterdata(Script* script) {
    if (!script || !script->data) return;
    free(script->data);
}

Entity* script_monster_getbattlingmonster() {
    return battlingmonster;
}

MonsterData* script_monster_getbattlingmonsterdata() {
    if (!battlingmonster || !entity_get_script(battlingmonster, "monster") 
        || !entity_get_script(battlingmonster, "monster")->data) 
        return NULL;
    return entity_get_script(battlingmonster, "monster")->data;
}

/// <summary>
/// Makes the monster look at the player
/// </summary>
/// <param name="self"></param>
void script_monster_look_at(Entity* self) {
    if (!self || !script_player_getplayer()) return;
    Vector3D direction;
    vector3d_sub(direction, script_player_getplayer()->position, self->position);
    vector3d_angles(direction, &self->rotation);

    //  model's likely need to be normalized to face forward.
    self->rotation.z += GFC_HALF_PI;
}

//  Monster AI think logic
void script_monster_ai_t_m1(Entity* self, Script* script) {
    Vector3D position;
    position = vector3d(0, 1, 0);
    vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
    vector3d_scale(position, position, -30.0 + 30.0 * ((MonsterData*)script->data)->timeDelta);
    vector3d_add(position, position, script_player_getplayer()->position);
    self->position.x = position.x;
    self->position.y = position.y;
    self->position.z = position.z;
}
void script_monster_ai_t_m2(Entity* self, Script* script) {
    script_monster_ai_t_m1(self, script);
}
void script_monster_ai_t_m3(Entity* self, Script* script) {
    Vector3D position;
    if (script_monster_getbattlingmonsterdata()->cooldown != 0)
    {
        position = vector3d(0, 1, 0);
        vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
        vector3d_scale(position, position, -30.0);
        vector3d_add(position, position, script_player_getplayer()->position);
        self->position.x = position.x;
        self->position.y = position.y;
        self->position.z = position.z;
    }
    else
    {
        ((MonsterData*)script->data)->timeDelta = ((MonsterData*)script->data)->timeDelta + 5 * engine_time_delta();
        position = vector3d(0, 1, 0);
        vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
        vector3d_scale(position, position, -30.0 + 30.0 * ((MonsterData*)script->data)->timeDelta);
        vector3d_add(position, position, script_player_getplayer()->position);
        self->position.x = position.x;
        self->position.y = position.y;
        self->position.z = position.z;
    }
}
void script_monster_ai_t_m4(Entity* self, Script* script) {
    script_monster_ai_t_m3(self, script);
}
void script_monster_ai_t_m5(Entity* self, Script* script) {
    Vector3D position;
    if (script_monster_getbattlingmonsterdata()->cooldown != 0)
    {
        position = vector3d(0, 1, 0);
        vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
        vector3d_scale(position, position, -30.0);
        vector3d_add(position, position, script_player_getplayer()->position);
        vector3d_add(position, position, vector3d(0, 0, 5 * sinf(engine_time_since_start()) + 5));
        self->position.x = position.x;
        self->position.y = position.y;
        self->position.z = position.z;
    }
    else
    {
        ((MonsterData*)script->data)->timeDelta = ((MonsterData*)script->data)->timeDelta + 5 * engine_time_delta();
        position = vector3d(0, 1, 0);
        vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
        vector3d_scale(position, position, -30.0 + 30.0 * ((MonsterData*)script->data)->timeDelta);
        vector3d_add(position, position, script_player_getplayer()->position);
        self->position.x = position.x;
        self->position.y = position.y;
        self->position.z = position.z;
    }
}
void script_monster_ai_t_b1(Entity* self, Script* script) {
    Vector3D position;
    position = vector3d(0, 1, 0);
    vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
    vector3d_scale(position, position, -30.0 + 30.0 * ((MonsterData*)script->data)->timeDelta);
    vector3d_add(position, position, script_player_getplayer()->position);
    self->position.x = position.x;
    self->position.y = position.y;
    self->position.z = position.z;
}

//  Monster AI attack execution logic
void script_monster_ai_x_m1(Entity* self, Script* script) {
    script_player_getplayerdata()->currentHealth -= 5;
}
void script_monster_ai_x_m2(Entity* self, Script* script) {
    script_monster_getbattlingmonsterdata()->currentHealth += 2;
    script_player_getplayerdata()->currentHealth -= 5;
}
void script_monster_ai_x_m3(Entity* self, Script* script) {
    if (script_monster_getbattlingmonsterdata()->cooldown == 0) {
        script_monster_getbattlingmonsterdata()->cooldown = 3;
        script_player_getplayerdata()->currentHealth -= 20;
    }
    script_monster_getbattlingmonsterdata()->cooldown = script_monster_getbattlingmonsterdata()->cooldown - 1;
}
void script_monster_ai_x_m4(Entity* self, Script* script) {
    if (script_monster_getbattlingmonsterdata()->cooldown == 0) {
        script_player_getplayerdata()->currentHealth -= 8;
        script_monster_getbattlingmonsterdata()->cooldown += 1;
    }
    else {
        script_monster_getbattlingmonsterdata()->currentHealth += 8;
        script_monster_getbattlingmonsterdata()->cooldown -= 1;
    }
}
void script_monster_ai_x_m5(Entity* self, Script* script) {
    if (script_monster_getbattlingmonsterdata()->cooldown == 0) {
        script_monster_getbattlingmonsterdata()->cooldown = 2;
        int mana = script_player_getplayerdata()->currentMana;
        script_player_getplayerdata()->currentMana = 0;
        script_player_getplayerdata()->currentHealth -= mana;
    }
    script_monster_getbattlingmonsterdata()->cooldown = script_monster_getbattlingmonsterdata()->cooldown - 1;
}
void script_monster_ai_x_b1(Entity* self, Script* script) {
    script_player_getplayerdata()->currentHealth -= 4;
}

/// <summary>
/// Runs the think for each different monster.
/// </summary>
void script_monster_aithink(Entity* self, Script* script) {
    if (script_manager_getdata()->turn == Monster || ((MonsterData*)script->data)->controller == BOSS1) {
        ((MonsterData*)script->data)->timeDelta += engine_time_delta();
    }
    else {
        ((MonsterData*)script->data)->timeDelta = 0;
    }
    //  Monster specific behaviour
    Vector3D position;
    switch (((MonsterData*)script->data)->controller) {
    default:
    case MONSTER1:
        script_monster_ai_t_m1(self, script);
        break;
    case MONSTER2:
        script_monster_ai_t_m2(self, script);
        break;
    case MONSTER3:
        script_monster_ai_t_m3(self, script);
        break;
    case MONSTER4:
        script_monster_ai_t_m4(self, script);
        break;
    case MONSTER5:
        script_monster_ai_t_m5(self, script);
        break;
    case BOSS1:
        script_monster_ai_t_b1(self, script);
        break;
    }
    if (((MonsterData*)script->data)->timeDelta >= 1) {
        script_manager_getdata()->turn = Player;
        switch (((MonsterData*)script->data)->controller) {
        default:
        case MONSTER1:
            script_monster_ai_x_m1(self, script);
            break;
        case MONSTER2:
            script_monster_ai_x_m2(self, script);
            break;
        case MONSTER3:
            script_monster_ai_x_m3(self, script);
            break;
        case MONSTER4:
            script_monster_ai_x_m4(self, script);
            break;
        case MONSTER5:
            script_monster_ai_x_m5(self, script);
            break;
        case BOSS1:
            script_monster_ai_x_b1(self, script);
            break;
        }
        ((MonsterData*)script->data)->timeDelta = 0;
    }
}


/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    if (script && !script->data)
    {
        script->data = script_monster_newmonsterdata();
        ((MonsterData*)script->data)->controller = MONSTER1;
    }
    else if (script && !script->data)
        script->data = script_monster_newmonsterdata();
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
    if (!script->data)   return;

    if (((MonsterData*)script->data)->currentHealth <= 0) {
        battlingmonster = NULL;
        entity_free(self);
        Dice* dice = dice_seed_reward(script_manager_getdata()->currentDay);
        gfc_list_append(script_player_getplayerdata()->inventory->diceSeeds, dice);
        event_manager_fire_event("transition_nighttoday");
    }
    if (script_manager_getgamestate() == BATTLE && ((MonsterData*)script->data)->state == MONSTER_IDLE &&
        vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 100.0) {
        ((MonsterData*)script->data)->state = MONSTER_CHASE;
    }
    else if (script_manager_getgamestate() == BATTLE && ((MonsterData*)script->data)->state == MONSTER_CHASE &&
        vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 5.0) {
        ((MonsterData*)script->data)->state = MONSTER_COMBAT;
        battlingmonster = self;
        event_manager_fire_event("entercombat");
    }
    else if (script_manager_getgamestate() == COMBAT && ((MonsterData*)script->data)->state == MONSTER_COMBAT) {
        //  Look at player
        script_monster_look_at(self);
        script_monster_aithink(self, script);
    }
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {
    if (((MonsterData*)script->data)->state == MONSTER_CHASE) {
        Vector3D towardPlayer;
        vector3d_sub(towardPlayer, script_player_getplayer()->position, self->position);
        vector3d_normalize(&towardPlayer);
        vector3d_scale(towardPlayer, towardPlayer, engine_time_delta() * 100);
        vector3d_add(self->position, self->position, towardPlayer);
        script_monster_look_at(self);
    }
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
    script_monster_freemonsterdata(script);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
    if (!self || !json) return;
    if (sj_get_string_value(sj_object_get_value(json, "ai"))) {
        script->data = script_monster_newmonsterdata();
        MonsterData* data = script->data;
        if (strcmp(sj_get_string_value(sj_object_get_value(json, "ai")), "monster1") == 0) {
            data->controller = MONSTER1;
        }
        else if (strcmp(sj_get_string_value(sj_object_get_value(json, "ai")), "monster2") == 0) {
            data->controller = MONSTER2;
        }
        else if (strcmp(sj_get_string_value(sj_object_get_value(json, "ai")), "monster3") == 0) {
            data->controller = MONSTER3;
        }
        else if (strcmp(sj_get_string_value(sj_object_get_value(json, "ai")), "monster4") == 0) {
            data->controller = MONSTER4;
        }
        else if (strcmp(sj_get_string_value(sj_object_get_value(json, "ai")), "monster5") == 0) {
            data->controller = MONSTER5;
        }
        else if (strcmp(sj_get_string_value(sj_object_get_value(json, "ai")), "boss1") == 0) {
            data->controller = BOSS1;
        }
    }
    if (sj_get_integer_value(sj_object_get_value(json, "cooldown"), NULL)) {
        if (script->data) {
            sj_get_integer_value(sj_object_get_value(json, "cooldown"), &((MonsterData*)script->data)->cooldown);
        }
    }
}
Script* script_new_monster() {
    return script_new("monster", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
