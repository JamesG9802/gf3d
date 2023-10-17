#include <string.h>

#include "script.h"
#include "script_loader.h"

#include "script_defs.h"

Script* script_loader_getScript(char* string) {
	if (strcmp(string, "debug") == 0)
		return script_new_debug();
	if (strcmp(string, "player") == 0)
		return script_new_player();
	if (strcmp(string, "camera") == 0)
		return script_new_camera();
	if (strcmp(string, "rigidbody") == 0)
		return script_new_rigidbody();
	if (strcmp(string, "boxcollider") == 0)
		return script_new_boxcollider();
	if (strcmp(string, "spherecollider") == 0)
		return script_new_spherecollider();
	return NULL;
}


/*eol@eof*/
