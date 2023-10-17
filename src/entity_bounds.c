#include "entity.h"
#include "entity_bounds.h"

Entity_Bounds entity_bounds_new_box(float x, float y, float z, float w, float h, float d) {
	Entity_Bounds bounds = {0};
	bounds.type = BOX;
	bounds.bounds.box = gfc_box(x, y, z, w, h, d);
	return bounds;
}
Entity_Bounds entity_bounds_new_sphere(float x, float y, float z, float r) {
	Entity_Bounds bounds = {0};
	bounds.bounds.sphere = gfc_sphere(x, y, z, r);
	bounds.type = SPHERE;
	return bounds;
}

void entity_bounds_visualize(Entity* entity, Model** model, Matrix4* modelMat) {
	Entity_Bounds* enbounds = entity->bounds;
	if (!enbounds || enbounds->type == NONE)	return;
	else if (enbounds->type == BOX) {
		Matrix4 modelMatrix = {0};
		*model = gf3d_model_load("models/cube.model");
		Vector3D scale = vector3d(enbounds->bounds.box.w, enbounds->bounds.box.h, enbounds->bounds.box.d);
		gfc_matrix_identity(modelMatrix);

		gfc_matrix_scale(modelMatrix, scale);
		gfc_matrix_rotate_by_vector(modelMatrix, modelMatrix, entity->rotation);
		gfc_matrix_translate(modelMatrix, entity->position);
		memmove(modelMat, modelMatrix, sizeof(Matrix4));
	}
	else if (enbounds->type == SPHERE) {
		Matrix4 modelMatrix = { 0 };
		*model = gf3d_model_load("models/sphere.model");
		Vector3D scale = vector3d(enbounds->bounds.sphere.r, enbounds->bounds.sphere.r, enbounds->bounds.sphere.r);
		gfc_matrix_identity(modelMatrix);

		gfc_matrix_scale(modelMatrix, scale);
		gfc_matrix_rotate_by_vector(modelMatrix, modelMatrix, entity->rotation);
		gfc_matrix_translate(modelMatrix, entity->position);
		memmove(modelMat, modelMatrix, sizeof(Matrix4));
	}
}
/*eol@eof*/
