#pragma once
#include "script.h"
/**
 * @brief Script to test out the scripting system.
 */
Script* script_new_debug();

/**
 * @brief Script to handle player controls.
 */
Script* script_new_player();

/**
 * @brief Script to handle player controls.
 */
Script* script_new_camera();

/**
 * @brief Script to handle collisions.
 */
Script* script_new_rigidbody();

/**
 * @brief Script to create a box collider.
 */
Script* script_new_boxcollider();

/**
 * @brief Script to create a sphere collider.
 */
Script* script_new_spherecollider();

/// <summary>
/// Script to make a soil entity.
/// </summary>
/// <returns></returns>
Script* script_new_soil();

/// <summary>
/// Script to make a billboard entity.
/// </summary>
/// <returns></returns>
Script* script_new_billboard();

/// <summary>
/// Script to make a UI entity. Uses entity->customData.
/// </summary>
/// <returns></returns>
Script* script_new_ui();

/// <summary>
/// Script to make a manager entity.
/// </summary>
/// <returns></returns>
Script* script_new_manager();

/// <summary>
/// Script to make a new manager flag which registers an entity to the manager.
/// </summary>
/// <returns></returns>
Script* script_new_manager_flag();

/// <summary>
/// Script to control the dice inventory UI.
/// </summary>
/// <returns></returns>
Script* script_new_inventoryui();

/*eol@eof*/
