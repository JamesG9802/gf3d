#include <SDL.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_matrix.h"
#include "gfc_input.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "entity.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "entity_bounds.h"

static EntityManager entity_manager;
static float timeDelta = 0;

Vector3D IsMouseOverEntity(Entity* entity) {
	int mouse_x, mouse_y;
	int width, height;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	gf3d_vgraphics_get_window_size(&width, &height);

	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = (2.0f * mouse_y) / height - 1.0f;
	float z = -1;
	timeDelta += engine_time_delta();
	Matrix4 projection, view;
	Matrix4 projection_inverse, view_inverse;
	gf3d_camera_get_view_mat4(view);
	gf3d_vgraphics_get_projection_matrix(projection);

	gfc_matrix4_invert(projection_inverse, projection);
	gfc_matrix4_invert(view_inverse, view);

	//	something about column major order vs row major order
	gfc_matrix_transpose(projection_inverse, projection_inverse);
	gfc_matrix_transpose(view_inverse, view_inverse);

	Vector4D ray_clip = vector4d(x, y, z, 1);
	Vector4D ray_eye;

	gfc_matrix_M_multiply_v(&ray_eye, projection_inverse, ray_clip);
	ray_eye = vector4d(ray_eye.x, ray_eye.y, -1, 0);

	Vector4D ray_wor_4D;
	gfc_matrix_M_multiply_v(&ray_wor_4D, view_inverse, ray_eye);

	Vector3D ray_wor = vector3d(ray_wor_4D.x, ray_wor_4D.y, ray_wor_4D.z);
	vector3d_normalize(&ray_wor);
	vector3d_scale(ray_wor, ray_wor, 60);
		
	Vector3D position = {0};
	Vector3D cameraPos = {0};
	gf3d_camera_get_position(&cameraPos);
	vector3d_add(position, position, ray_wor);
	vector3d_add(position, position, cameraPos);
	slog("%f %f %f", position.x, position.y, position.z);
	return position;
}