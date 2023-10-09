#include <stdio.h>

#include "entity.h"
#include "script.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self) {
	printf("\n-------------------\nStarting\n");
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {
	//printf("\n-------------------\nThinking\n");
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
	//printf("\n-------------------\nUpdating\n");
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
	printf("\n-------------------\nDestroying\n");
}

Script* script_new_debug() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
