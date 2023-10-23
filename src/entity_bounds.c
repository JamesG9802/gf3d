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
/*eol@eof*/
