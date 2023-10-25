#include "simple_logger.h"

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


Bool entity_bounds_islineintersecting(Entity_Bounds* bounds, Edge3D line) {
	if (!bounds)
		return false;
	if (bounds->type == BOX)
	{
		return gfc_edge_box_test(line, bounds->bounds.box, NULL, NULL);
	}
	if (bounds->type == SPHERE)
	{
		return gfc_edge3d_to_sphere_intersection(line, bounds->bounds.sphere, NULL, NULL);
	}
	return false;
}
/*eol@eof*/
