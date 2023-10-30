#include <stdio.h>
#include <stdlib.h>

#include "gfc_input.h"

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

#include "engine_time.h"
#include "engine_utility.h"

#include "entity_bounds.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
//	self->acceleration = vector3d(4, 0, 0);
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
	engine_utility_ismouseover(self, NULL);
	
	self->selected = engine_utility_ismouseover(self, NULL);
	//slog("%d", self->selected);
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
}
static void Arguments(Entity* self, Script* script, const char** argv, int argc) {
}
Script* script_new_rigidbody() {
	return script_new("rigidbody", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
