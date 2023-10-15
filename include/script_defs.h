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

/*eol@eof*/
