#include <stdio.h>
#include <stdlib.h>

#include "simple_logger.h"

#include "entity.h"
#include "script.h"

Script* script_new(
	TextLine name,
	void(*Start)(Entity*, Script_s*),
	void(*Think)(Entity*, Script_s*),
	void(*Update)(Entity*, Script_s*),
	void(*Destroy)(Entity*, Script_s*),
	void(*Arguments)(Entity*, Script_s*, SJson*)
)
{
	Script* script = calloc(1, sizeof(Script));
	if (!script)	return NULL;
	strcpy_s(script->name, sizeof(TextLine), name);
	script->Start = Start;
	script->Think = Think;
	script->Update = Update;
	script->Destroy = Destroy;
	script->Arguments = Arguments;
	return script;
}

/*eol@eof*/
