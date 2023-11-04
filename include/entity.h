#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_list.h"
#include "gfc_types.h"
#include "gfc_color.h"
#include "gfc_primitives.h"

#include "gf3d_model.h"

typedef struct Script_s Script;
typedef struct Entity_Bounds_s Entity_Bounds;

typedef enum Layer {
    NORMAL = 1,
    UI = 2
} Layer;

typedef enum
{
    ES_idle = 0,
    ES_hunt,
    ES_dead,
    ES_attack
}EntityState;

typedef struct Entity_S
{
    Uint8       _inuse;     /**<keeps track of memory usage*/
    Matrix4     modelMat;   /**<orientation matrix for the model*/
    Color       color;      /**<default color for the model*/
    Model      *model;      /**<pointer to the entity model to draw  (optional)*/
    Uint8       hidden;     /**<if true, not drawn*/
    Uint8       selected;
    Color       selectedColor;      /**<Color for highlighting*/
    
//    Box         bounds; // for collisions
    Entity_Bounds* bounds; // for collisions
    Layer       layer;  // the layer that the entity is on
    int         clips;  // if false, skip collisions

//    void       (*think)(struct Entity_S *self); /**<pointer to the think function*/
    Uint8       skipCommonUpdate;  //  true if entity wants to skip their common update function
    Uint8       skipCommonDraw; //  true if entity wants to skip their common draw function
//    void       (*update)(struct Entity_S *self); /**<pointer to the update function*/
    void        (*free)(struct Entity_S* self); /**pointer to the free function*/
    void        (*draw)(struct Entity_S *self); /**<pointer to an optional extra draw funciton*/
    void        (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /**<pointer to the think function*/
    void        (*onDeath)(struct Entity_S *self); /**<pointer to an funciton to call when the entity dies*/
    
    EntityState state;
    
    Vector3D    position;  
    Vector3D    velocity;
    Vector3D    acceleration;
        
    Vector3D    scale;
    Vector3D    rotation;
    
    List*       scripts;    //  List of scripts, if any
    List*       children;    //  List of children, if any

    struct Entity_S*     parent; // parent, if any
    void *customData;   /**<IF an entity needs to keep track of extra data, we can do it here*/
}Entity;

typedef struct
{
    Entity* entity_list;
    Uint32  entity_count;

}EntityManager;

/**
 * @brief initializes the entity subsystem
 * @param maxEntities the limit on number of entities that can exist at the same time
 */
void entity_system_init(Uint32 maxEntities);

/**
 * @brief provide a pointer to a new empty entity
 * @return NULL on error or a valid entity pointer otherwise
 */
Entity *entity_new();

/**
 * @brief Takes a new entity and sets it up based on parameters
 * @param self the entity in question
 * @param model model of the entity
 * @param scale scale of the entity
 * @param position position of the entity
 * @param rotation rotation of the entity
 * @param scripts a list of scripts that the entity has. Can be NULL
 */
void entity_setup(Entity* self, Model* model, Vector3D scale, Vector3D position, Vector3D rotation, List* scripts);


/**
 * @brief free a previously created entity from memory
 * @param self the entity in question
 */
void entity_free(Entity *self);


/**
 * @brief Draw an entity in the current frame
 * @param self the entity in question
 */
void entity_draw(Entity *self);

/**
 * @brief draw ALL active entities
 */
void entity_draw_all();

/**
 * @brief Call an entity's think function if it exists
 * @param self the entity in question
 */
void entity_think(Entity *self);

/**
 * @brief run the think functions for ALL active entities
 */
void entity_think_all();

/**
 * @brief run the update functions for ALL active entities
 */
void entity_update_all();

Vector3D vector3d_get_from_angles(Vector3D angles);

/// <summary>
/// Gets a script by the name, if it exists.
/// </summary>
/// <param name="self"></param>
/// <param name="name"></param>
/// <returns></returns>
Script* entity_get_script(Entity* self, TextLine name);

/// <summary>
/// Removes a script by the name, if it exists.
/// </summary>
/// <param name="self"></param>
/// <param name="name"></param>
void entity_remove_script(Entity* self, TextLine name);

/// <summary>
/// Loads an entity from a prefab file.
/// </summary>
/// <param name="file"></param>
/// <param name="parent>Optional parent</param>
/// <returns></returns>
Entity* entity_load_from_prefab(const char* filename);
    
/// <summary>
/// Loads an entity from a json.
/// </summary>
/// <param name="json"></param>
/// <returns></returns>
Entity* entity_load_from_sjson(SJson* json, const char* filename);

void entity_add_child(Entity* parent, Entity* child);
#endif
