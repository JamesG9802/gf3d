#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_list.h"
#include "gfc_types.h"
#include "gfc_config.h"

#include "gf3d_model.h"

#include "entity.h"
#include "entity_bounds.h"

#include "engine_time.h"
#include "script.h"
#include "script_loader.h"


EntityManager entity_manager = {0};

void entity_system_close()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        entity_free(&entity_manager.entity_list[i]);        
    }
    free(entity_manager.entity_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_system_init(Uint32 maxEntities)
{
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;
    atexit(entity_system_close);
    slog("entity_system initialized");
}

Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.entity_list[i].modelMat);
            entity_manager.entity_list[i].scale.x = 1;
            entity_manager.entity_list[i].scale.y = 1;
            entity_manager.entity_list[i].scale.z = 1;
            
            entity_manager.entity_list[i].color = gfc_color(1,1,1,1);
            entity_manager.entity_list[i].selectedColor = gfc_color(1,1,1,1);

            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}


void entity_setup(Entity* self, Model* model, Vector3D scale, Vector3D position, Vector3D rotation, List* scripts) {
    if (!self)   return;
    self->model = model;
    self->scale = scale;
    self->position = position;
    self->rotation = rotation;
    self->scripts = scripts;
    self->children = gfc_list_new();
    if (scripts)
    {
        for (Uint32 i = 0; i < gfc_list_get_count(self->scripts); i++)
        {
            Script* script = (Script*)gfc_list_get_nth(self->scripts, i);
            if (!script) continue;
            if (script->Start)
                script->Start(self, script);
        }
    }
}


void entity_free(Entity *self)
{
    if (!self)return;
    if (self->free)self->free(self);
    if (self->scripts) {
        for (Uint32 i = 0; i < gfc_list_get_count(self->scripts); i++)
        {
            Script* script = (Script*)gfc_list_get_nth(self->scripts, i);
            if (!script) continue;
            if (script->Destroy)
                script->Destroy(self, script);
            free(script);
        }
        gfc_list_delete(self->scripts);
    }
    if (self->children) {
        for (Uint32 i = 0; i < gfc_list_get_count(self->children); i++)
        {
            entity_free(gfc_list_get_nth(self->children, i));
        }
        gfc_list_delete(self->children);
    }
    //MUST DESTROY
    gf3d_model_free(self->model);
    memset(self,0,sizeof(Entity));
}


void entity_draw(Entity *self)
{
    if (!self)return;
    if (self->hidden)return;
    gf3d_model_draw(self->model, self->modelMat, gfc_color_to_vector4f(self->color), vector4d(1, 1, 1, 1));
    if (self->selected)
    {
        gf3d_model_draw_highlight(
            self->model,
            self->modelMat,
            gfc_color_to_vector4f(self->selectedColor));
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.entity_list[i]);
    }
}

void entity_think(Entity *self)
{
    if (!self)return;
    if (self->scripts) {
        for (Uint32 i = 0; i < gfc_list_get_count(self->scripts); i++)
        {
            Script* script = (Script*)gfc_list_get_nth(self->scripts, i);
            if (!script) continue;
            if (script->Think)
                script->Think(self, script);
        }
    }
}

void entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.entity_list[i]);
    }
}


void entity_update(Entity *self)
{
    if (!self)return;
    // HANDLE ALL COMMON UPDATE STUFF
    if (!self->skipCommonUpdate)
    {
        double timeDelta = engine_time_delta();

        Vector3D acceleration = self->acceleration;
        acceleration.x *= .5 * timeDelta;
        acceleration.y *= .5 * timeDelta;
        acceleration.z *= .5 * timeDelta;


        vector3d_add(self->velocity, acceleration, self->velocity);
        Vector3D velocity = self->velocity;
        velocity.x *= timeDelta;
        velocity.y *= timeDelta;
        velocity.z *= timeDelta;
        vector3d_add(self->position, self->position, velocity);
        vector3d_add(self->velocity, acceleration, self->velocity);

        gfc_matrix_identity(self->modelMat);

        gfc_matrix_scale(self->modelMat, self->scale);
        gfc_matrix_rotate_by_vector(self->modelMat, self->modelMat, self->rotation);
        gfc_matrix_translate(self->modelMat, self->position);
    }
    
    if (self->scripts) {
        for (Uint32 i = 0; i < gfc_list_get_count(self->scripts); i++)
        {
            Script* script = (Script*)gfc_list_get_nth(self->scripts, i);
            if (!script) continue;
            if (script->Update)
                script->Update(self, script);
        }
    }
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_update(&entity_manager.entity_list[i]);
    }
}

Vector3D vector3d_get_from_angles(Vector3D angles)
{
    Vector3D forward;
    vector3d_set(forward, 0, 1, 0);
    vector3d_rotate_about_x(&forward, angles.x);
    vector3d_rotate_about_z(&forward, angles.z);
    return forward;
}
Script* entity_get_script(Entity* self, TextLine name) {
    if (!self || !self->scripts) {
        return NULL;
    }
    for (int i = 0; i < gfc_list_get_count(self->scripts); i++) {
        if (strcmp(((Script*)gfc_list_get_nth(self->scripts, i))->name, name) == 0)  {
            return gfc_list_get_nth(self->scripts, i);
        }
    }
    return NULL;
}

