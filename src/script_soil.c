#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "entity.h"

#include "engine_utility.h"

#include "script.h"
#include "script_defs.h"

#define state	*((SoilState*)self->customData)
typedef enum SoilState {
	IDLE,
	CLICKED
} SoilState;

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
	self->customData = malloc(sizeof(SoilState));
	if (!self->customData)
	{
		slog("Couldn't allocate memory.");
		slog_sync();
		return;
	}
	state = IDLE;
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {
	self->selected = engine_utility_ismouseover(self, NULL);

	if (state == IDLE && self->selected && engine_utility_isleftmousereleased()) {
		state = CLICKED;
	}
	else if (state == CLICKED && !self->selected && engine_utility_isleftmousereleased()) {
		state = IDLE;
	}
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
	if (state == CLICKED) {
		self->color = gfc_color(0, 1, 0, 1);
	}
	else {
		self->color = gfc_color(1, 1, 1, 1);
	}
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
	free(self->customData);
}

Script* script_new_soil() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
