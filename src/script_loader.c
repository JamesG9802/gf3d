#include <string.h>

#include "script.h"
#include "script_loader.h"

#include "script_defs.h"

Script* script_loader_getScript(char* string) {
	if (strcmp(string, "debug") == 0)
		return script_new_debug();
	return NULL;
}


/*eol@eof*/
