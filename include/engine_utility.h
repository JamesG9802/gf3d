#pragma once

#include "gfc_types.h"
#include "entity.h"


/// <summary>
/// Returns true if the mouse is over the entity in world space.
/// </summary>
/// <param name="entity"></param>
/// <returns></returns>
Vector3D IsMouseOverEntity(Entity* entity);
