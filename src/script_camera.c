#include <stdio.h>

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
static void Start(Entity* self) {
    gf3d_camera_set_scale(vector3d(1, 1, 1));
}
/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self) {}
/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self) {
    gf3d_camera_update_view();
    gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());
}
/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self) {
}

Script* script_new_camera() {
    return script_new(&Start, NULL, &Update, &Destroy);
}


/*eol@eof*/
