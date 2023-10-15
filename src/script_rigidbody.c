#include <stdio.h>

#include "simple_logger.h"

#include "entity.h"
#include "script.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
	self->bounds = gfc_box(
		self->position.x - self->scale.x / 2.0,
		self->position.y - self->scale.y / 2.0,
		self->position.z - self->scale.z / 2.0,
		self->scale.x, 
		self->scale.y, 
		self->scale.z);
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
