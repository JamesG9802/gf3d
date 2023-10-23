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
	int mouse_x, mouse_y;
	int width, height;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	gf3d_vgraphics_get_window_size(&width, &height);

	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = (2.0f * mouse_y) / height - 1.0f;
	float z = -1;

	Matrix4 projection, view;
	gf3d_camera_get_view_mat4(view);
	gf3d_vgraphics_get_projection_matrix(projection);

	Vector4D ray_clip = vector4d(x, y, z, 1);
	Matrix4 projection_inverse;
	gfc_matrix4_invert(projection_inverse, projection);

	Vector4D ray_eye;
	gfc_matrix_M_multiply_v(&ray_eye, projection_inverse, ray_clip);
	ray_eye = vector4d(ray_eye.x, ray_eye.y, -1, 0);

	Vector4D ray_wor_4D;
	Matrix4 view_inverse;
	gfc_matrix4_invert(view_inverse, view);
	gfc_matrix_M_multiply_v(&ray_wor_4D, view_inverse, ray_eye);

	Vector3D ray_wor = vector3d(ray_wor_4D.x, ray_wor_4D.y, ray_wor_4D.z);
	vector3d_normalize(&ray_wor);
	vector3d_scale(ray_wor, ray_wor, 70);

	Vector3D rotation;
	Vector3D position = {0};
	Vector3D cameraPos = {0};
	gf3d_camera_get_rotation(&rotation);
	gf3d_camera_get_position(&cameraPos);
	vector3d_add(position, position, ray_wor);
//	vector3d_rotate_about_z(&position,-rotation.z);
	vector3d_add(position, position, cameraPos);
	slog("%f %f %f", position.x, position.y, position.z);
	return position;
}