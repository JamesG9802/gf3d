#pragma once
#include "gfc_primitives.h"
#include "gf3d_model.h"

/**
 * @brief the type of the bounds.
 */
typedef enum {
	NONE,	//	?
	BOX,
	SPHERE,
	POLYHEDRA
} Entity_Bounds_Type;

/**
 * @brief can be any bounds.
 */
static union Entity_Bound_Container{
	Box box;
	Sphere sphere;
	//Polyhedra polyhedra;
} Entity_Bounds_Container;


/**
 * @brief represents the bounding object of an entity.
 */
typedef struct Entity_Bounds_s {
	union Entity_Bound_Container bounds;
	Entity_Bounds_Type type;
} Entity_Bounds;

/**
 * @brief creates a new entity bounds as a box.
 */
Entity_Bounds entity_bounds_new_box(float x, float y, float z, float w, float h, float d);

/**
 * @brief creates a new entity bounds as a sphere.
 */
Entity_Bounds entity_bounds_new_sphere(float x, float y, float z, float r);

/**
 * @brief creates a new entity bounds as a polygon.
 */
Entity_Bounds entity_bounds_new_polygon();

/**
 * @brief Sets the model representing the current entity_bounds and the model matrix.
 */
void entity_bounds_visualize(Entity* entity, Model** model, Matrix4* modelMat);