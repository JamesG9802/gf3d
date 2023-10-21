#include <SDL.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_matrix.h"
#include "gfc_input.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "entity.h"

#include "engine_utility.h"
#include "entity_bounds.h"

static EntityManager entity_manager;

//	I stole this from 
//	http://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-Sphere-Intersection.html

Vector3D IsMouseOverEntity(Entity* entity) {
	//	https://antongerdelan.net/opengl/raycasting.html

	//	Step 0
	int mousex, mousey;
	int width, height;
	SDL_GetMouseState(&mousex, &mousey);
	gf3d_vgraphics_get_window_size(&width, &height);

	Vector3D in = vector3d(mousex, mousey, 0);
	Matrix4 view, projection;
	Vector2D viewport = vector2d(width, height);
	gf3d_camera_get_view_mat4(view);
	gf3d_vgraphics_get_projection_matrix(projection);
	gf3d_vgraphics_get_window_size(&width, &height);
	Vector3D output = gfc_unproject(in, view, projection, viewport);

	slog("%f %f %f", output.x, output.y, output.z);
	return output;
}