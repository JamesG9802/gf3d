#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "entity.h"
#include "script.h"
#include "script_defs.h"

/**
 * @brief Set when the in-game command line should be visible.
 */
Bool commandModeFlag = false;

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {
	if (gfc_input_command_released("status")) {
		commandModeFlag = !commandModeFlag;
		slog("Command Line visibility: %d", commandModeFlag);
		slog_sync();
	}
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
}

Script* script_new_debug() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
