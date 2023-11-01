#pragma once

#include "entity.h"
#include "script.h"

/// <summary>
/// Initializes the event manager.
/// </summary>
void event_manager_init();

/// <summary
/// Registers a callback to be called when the specified event is sent.
/// </summary>
/// <param name="event"></param>
/// <param name="function"></param>
void event_manager_register_callback(char* event, void(*function)(Entity*, Script*), Entity* entity, Script* script);

/// <summary>
/// Unregisters a callback from an event.
/// </summary>
/// <param name="event"></param>
/// <param name="function"></param>
void event_manager_unregister_callback(char* event, void(*function)(Entity*, Script*));

/// <summary>
/// Calls every registered callback associated with an event.
/// </summary>
/// <param name="event"></param>
void event_manager_fire_event(char* event);
