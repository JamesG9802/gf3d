#include <stdio.h>

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

#include "entity_bounds.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	self->bounds = malloc(sizeof(Entity_Bounds));
	if (!self->bounds)
	{
		slog("Couldn't allocate entity box bounds.");
		slog_sync();
	}
	Entity_Bounds enb = entity_bounds_new_box(self->position.x - self->scale.x / 2.0,
		self->position.y - self->scale.y / 2.0,
		self->position.z - self->scale.z / 2.0,
		self->scale.x,
		self->scale.y,
		self->scale.z);
	memcpy(self->bounds, &enb, sizeof(Entity_Bounds));
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
	/*
	slog("%.2lf %.2lf %.2lf %.2lf %.2lf %.2lf",
		self->bounds.x, self->bounds.y, self->bounds.z, self->bounds.w, self->bounds.h, self->bounds.d);
	slog_sync();
	*/
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
	free(self->bounds);
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_boxcollider() {
	return script_new("boxcollider", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
