#pragma once

#include "entity.h"
#include "script.h"

/// <summary>
/// Gets the currrent script manager.
/// </summary>
/// <returns></returns>
Script* script_manager_get();

/// <summary>
/// Stores the pointer to the entity with its name as the key.
/// </summary>
/// <param name="name"></param>
/// <param name="entity"></param>
/// <returns></returns>
void script_manager_flagentity(char* name, Entity* entity);

/// <summary>
/// Get an entity if it has been flagged by the manager before.
/// </summary>
/// <param name="name"></param>
/// <returns></returns>
Entity* script_manager_getentity(char* name);
