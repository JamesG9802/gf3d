#include <SDL.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_matrix.h"
#include "gfc_primitives.h"
#include "gfc_input.h"

#include "gf2d_font.h"

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "entity.h"

#include "engine_utility.h"
#include "engine_time.h"

#include "entity_bounds.h"
#include "script_manager.h"

#define UNPRESSED 0
#define DOWN 1
#define RELEASED 2
typedef struct {
	Uint8 leftMouse;
	Uint8 rightMouse;

} MouseState;

static EntityManager entity_manager;
static MouseState mouse_state;

void engine_utility_update() {
	int state = SDL_GetMouseState(NULL, NULL);
	if ((mouse_state.leftMouse & DOWN) && !(state & SDL_BUTTON_LMASK))
		mouse_state.leftMouse = RELEASED;
	else if (state & SDL_BUTTON_LMASK)
		mouse_state.leftMouse = DOWN;
	else
		mouse_state.leftMouse = UNPRESSED;
	if ((mouse_state.rightMouse & DOWN) && !(state & SDL_BUTTON_RMASK))
		mouse_state.rightMouse = RELEASED;
	else if (state & SDL_BUTTON_RMASK)
		mouse_state.rightMouse = DOWN;
	else
		mouse_state.rightMouse = UNPRESSED;
}

Vector2D engine_utility_screentondc(Vector2D screen) {
	int width, height;
	gf3d_vgraphics_get_window_size(&width, &height);
	return vector2d((2.0f * screen.x) / width - 1.0f, 1.0f - (2.0f * screen.y) / height);
}

Vector2D engine_utility_ndctoscreen(Vector2D ndc) {
	int width, height;
	gf3d_vgraphics_get_window_size(&width, &height);
	return vector2d((ndc.x + 1) * width / 2.0, (ndc.y - 1) * height / -2.0);
}

Bool engine_utility_ismouseover(Entity* entity, Edge3D* ray) {
	if (script_manager_getmetastate() == INMENU)	return false;
	int mouse_x, mouse_y;
	int width, height;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	gf3d_vgraphics_get_window_size(&width, &height);

	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = (2.0f * mouse_y) / height - 1.0f;
	float z = -1;

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
	vector3d_scale(ray_wor, ray_wor, 1000);
	Vector3D position = {0};
	Vector3D cameraPos = {0};
	gf3d_camera_get_position(&cameraPos);
	vector3d_add(position, position, ray_wor);
	vector3d_add(position, position, cameraPos);
	Edge3D line;
	memcpy(&line.a, &cameraPos, sizeof(Vector3D));
	memcpy(&line.b, &position, sizeof(Vector3D));
	if (ray)
		memcpy(ray, &line, sizeof(Edge3D));
	return entity_bounds_islineintersecting(entity->bounds, line);
}

Vector3D engine_utility_mouseprojectray() {
	int mouse_x, mouse_y;
	int width, height;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	gf3d_vgraphics_get_window_size(&width, &height);

	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = (2.0f * mouse_y) / height - 1.0f;
	float z = -1;

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
	return ray_wor;
}

Uint32 engine_utility_ismousedown() {
	return mouse_state.leftMouse & DOWN || mouse_state.rightMouse & DOWN;
}

Bool engine_utility_isleftmousedown() {
	return mouse_state.leftMouse & DOWN;
}

Bool engine_utility_isrightmousedown() {
	return mouse_state.rightMouse & DOWN;
}

Bool engine_utility_isleftmousereleased() {
	return mouse_state.leftMouse & RELEASED;
}

Bool engine_utility_isrightmousereleased() {
	return mouse_state.rightMouse & RELEASED;
}

void engine_utility_leftmouse_consume() {
	mouse_state.leftMouse = UNPRESSED;
}

void engine_utility_rightmouse_consume() {
	mouse_state.rightMouse = UNPRESSED;
}

Model* engine_utility_createquadmodel(char* texture_filepath) {
	Model* model = gf3d_model_new();
	if (!model)return NULL;
	gfc_line_cpy(model->filename, "Quad texture");
	model->mesh = gf3d_mesh_load("meshes/quad.obj");
	model->texture = gf3d_texture_load(texture_filepath);
	if (!model->texture)
		model->texture = gf3d_texture_load("images/default.png");
	return model;
}

Entity* engine_utility_createquad(char* texture_filepath) {
	Entity* entity = entity_new();
	entity_setup(entity, 
		engine_utility_createquadmodel(texture_filepath), 
		vector3d(1, 1, 1), 
		vector3d(0, 0, 0), 
		vector3d(0, 0, 0), 
		NULL,
		NULL);
	return entity;
}

Texture* engine_utility_createtexturefromtext(Font* font, const char* text, Color color, 
	int* width, int* height) {
	TTF_SizeText(font->font, text, width, height);
	SDL_Surface* surface =
		TTF_RenderUTF8_Blended_Wrapped(font->font, text, gfc_color_to_sdl(color), 800);
	TextLine name;
	strcpy_s(name, strlen(text) + 1, text);
	name[sizeof(TextLine) - 1] = '\0';
	return gf3d_texture_from_surface(name, surface);
}
void engine_utility_settexture(Entity* entity, Texture* texture, int width, int height, int scaleFactor) {
	if (!texture)	return;
	gf3d_texture_free(entity->model->texture);
	entity->model->texture = texture;
	Vector2D scale = vector2d(width, height);
	vector2d_normalize(&scale);
	vector2d_scale(scale, scale, scaleFactor);

	entity->scale.x = scale.x;
	entity->scale.y = 1;
	entity->scale.z = scale.y;
}
void engine_utility_settexturebysurface(Entity* entity, SDL_Surface* src) {
	if (!src) return;
	SDL_Surface* surface = SDL_DuplicateSurface(src);
	gf3d_texture_free(entity->model->texture);
	entity->model->texture = gf3d_texture_convert_surface(surface);
	entity->model->texture->surface = surface;
}