void entity_remove_script(Entity* self, TextLine name) {
    if (!self || !self->scripts) {
        return;
    }
    for (int i = 0; i < gfc_list_get_count(self->scripts); i++) {
        if (strcmp(((Script*)gfc_list_get_nth(self->scripts, i))->name, name) == 0) {
            gfc_list_delete_nth(self->scripts, i);
            return;
        }
    }
    return;
}


Entity* entity_load_from_prefab(const char* filename) {
    SJson* json;
    json = sj_load(filename);
    return entity_load_from_sjson(json, filename);
}

Entity* entity_load_from_sjson(SJson* json, const char* filename) {

    //  Memory allocation error
    if (!json) {
        slog("failed to load json file (%s) for the entity data", filename);
        return NULL;
    }

    const char* model = NULL;
    Vector3D scale;
    Vector3D position;
    Vector3D rotation;

    SJson* scriptArray;
    Entity* entity = entity_new();

    List* scripts = NULL;

    if (!entity) {
        slog("Couldn't allocate memory for entity %s", filename);
        return NULL;
    }

    //  It is ok if not every object has a script.
    if (!sj_object_get_value(json, "model")) {
        slog("No model defined for %s", filename);
    }
    else if (!(model = sj_get_string_value(sj_object_get_value(json, "model"))))
    {
        slog("Failed to load model for %s", filename);
        goto fail;
    }
    if (!sj_value_as_vector3d((sj_object_get_value(json, "scale")), &scale))
    {
        slog("Failed to load scale for %s", filename);
        goto fail;
    }
    if (!sj_value_as_vector3d((sj_object_get_value(json, "position")), &position))
    {
        slog("Failed to load position for %s", filename);
        goto fail;
    }
    if (!sj_value_as_vector3d((sj_object_get_value(json, "rotation")), &rotation))
    {
        slog("Failed to load rotation for %s", filename);
        goto fail;
    }
    //  It is ok if not every object has a script.
    if (!(scriptArray = sj_object_get_value(json, "scripts")))
    {
        slog("Failed to load scripts for %s", filename);
    }
    else if (!sj_is_array(scriptArray)) {
        slog("Scripts are improperly formatted for %s", filename);
        goto fail;
    }
    else {
        scripts = gfc_list_new();
        if (!scripts) {
            slog("Couldn't allocate memory for scripts for %s", filename);
            goto fail;
        }
        for (int j = 0; j < sj_array_get_count(scriptArray); j++) {
            if (sj_is_object(sj_array_get_nth(scriptArray, j))) {
                SJson* scriptObject = sj_array_get_nth(scriptArray, j);
                const char* scriptName = sj_object_get_value_as_string(scriptObject, "name");
                if (!scriptName) {
                    slog("%dth script in %s prefab is not formatted correctly", j, filename);
                    continue;
                }
                Script* script = script_loader_getScript(scriptName);
                if (!script) {
                    slog("%dth script %s in %s prefab could not be found", j, scriptName, filename);
                    continue;
                }
                if (!(sj_object_get_value(scriptObject, "args")))
                {
                    slog("%dth script %s in %s prefab has no arguments; consider restructuring",
                        j, scriptName, filename);
                }
                else {
                    SJson* arguments = sj_object_get_value(scriptObject, "args");
                    char** argvalues = calloc(sizeof(char), sj_array_get_count(arguments));
                    for (int k = 0; k < sj_array_get_count(arguments); k++) {
                        if (!sj_is_string(sj_array_get_nth(arguments, k))) {
                            slog("%dth script %s %dth argument in %s prefab is not a string",
                                j, scriptName, k, filename);
                            continue;
                        }
                        argvalues[k] = malloc(sizeof(char) *
                            (1 + strlen(sj_get_string_value(sj_array_get_nth(arguments, k)))));
                        strcpy(argvalues[k], sj_get_string_value(sj_array_get_nth(arguments, k)));
                    }
                    script->Arguments(entity, script, argvalues, sj_array_get_count(arguments));
                    for (int k = 0; k < sj_array_get_count(arguments); k++) {
                        free(argvalues[k]);
                    }
                    free(argvalues);
                }

                gfc_list_append(scripts, script);

            }
            else if (sj_is_string(sj_array_get_nth(scriptArray, j))) {
                const char* scriptName = sj_get_string_value(sj_array_get_nth(scriptArray, j));
                if (!scriptName) {
                    slog("%dth script in %s prefab is not formatted correctly", j, filename);
                    continue;
                }
                Script* script = script_loader_getScript(scriptName);
                if (!script) {
                    slog("%dth script %s in %s scene cannot be found", j, scriptName, filename);
                    continue;
                }
                gfc_list_append(scripts, script);
            }
            else {
                slog("%dth script in %s prefab incorrectly formatted", j, filename);
            }
        }
    }
    entity_setup(entity, gf3d_model_load(model), scale, position, rotation, scripts);
    return entity;
fail:
    entity_free(entity);
    return NULL;
}
/*eol@eof*/
