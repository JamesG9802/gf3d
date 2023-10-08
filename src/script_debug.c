#include <stdio.h>
#include "script.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start() {
	printf("\n-------------------\nStarting\n");
}
/**
 * @brief Called when a script is created.
 */
static void Think() {
	printf("\n-------------------\nThinking\n");
}
/**
 * @brief Called when a script is created.
 */
static void Update() {
	printf("\n-------------------\nUpdating\n");
}
/**
 * @brief Called when a script is created.
 */
static void Destroy() {
	printf("\n-------------------\nDestroying\n");
}

Script* script_new_debug() {
	return script_new(&Start, &Think, &Update, &Destroy);
}


/*eol@eof*/
