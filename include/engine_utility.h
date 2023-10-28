#pragma once

#include "gfc_types.h"
#include "gf2d_font.h"
#include "entity.h"

/// <summary>
/// Updates internal values for engine utility.
/// </summary>
void engine_utility_update();

/// <summary>
/// Returns true if the mouse is over the entity in world space.
/// </summary>
/// <param name="entity"></param>
/// <param name="ray">A pointer to an Edge3D that can be populated with the ray of the mouse.</param>
/// <returns></returns>
Bool engine_utility_ismouseover(Entity* entity, Edge3D* ray);

/// <summary>
/// Returns the current bitmask of the mouse.
/// </summary>
/// <returns></returns>
Uint32 engine_utility_ismousedown();

/// <summary>
/// Returns true if the left mouse button is down.
/// </summary>
/// <returns></returns>
Bool engine_utility_isleftmousedown();

/// <summary>
/// Returns true if the right mouse button is down.
/// </summary>
/// <returns></returns>
Bool engine_utility_isrightmousedown();

/// <summary>
/// Returns true if the left mouse button is released.
/// </summary>
/// <returns></returns>
Bool engine_utility_isleftmousereleased();

/// <summary>
/// Returns true if the right mouse button is released.
/// </summary>
/// <returns></returns>
Bool engine_utility_isrightmousereleased();

/// <summary>
/// Creates and returns a quad model with a specified texture.
/// </summary>
/// <returns></returns>
Model* engine_utility_createquadmodel(char* texture_filepath);

/// <summary>
/// Creates and returns a quad entity.
/// </summary>
/// <returns></returns>
Entity* engine_utility_createquad(char* texture_filepath);

/// <summary>
/// Creates and returns a texture with text drawn onto it.
/// </summary>
/// <param name="text"></param>
/// <returns></returns>
Texture* engine_utility_createtexturefromtext(Font* font, const char* text, Color color);

/// <summary>
/// Sets the texture of an entity.
/// </summary>
/// <param name="entity"></param>
/// <param name="texture"></param>
void engine_utility_settexture(Entity* entity, Texture* texture);
