#include <stdio.h>
#include <stdlib.h>

#include "entity.h"
#include "script.h"

Script* script_new(void(*Start)(Entity*), void(*Think)(Entity*), void(*Update)(Entity*), void(*Destroy)(Entity*)) {
	Script* script = calloc(1, sizeof(Script));
	if (!script)	return NULL;
	script->Start = Start;
	script->Think = Think;
	script->Update = Update;
	script->Destroy = Destroy;

	return script;
}

/*eol@eof*/
