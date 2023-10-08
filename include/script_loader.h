#pragma once
#include "script.h"

/**
 * @brief This class handles strings to script mapping (why dont c have reflection 😔)
 */

/**
 * @brief Checks the string to return the appropriate script.
 * @param string the name of the script
 * @return the script to be attached to an entity.
 */
Script* script_loader_getScript(char* string);

/*eol@eof*/
