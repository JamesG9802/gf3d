#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "gf3d_camera.h"

#include "engine_time.h"
#include "engine_utility.h"

#include "entity.h"

#include "script.h"
#include "script_defs.h"

Entity* player = NULL;
static int thirdPersonMode = 0;

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    player = self;
    self->hidden = true;
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
    Vector3D forward, move = { 0 };
    Vector2D w, mouse;
    int mx, my;
    SDL_GetRelativeMouseState(&mx, &my);
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;

    move = vector3d_get_from_angles(self->rotation);

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

    if (keys[SDL_SCANCODE_UP])self->rotation.x += 0.00750;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x -= 0.00750;
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
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {

    if (!self)return;

    gf3d_camera_set_position(self->position);
    gf3d_camera_set_rotation(self->rotation);
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
}
static void Arguments(Entity* self, Script* script, const char** argv, int argc) {
}
Script* script_new_player() {
	return script_new("player", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
