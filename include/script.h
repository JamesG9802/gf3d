#pragma once

#include "entity.h"

/**
 * @brief A script describes components that attach to entities.
 */
typedef struct Script_s {
	/**
	 * @brief Called when a script is created.
	 * @param entity is the caller 
	 */
	void (*Start)(Entity*);
	/**
	 * @brief Called every think frame.
	 * @param entity is the caller 
	 */
	void (*Think)(Entity*);
	/**
	 * @brief Called every update frame.
	 * @param entity is the caller 
	 */
	void (*Update)(Entity*);
	/**
	 * @brief Called when a script is destroyed.
	 * @param entity is the caller 
	 */
	void (*Destroy)(Entity*);
	void* data;
} Script;


/**
 * @brief DO NOT use this to create a script if you do not know what you are doing. This is used to help create base scripts.
 * @return a new generic script object.
 */
Script* script_new(void(*Start)(Entity*), void(*Think)(Entity*), void(*Update)(Entity*), void(*Destroy)(Entity*));

/*eol@eof*/
