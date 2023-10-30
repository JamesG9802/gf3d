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
        slog("Loading %dth entity", i);
        entity_load_from_sjson(sj_array_get_nth(scene, i), filename);
    }

    fail:
    sj_free(json);
}
/*eol@eof*/
