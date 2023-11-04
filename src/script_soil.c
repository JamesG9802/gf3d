#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "entity.h"

#include "engine_utility.h"

#include "script.h"
#include "script_defs.h"
#include "script_billboard.h"

#define state	*((SoilState*)script->data)
typedef enum SoilState {
	IDLE,
	CLICKED,
	GROWING
} SoilState;

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
	script->data = malloc(sizeof(SoilState));
	if (!script->data)
	{
		slog("Couldn't allocate memory.");
		slog_sync();
		return;
	}
	state = IDLE;

	self->color = gfc_color(1, 1, 1, 1);
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
	self->selected = engine_utility_ismouseover(self, NULL);

	if (state == IDLE && self->selected && engine_utility_isleftmousereleased()) {
		state = CLICKED;
		self->color = gfc_color(0, 1, 1, 1);
		Script* billboard = NULL;
		if (gfc_list_get_count(self->children) == 0) {
			Entity* child = entity_load_from_prefab("prefabs/text.prefab");
			billboard = entity_get_script(child, "billboard");
			if (!billboard) {
				slog("For soil, couldn't create billboard");
				entity_free(child);
				return;
			}
			child->position.x = self->position.x;
			child->position.y = self->position.y;
			child->position.z = self->position.z + 30;
			script_billboard_settext(child, billboard, "PLANT DICE?");
			script_billboard_updatetexture(child, billboard);
			entity_add_child(self, child);
		}
	}
	else if (state == CLICKED && !self->selected && engine_utility_isleftmousereleased()) {
		state = IDLE;
		entity_free(gfc_list_get_nth(self->children, 0));
		gfc_list_delete_nth(self->children, 0);
		self->color = gfc_color(1, 1, 1, 1);
	}
	else if (state == CLICKED && self->selected && engine_utility_isleftmousereleased()) {
		state = GROWING;
		self->color = gfc_color(1, 0, 0, 1);
		entity_free(gfc_list_get_nth(self->children, 0));
		gfc_list_delete_nth(self->children, 0);
		self->color = gfc_color(1, 0, 0, 1);
	}
}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {

}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
	free(script->data);
}

static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_soil() {
	return script_new("soil", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
