#pragma once

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_color.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_model.h"

#include "entity.h"

/**
 * @brief loads in a scene
 * @param filename - the path to the scene 
 */
void scene_load(char* filename);

/*
void world_draw(World* world);

void world_delete(World* world);

void world_run_updates(World* world);

void world_add_entity(World* world, Entity* entity);
*/