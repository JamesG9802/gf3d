#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
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
