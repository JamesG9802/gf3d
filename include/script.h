#pragma once

/**
 * @brief A script describes components that attach to entities.
 */
typedef struct {
	/**
	 * @brief Called when a script is created.
	 */
	void (*Start)();
	/**
	 * @brief Called every think frame.
	 */
	void (*Think)();
	/**
	 * @brief Called every update frame.
	 */
	void (*Update)();
	/**
	 * @brief Called when a script is destroyed.
	 */
	void (*Destroy)();
	void* data;
} Script;


/**
 * @brief DO NOT use this to create a script if you do not know what you are doing. This is used to help create base scripts.
 * @return a new generic script object.
 */
Script* script_new(void(*Start), void(*Think), void(*Update), void(*Destroy));

/*eol@eof*/
