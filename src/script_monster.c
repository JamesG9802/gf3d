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
/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    script->data = script_monster_newmonsterdata();
    int day = script_manager_getdata()->currentDay;
    switch (day) {
    case 6:
        ((MonsterData*)script->data)->cooldown = 2;
        break;
    case 7:
        ((MonsterData*)script->data)->cooldown = 0;
        break;
    case 8:
        ((MonsterData*)script->data)->cooldown = 1;
        break;
    }
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
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
        if (script_manager_getdata()->turn == Monster || script_manager_getdata()->currentDay == 9) {
            ((MonsterData*)script->data)->timeDelta += engine_time_delta();
        }
        else {
            ((MonsterData*)script->data)->timeDelta = 0;
        }
        //  Monster specific behaviour
        int day = script_manager_getdata()->currentDay;
        Vector3D position;
        switch (day) {
        default:
        case 4:
        case 5:
            position = vector3d(0, 1, 0);
            vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
            vector3d_scale(position, position, -30.0 + 30.0 * ((MonsterData*)script->data)->timeDelta);
            vector3d_add(position, position, script_player_getplayer()->position);
            self->position.x = position.x;
            self->position.y = position.y;
            self->position.z = position.z;
            break;
        case 6:
        case 7:
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
            break;
        case 8:
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
            break;
        case 9:
            position = vector3d(0, 1, 0);
            vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
            vector3d_scale(position, position, -30.0 + 30.0 * ((MonsterData*)script->data)->timeDelta);
            vector3d_add(position, position, script_player_getplayer()->position);
            self->position.x = position.x;
            self->position.y = position.y;
            self->position.z = position.z;
        }
       
        if (((MonsterData*)script->data)->timeDelta >= 1) {
            script_manager_getdata()->turn = Player;

            switch (day) {
            default:
            case 4:
                script_player_getplayerdata()->currentHealth -= 5;
                break;
            case 5:
                script_monster_getbattlingmonsterdata()->currentHealth += 2;
                script_player_getplayerdata()->currentHealth -= 5;
                break;
            case 6:
                if (script_monster_getbattlingmonsterdata()->cooldown == 0) {
                    script_monster_getbattlingmonsterdata()->cooldown = 3;
                    script_player_getplayerdata()->currentHealth -= 20;
                }
                script_monster_getbattlingmonsterdata()->cooldown = script_monster_getbattlingmonsterdata()->cooldown - 1;
                break;
            case 7:
                if (script_monster_getbattlingmonsterdata()->cooldown == 0) {
                    script_player_getplayerdata()->currentHealth -= 8;
                    script_monster_getbattlingmonsterdata()->cooldown += 1;
                }
                else {
                    script_monster_getbattlingmonsterdata()->currentHealth += 8;
                    script_monster_getbattlingmonsterdata()->cooldown -= 1;
                }
                break;
            case 8:
                if (script_monster_getbattlingmonsterdata()->cooldown == 0) {
                    script_monster_getbattlingmonsterdata()->cooldown = 2;
                    int mana = script_player_getplayerdata()->currentMana;
                    script_player_getplayerdata()->currentMana = 0;
                    script_player_getplayerdata()->currentHealth -= mana;
                }
                script_monster_getbattlingmonsterdata()->cooldown = script_monster_getbattlingmonsterdata()->cooldown - 1;
                break;
            case 9:
                script_player_getplayerdata()->currentHealth -= 4;
                break;
            }
            ((MonsterData*)script->data)->timeDelta = 0;
        }
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
    }
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
    script_monster_freemonsterdata(script);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_monster() {
    return script_new("monster", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
