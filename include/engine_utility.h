#pragma once

#include "gfc_types.h"
#include "entity.h"


/// <summary>
/// Returns true if the mouse is over the entity in world space.
/// </summary>
/// <param name="entity"></param>
/// <param name="ray">A pointer to an Edge3D that can be populated with the ray of the mouse.</param>
/// <returns></returns>
Bool engine_utility_ismouseover(Entity* entity, Edge3D* ray);
