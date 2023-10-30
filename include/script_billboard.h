#pragma once

#include "entity.h"
#include "script.h"

/// <summary>
/// Sets the text of the billboard.
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
/// <param name="text"></param>
void script_billboard_settext(Entity* self, Script* script, const char* text);

/// <summary>
/// Tells the entity to update their texture.
/// </summary>
/// <param name="self"></param>
/// <param name="script"></param>
void script_billboard_updatetexture(Entity* self, Script* script);
