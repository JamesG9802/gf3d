#include <stdio.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_list.h"
#include "gfc_types.h"
#include "gfc_config.h"

#include "gf3d_model.h"

#include "entity.h"

#include "script.h"
#include "script_loader.h"

#include "scene.h"

/*
typedef struct
{

    Model *worldModel;
    List *spawnList;        //entities to spawn
    List *entityList;       //entities that exist in the world
}World;
*/

void scene_load(char *filename)
{
    SJson *json,* scene;
    
    json = sj_load(filename);

    //  Memory allocation error
    if (!json) {
        slog("failed to load json file (%s) for the scene data",filename);
        return;
    }
    scene = sj_object_get_value(json,"scene");

    //  Missing object error
    if (!scene) {
        slog("failed to find scene object in %s scene config",filename);
        goto fail;
    }
    //  Improper formatting error
    if (!sj_is_array(scene)) {
        slog("scene is not formatted as an array in %s scene config", filename);
        goto fail;
    }
    for (int i = 0; i < sj_array_get_count(scene); i++) {
        SJson* object = sj_array_get_nth(scene, i);
        const char* model;
        Vector3D scale;
        Vector3D position;
        Vector3D rotation;

        SJson* scriptArray;
        List* scripts = NULL;
        char arr[256];
        if(!(model = sj_get_string_value(sj_object_get_value(object, "model"))))
        { 
            slog("%dth object in %s scene failed to load model", i, filename);
            goto objectfail;
        }
        if (!sj_value_as_vector3d((sj_object_get_value(object, "scale")), &scale))
        {
            slog("%dth object in %s scene failed to load scale", i, filename);
            goto objectfail;
        }
        if (!sj_value_as_vector3d((sj_object_get_value(object, "position")), &position))
        {
            slog("%dth object in %s scene failed to load position", i, filename);
            goto objectfail;
        }
        if (!sj_value_as_vector3d((sj_object_get_value(object, "rotation")), &rotation))
        {
            slog("%dth object in %s scene failed to load rotation", i, filename);
            goto objectfail;
        }
        //  It is ok if not every object has a script.
        if (!(scriptArray = sj_object_get_value(object, "scripts")))
        {
            slog("%dth object in %s scene has no scripts defined", i, filename);
        }
        else if (!sj_is_array(scriptArray)) {
            slog("%dth object in %s scene has scripts improperly defined", i, filename);
            goto objectfail;
        }
        else {
            scripts = gfc_list_new();
            if (!scripts) {
                slog("%dth object in %s scene couldn't allocate memory for scripts", i, filename);
                goto fail;
            }
            for (int j = 0; j < sj_array_get_count(scriptArray); j++) {
                const char* scriptName = sj_get_string_value(sj_array_get_nth(scriptArray, j));
                if (!scriptName) {
                    slog("%dth object's %dth script '$s' in %s scene is not formatted correctly", i, j, scriptName, filename);
                    continue;
                }
                Script* script = script_loader_getScript(scriptName);
                if (!script) {
                    slog("%dth object's %dth script '%s' in %s scene cannot be found", i, j, scriptName, filename);
                    continue;
                }
                //script->Start();
                gfc_list_append(scripts, script);
            }
        }
        Entity* entity = entity_new();
        if (!entity) {
            slog("%dth object in %s scene couldn't allocate memory for entity", i, filename);
        }
        entity->model = gf3d_model_load(model);
        entity->scale = scale;
        entity->position = position;
        entity->rotation = rotation;
        if (scripts)
            entity->scripts = scripts;
        objectfail:
            continue;
    }

    fail:
    sj_free(json);
}
/*eol@eof*/
