#pragma once

#include "gfc_types.h"
#include "gf2d_font.h"
#include "entity.h"

/// <summary>
/// Updates internal values for engine utility.
/// </summary>
void engine_utility_update();

/// <summary>
/// Returns a vector transforming the screen coordinates to normalized device coordinates.
/// </summary>
/// <param name="screen"></param>
/// <returns></returns>
Vector2D engine_utility_screentondc(Vector2D screen);

/// <summary>
/// Returns a vector transforming the normalized device coordinates to screen coordinates/
/// </summary>
/// <param name="ndc"></param>
/// <returns></returns>
Vector2D engine_utility_ndctoscreen(Vector2D ndc);

/// <summary>
/// Returns true if the mouse is over the entity in world space.
/// </summary>
/// <param name="entity"></param>
/// <param name="ray">A pointer to an Edge3D that can be populated with the ray of the mouse.</param>
/// <returns></returns>
Bool engine_utility_ismouseover(Entity* entity, Edge3D* ray);

/// <summary>
/// Returns a vector projected from the mouse to the screen, normalized.
/// </summary>
/// <returns></returns>
Vector3D engine_utility_mouseprojectray();

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
/// When the left mouse button is pressed or released, it clears itself 
/// so no one else will think it is pressed or released
/// </summary>
void engine_utility_leftmouse_consume();

/// <summary>
/// When the right mouse button is pressed or released, it clears itself 
/// so no one else will think it is pressed or released
/// </summary>
void engine_utility_rightmouse_consume();

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
/// Creates and returns a texture with text drawn onto it. Populates width and height if not NULL.
/// </summary>
/// <param name="text"></param>
/// <returns></returns>
Texture* engine_utility_createtexturefromtext(Font* font, const char* text, Color color, 
	int* width, int* height);

/// <summary>
/// Sets the texture of an entity and rescales it so text looks correct.
/// </summary>
/// <param name="entity"></param>
/// <param name="texture"></param>
void engine_utility_settexture(Entity* entity, Texture* texture, int width, int height, int scaleFactor);

/// <summary>
/// Sets the surface of an entity's texture by duplicating the input surface. You can free your SDL_Surface at any time.
/// </summary>
/// <param name="entity"></param>
/// <param name="surface"></param>
void engine_utility_settexturebysurface(Entity* entity, SDL_Surface* surface);
