#include <stdio.h>

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

#include "engine_time.h"

#include "entity_bounds.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
//	self->acceleration = vector3d(4, 0, 0);
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
	/*
	slog("%.2lf %.2lf %.2lf %.2lf %.2lf %.2lf",
		self->bounds.x, self->bounds.y, self->bounds.z, self->bounds.w, self->bounds.h, self->bounds.d);
	slog_sync();
	*/
	double timeDelta = engine_time_delta();
	self->rotation.x += GFC_HALF_PI * timeDelta / 16;
	self->rotation.y += GFC_PI * timeDelta / 16;
	self->rotation.z += GFC_PI_HALFPI * timeDelta / 16;
	if (true) return;

	self->position.x += 20 * timeDelta;
	if (self->position.x > 50)
		self->position.x = -50;

	self->position.x += 40 * timeDelta;
	if (self->position.x > 50)
		self->position.x = -100;

}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
}

Script* script_new_rigidbody() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
