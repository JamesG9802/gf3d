#include <stdio.h>
#include <SDL_image.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"

#include "engine_time.h"

#include "script_defs.h"

static SDL_Surface* src = 0;
static float timeDelta = 0;
/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    src = IMG_Load("images/testworld.png");
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
    timeDelta += 128 * engine_time_delta();

    if (timeDelta > 255)
    {
        timeDelta = 0;
    }
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    if (keys[SDL_SCANCODE_1])
    {
        SDL_LockSurface(src);
        SDL_FillRect(src, NULL, SDL_MapRGB(src->format, 255 - (int)(timeDelta), (int)timeDelta, 50 + (int)(.3 * timeDelta)));
        SDL_UnlockSurface(src);
        SDL_Surface* surface = SDL_DuplicateSurface(src);
        gf3d_texture_free(self->model->texture);
        self->model->texture = gf3d_texture_convert_surface(surface);
        self->model->texture->surface = surface;
    }
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_debug() {
	return script_new("debug", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
