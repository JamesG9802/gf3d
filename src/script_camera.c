#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "entity.h"

#include "script.h"
#include "script_defs.h"

/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    gf3d_camera_set_scale(vector3d(1, 1, 1));
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {
    gf3d_camera_update_view();
    gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_camera() {
    return script_new("camera", &Start, NULL, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
