#include <stdio.h>
#include <stdlib.h>

#include "script.h"

Script* script_new(void(*Start), void(*Think), void(*Update), void(*Destroy)) {
	Script* script = calloc(1, sizeof(Script));
	if (!script)	return NULL;
	script->Start = Start;
	script->Think = Think;
	script->Update = Update;
	script->Destroy = Destroy;

	return script;
}

/*eol@eof*/
