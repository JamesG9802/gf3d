#include "simple_logger.h"

#include "game_state.h"

#include "script.h"

#include "script_manager.h"

GameState game_state_getstate() {
	Script* manager;
	if (manager = script_manager_get())
	{
		ManagerData* data;
		if (data = (ManagerData*)script_manager_get()->data)
		{
			return data->gamestate;
		}
	}
	slog("Manager does not exist.");
	return INVALID;
}
