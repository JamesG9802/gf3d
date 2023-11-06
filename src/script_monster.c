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

MonsterData* script_monster_newmonsterdata() {
    MonsterData* data = malloc(sizeof(MonsterData));
    if (!data) return NULL;
    data->state = MONSTER_IDLE;
    data->timeDelta = 0;

    return data;
}

void script_monster_freemonsterdata(Script* script) {
    if (!script || !script->data) return;
    free(script->data);
}

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    script->data = script_monster_newmonsterdata();
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
    if (((MonsterData*)script->data)->state == MONSTER_IDLE &&
        vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 100.0) {
        ((MonsterData*)script->data)->state = MONSTER_CHASE;
    }
    else if (((MonsterData*)script->data)->state == MONSTER_CHASE &&
        vector3d_magnitude_between(script_player_getplayer()->position, self->position) < 5.0) {
        ((MonsterData*)script->data)->state = MONSTER_COMBAT;
        event_manager_fire_event("entercombat");
    }
    else if (((MonsterData*)script->data)->state == MONSTER_COMBAT) {
        Vector3D position = vector3d(0, 1, 0);
        vector3d_rotate_about_z(&position, script_player_getplayer()->rotation.z);
        vector3d_scale(position, position, -30.0);
        vector3d_add(position, position, script_player_getplayer()->position);
        self->position.x = position.x;
        self->position.y = position.y;
        self->position.z = position.z;
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
