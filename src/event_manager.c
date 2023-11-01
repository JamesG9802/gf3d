#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_hashmap.h"

#include "entity.h"
#include "script.h"

#include "event_manager.h"

typedef struct EventManager {
	/// <summary>
	/// A map<char*,List<callback>> of event names to a list of callbacks;
	/// </summary>
	HashMap* events;
} EventManager;

/// <summary>
/// A container for the information needed to register a callback.
/// </summary>
typedef struct EventMapObject {
	Entity* entity;
	Script* script;
	void(*function)(Entity*, Script*);
} EventMapObject;

static EventManager eventManager;

/// <summary>
/// Frees the event manager.
/// </summary>
void event_manager_close(void) {
	List* lists = gfc_hashmap_get_all_values(eventManager.events);
	for (int i = 0; i < gfc_list_get_count(lists); i++) {
		List* callBackList = ((HashElement*)gfc_list_get_nth(lists, i))->data;
		for (int j = 0; j < gfc_list_get_count(callBackList); j++) {
			free(gfc_list_get_nth(callBackList, j));
		}
		gfc_list_delete(callBackList);
	}
	gfc_hashmap_free(eventManager.events);
}

void event_manager_init() {
	eventManager.events = gfc_hashmap_new();
	slog("Initializing event manager.");
	atexit(event_manager_close);
}

void event_manager_register_callback(char* event, void(*function)(Entity*, Script*), Entity* entity, Script* script) {
	if(!gfc_hashmap_get(eventManager.events, event))
		gfc_hashmap_insert(eventManager.events, event, gfc_list_new());
	EventMapObject* object = malloc(sizeof(EventMapObject));
	if (!object)
	{
		slog("Couldn't allocate memory");
		return;
	}
	object->function = function;
	object->entity = entity;
	object->script = script;
	gfc_list_append(gfc_hashmap_get(eventManager.events, event), object);
}

void event_manager_unregister_callback(char* event, void(*function)(Entity*, Script*)) {
	if (gfc_hashmap_get(eventManager.events, event))
	{
		List* list = gfc_hashmap_get(eventManager.events, event);
		int length = gfc_list_get_count(list);
		for (int i = 0; i < length; i++)
		{
			if (((EventMapObject*)gfc_list_get_nth(list, i))->function == function)
			{
				free(gfc_list_get_nth(list, i));
				gfc_list_delete_nth(list, i);
				break;
			}
		}
	}
}

void event_manager_fire_event(char* event) {
	if (gfc_hashmap_get(eventManager.events, event)) {
		int length = gfc_list_get_count(gfc_hashmap_get(eventManager.events, event));
		for (int i = 0; i < length; i++) {
			EventMapObject* object = gfc_list_get_nth(gfc_hashmap_get(eventManager.events, event), i);
			void (*function)(Entity*, Script*) = object->function;
			function(object->entity, object->script);
		}
	}
}