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
	if (strcmp(string, "soil") == 0)
		return script_new_soil();
	if (strcmp(string, "billboard") == 0)
		return script_new_billboard();
	if (strcmp(string, "ui") == 0)
		return script_new_ui();
	if (strcmp(string, "manager") == 0)
		return script_new_manager();
	if (strcmp(string, "manager_flag") == 0)
		return script_new_manager_flag();
	if (strcmp(string, "inventoryui") == 0)
		return script_new_inventoryui();
	if (strcmp(string, "dice") == 0)
		return script_new_dice();
	if (strcmp(string, "monster") == 0)
		return script_new_monster();
	return NULL;
}


/*eol@eof*/
