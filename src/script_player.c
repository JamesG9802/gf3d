#include <stdio.h>

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
static void Start(Entity* self) {
    player = self;
    self->hidden = true;
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {
    Vector3D forward = { 0 };
    Vector3D right = { 0 };
    Vector2D w, mouse;
    int mx, my;
    SDL_GetRelativeMouseState(&mx, &my);
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x * engine_time_delta() * 200;
    forward.y = w.y * engine_time_delta() * 200;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x * engine_time_delta() * 200;
    right.y = w.y * engine_time_delta() * 200;
    if (keys[SDL_SCANCODE_W])
    {
        vector3d_add(self->position, self->position, forward);
    }
    if (keys[SDL_SCANCODE_S])
    {
        vector3d_add(self->position, self->position, -forward);
    }
    if (keys[SDL_SCANCODE_D])
    {
        vector3d_add(self->position, self->position, right);
    }
    if (keys[SDL_SCANCODE_A])
    {
        vector3d_add(self->position, self->position, -right);
    }
    if (keys[SDL_SCANCODE_SPACE])self->position.z += 1;
    if (keys[SDL_SCANCODE_Z])self->position.z -= 1;

    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;

    if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    if (keys[SDL_SCANCODE_F3])
    {
        self->hidden = !self->hidden;
    }
    if(gfc_input_keycode_released(SDL_SCANCODE_BACKSPACE))
        self->hidden = !self->hidden;
    if (gfc_input_keycode_released(SDL_SCANCODE_HOME))
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }
    // IsMouseOverEntity(self);
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
	//printf("\n-------------------\nUpdating\n");
    Vector3D forward = { 0 };
    Vector3D position;
    Vector3D rotation;
    Vector2D w;

    if (!self)return;

    vector3d_copy(position, self->position);
    vector3d_copy(rotation, self->rotation);

    if (thirdPersonMode)
    {
        position.z += 100;
        rotation.x += M_PI * 0.125;
        w = vector2d_from_angle(self->rotation.z);
        forward.x = w.x * 100;
        forward.y = w.y * 100;
        vector3d_add(position, position, -forward);
    }

    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);

}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
}

Script* script_new_player() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
