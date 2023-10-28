#include <stdio.h>

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

#include "entity_bounds.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
	self->bounds = malloc(sizeof(Entity_Bounds));
	if (!self->bounds)
	{
		slog("Couldn't allocate entity sphere bounds.");
		slog_sync();
	}
	Entity_Bounds enb = entity_bounds_new_sphere(self->position.x,
		self->position.y,
		self->position.z,
		self->scale.x);
	memcpy(self->bounds, &enb, sizeof(Entity_Bounds));
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
	free(self->bounds);
}

Script* script_new_spherecollider() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
