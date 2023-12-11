#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_input.h"

#include "gf3d_camera.h"
#include "gf3d_vgraphics.h"
#include "gf3d_buffers.h"

#include "engine_time.h"
#include "engine_utility.h"
#include "event_manager.h"

#include "entity.h"
#include "script.h"
#include "script_defs.h"

#include "dice.h"
#include "dicevalue.h"

#include "inventory.h"

#include "script_player.h"
#include "script_manager.h"

Entity* player = NULL;

Vector3D desiredRotation(int sideCount, int desiredSide) {
    switch (sideCount) {
    default:
    case 4:
        switch (desiredSide) {
        default:
        case 0:
            return vector3d(0, 0, 0);
        case 1:
            return vector3d(0, 110 * GFC_DEGTORAD, 0);
        case 2:
            return vector3d(-52.3715 * GFC_DEGTORAD,
                -131.5 * GFC_DEGTORAD,
                215 * GFC_DEGTORAD);
        case 3:
            return vector3d(52.3715 * GFC_DEGTORAD,
                -131.5 * GFC_DEGTORAD,
                -215 * GFC_DEGTORAD);
        }
    case 6:
        switch (desiredSide) {
        default:
        case 0:
            return vector3d(0, 0, 0);
        case 1:
            return vector3d(0, GFC_HALF_PI, 0);
        case 2:
            return vector3d(0, -GFC_HALF_PI, 0);
        case 3:
            return vector3d(0, GFC_PI, 0);
        case 4:
            return vector3d(GFC_HALF_PI, 0, 0);
        case 5:
            return vector3d(-GFC_HALF_PI, 0, 0);
        }
    }
}


Bool diceCollisionCheck(PlayerData* data, Vector3D* relativePosition, Vector3D* normal, Vector3D* faceNormal, float* penetration) {
    Mesh* mesh = data->diceEntity->model->mesh;
    Face* faces;
    Vertex* vertices;

    Bool collided = false;
    {
        VkDevice device = gf3d_vgraphics_get_default_logical_device();
        VkDeviceSize bufferSize = sizeof(Face) * mesh->faceCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &stagingBuffer, &stagingBufferMemory);
        gf3d_buffer_copy(mesh->faceBuffer, stagingBuffer, bufferSize);

        void* _data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &_data);

        // Allocate memory for faces and copy data from staging buffer
        faces = malloc(bufferSize);
        memcpy(faces, _data, (size_t)bufferSize);

        vkUnmapMemory(device, stagingBufferMemory);

        vkDestroyBuffer(device, stagingBuffer, NULL);
        vkFreeMemory(device, stagingBufferMemory, NULL);
    }
    {
        VkDevice device = gf3d_vgraphics_get_default_logical_device();
        VkDeviceSize bufferSize = sizeof(Vertex) * mesh->vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &stagingBuffer, &stagingBufferMemory);
        gf3d_buffer_copy(mesh->buffer, stagingBuffer, bufferSize);

        void* _data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &_data);

        // Allocate memory for faces and copy data from staging buffer
        vertices = malloc(bufferSize);
        memcpy(vertices, _data, (size_t)bufferSize);

        vkUnmapMemory(device, stagingBufferMemory);

        vkDestroyBuffer(device, stagingBuffer, NULL);
        vkFreeMemory(device, stagingBufferMemory, NULL);
    }

    //  Collision check - plane is at 0 so just check if any vertex z is below 0

    Vector3D deepestrelativePosition;
    Vector3D deepestNormal;
    Vector3D deepestFaceNormal;
    float deepestPenetration = 0;

    //  Find deepest face;
    for (int i = 0; i < mesh->faceCount; i++) {
        Vector3D face1, face2, face3;
        memcpy(&face1, &vertices[faces[i].verts[0]].vertex, sizeof(Vector3D));
        memcpy(&face2, &vertices[faces[i].verts[1]].vertex, sizeof(Vector3D));
        memcpy(&face3, &vertices[faces[i].verts[2]].vertex, sizeof(Vector3D));

        vector3d_rotate_about_x(&face1, data->diceEntity->rotation.x);
        vector3d_rotate_about_y(&face1, data->diceEntity->rotation.y);
        vector3d_rotate_about_z(&face1, data->diceEntity->rotation.z);

        vector3d_rotate_about_x(&face2, data->diceEntity->rotation.x);
        vector3d_rotate_about_y(&face2, data->diceEntity->rotation.y);
        vector3d_rotate_about_z(&face2, data->diceEntity->rotation.z);

        vector3d_rotate_about_x(&face3, data->diceEntity->rotation.x);
        vector3d_rotate_about_y(&face3, data->diceEntity->rotation.y);
        vector3d_rotate_about_z(&face3, data->diceEntity->rotation.z);

        //    slog("%f %f %f", face1.x, face1.y, face1.z);
        //    slog("%f %f %f", face2.x, face2.y, face2.z);
        //    slog("%f %f %f", face3.x, face3.y, face3.z);


        if (face1.z + data->diceEntity->position.z < 0 || face2.z + data->diceEntity->position.z < 0 || face3.z + data->diceEntity->position.z < 0)
        {
            if (collided) {
                Vector3D a, b, c, d, e;
                vector3d_sub(a, face2, face1);
                vector3d_sub(b, face3, face1);

                vector3d_cross_product(&c, a, b);
                vector3d_normalize(&c);
                
                vector3d_angles(c, &d);
                vector3d_angles(deepestFaceNormal, &e);

                if(d.x > e.x)
                    continue;
            }
            collided = true;

            //  Who knows how the 'relative position is calculated'
            int f1_w = face1.z < 0, f2_w = face2.z < 0, f3_w = face3.z < 0;

            deepestrelativePosition.x = (face1.x * f1_w + face2.x * f2_w + face3.x * f3_w) / (f1_w + f2_w + f3_w);
            deepestrelativePosition.y = (face1.y * f1_w + face2.y * f2_w + face3.y * f3_w) / (f1_w + f2_w + f3_w);
            deepestrelativePosition.z = (face1.z * f1_w + face2.z * f2_w + face3.z * f3_w) / (f1_w + f2_w + f3_w);

            deepestNormal.x = 0;
            deepestNormal.y = 0;
            deepestNormal.z = 1;

            Vector3D a, b;
            vector3d_sub(a, face2, face1);
            vector3d_sub(b, face3, face1);

            vector3d_cross_product(&deepestFaceNormal, a, b);
            vector3d_normalize(&deepestFaceNormal);
            deepestPenetration = min(face1.z, min(face2.z, face3.z)) + data->diceEntity->position.z;
        }
    }
    if (collided) {
        memcpy(relativePosition, &deepestrelativePosition, sizeof(Vector3D));
        memcpy(normal, &deepestNormal, sizeof(Vector3D));
        memcpy(faceNormal, &deepestFaceNormal, sizeof(Vector3D));
        *penetration = deepestPenetration;
    }

    free(faces);
    free(vertices);
    return collided;
}

//  Page 8 (PhysicsSystem::ImpuleResolveCollision
void ImpulseResolveCollision(PlayerData* data) {
    float totalMass = 1;    //  constant

    Vector3D relative, normal, faceNormal;
    float penetration;

    if (diceCollisionCheck(data, &relative, &normal, &faceNormal, &penetration))
    {
        //  projection to fix position
        data->diceEntity->position.z -= 2 * penetration;
        Vector3D relativeVelocity = vector3d(-data->diceVelocity.x, -data->diceVelocity.y, -data->diceVelocity.z);

        float velAlongNormal = relativeVelocity.x * normal.x +
            relativeVelocity.y * normal.y +
            relativeVelocity.z * normal.z;

        float j = -(1 + .8f) * velAlongNormal;  //  assume mass = 1 for simplicity

        data->diceVelocity.x -= j * normal.x;
        data->diceVelocity.y -= j * normal.y;
        data->diceVelocity.z -= j * normal.z;

        //  Friction
        data->diceVelocity.x *= .8;
        data->diceVelocity.y *= .8;

        Vector3D torque;
        torque.x = relative.y * normal.z - relative.z * normal.y;
        torque.y = relative.z * normal.x - relative.x * normal.z;
        torque.z = relative.x * normal.y - relative.y * normal.x;

        float angularImpulse = torque.x * normal.x + torque.y * normal.y + torque.z * normal.z;

        data->angularVelocity.x -= angularImpulse;
        data->angularVelocity.y -= angularImpulse;
        data->angularVelocity.z -= angularImpulse;

        //  Cheating (?) by calculating the normal of the colliding face and determining the angular velocity needed
        //  to make it face the ground (0, 0, -1)

        // Calculate the angles needed to rotate faceNormal to point towards (0, 0, -1
        Vector3D targetAngles = desiredRotation(((Dice*)gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex))->sideCount, data->chosenSide);
        
        //  show upside down so the player can actually see the value
    //    targetAngles.y += GFC_PI;

//        Vector3D startAngles;
//        vector3d_angles(faceNormal, &startAngles);
        slog("%d", data->chosenSide);
        //slog("%f %f", data->diceEntity->rotation.x, data->diceEntity->rotation.z);
        // Rotate faceNormal using the calculated angles
        float angularWeight = .75;
        float existingWeight = 1 - angularWeight;

        data->angularVelocity.x = angularWeight * (-data->diceEntity->rotation.x + targetAngles.x) + existingWeight * (data->angularVelocity.x);
        data->angularVelocity.y = angularWeight * (-data->diceEntity->rotation.y + targetAngles.y) + existingWeight * (data->angularVelocity.y);
        data->angularVelocity.z = angularWeight * (-data->diceEntity->rotation.z + targetAngles.z) + existingWeight * (data->angularVelocity.z);
  
        

        data->angularVelocity.x *= .8;
        data->angularVelocity.y *= .8;
        data->angularVelocity.z *= .8;
    }
}

void createDiceEntity() {
    PlayerData* data = script_player_getplayerdata();
    if (gfc_list_get_count(data->inventory->diceLoadout) == 0) {
        slog("You don't have dice in your loadout");
        return;
    }
    Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
    if (!dice)
    {
        slog("Couldn't find dice");
        return;
    }
    switch (dice->sideCount) {
    case 4:
        data->diceEntity = entity_load_from_prefab("prefabs/dice4.prefab", player);
        break;
    default:
    case 6:
        data->diceEntity = entity_load_from_prefab("prefabs/dice6.prefab", player);
        break;
    }
    Entity* diceInfoWindow = script_manager_getentity("ui_combatdiceinformation");
    if (!diceInfoWindow) return;
    dice_to_ui(dice, diceInfoWindow);
    dice_to_texture(dice, data->diceEntity);
}

PlayerData script_player_newplayerdata() {
    PlayerData playerData = { 0 };
    playerData.diceEntity = NULL;
    playerData.throwingDice = false;
    playerData.diceVelocity = vector3d(0, 0, 0);
    playerData.angularVelocity = vector3d(0, 0, 0);
    playerData.chosenSide = -1;
    playerData.timeDelta = 0;
    playerData.stopped = false;

    playerData.soundDice = gfc_sound_load("sounds/diceroll.wav", 1, -1);
    playerData.selectedDiceIndex = 0;
    playerData.currentHealth = 30;
    playerData.maxHealth = 30;
    playerData.currentMana = 10;
    playerData.maxMana = 10;
    playerData.inventory = inventory_new();

    List* dices = dice_list_load("config/diceinventory.json");
    for (int i = 0; i < gfc_list_get_count(dices); i++) {
        if (((Dice*)gfc_list_get_nth(dices, i))->isSeed) {
            gfc_list_append(playerData.inventory->diceSeeds, gfc_list_get_nth(dices, i));
        }
        else {
            gfc_list_append(playerData.inventory->diceInventory, gfc_list_get_nth(dices, i));
        }
    }
    gfc_list_delete(dices);
    return playerData;
}

void script_player_freeplayerdata(Script* script) {
    if (script && script->data)
    {
        if (((PlayerData*)(script->data))->soundDice) gfc_sound_free(((PlayerData*)(script->data))->soundDice);
        //  No need to free diceEntity because all entities will be cleaned up with their parent
        inventory_free(((PlayerData*)(script->data))->inventory);
        free(script->data);
    }
}

Entity* script_player_getplayer() {
    return player;
}
PlayerData* script_player_getplayerdata() {
    if (entity_get_script(player, "player"))
        return entity_get_script(player, "player")->data;
    return NULL;
}
/**
 * @brief Called when a script is created.
 */
static void Start(Entity* self, Script* script) {
    if (!player)
    {
        player = self;
    }
    else
    {
        slog("A player already exists.");
        return;
    }
    self->hidden = false;
    script->data = malloc(sizeof(PlayerData));
    if (!script->data) {
        slog("Couldn't allocate memory for player data.");
        return;
    }
    PlayerData playerData = script_player_newplayerdata();
    memcpy(script->data, &playerData, sizeof(PlayerData));
    Vector3D position;
    vector3d_add(position, self->position, vector3d(0, 0, 50));
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(self->rotation);
}

/**
 * @brief Called when a script is created.
 */
static void Think(Entity* self, Script* script) {
    if (script_manager_getgamestate() == GROW) {
        PlayerData* data = script_player_getplayerdata();
        if (data->diceEntity) {
            entity_free(data->diceEntity);
            data->diceEntity = NULL;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_ESCAPE))
        {
            event_manager_fire_event("inventoryToggle");
        }
    }
    if (script_manager_getgamestate() == BATTLE) {
        PlayerData* data = script_player_getplayerdata();
        if (data->diceEntity) {
            entity_free(data->diceEntity);
            data->diceEntity = NULL;
        }
        Vector3D forward, move = { 0 };
        Vector2D w, mouse;
        int mx, my;
        SDL_GetRelativeMouseState(&mx, &my);
        const Uint8* keys;
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

        mouse.x = mx;
        mouse.y = my;

        move = vector3d_get_from_angles(self->rotation);
        move.z = 0;
        vector3d_scale(move, move, 200 * engine_time_delta());
        if (keys[SDL_SCANCODE_W])
        {
            vector3d_add(self->position, self->position, move);
        }
        if (keys[SDL_SCANCODE_S])
        {
            vector3d_add(self->position, self->position, -move);
        }
        move = vector3d_get_from_angles(self->rotation);
        move.z = 0;
        vector3d_rotate_about_z(&move, -GFC_HALF_PI);
        vector3d_scale(move, move, 200 * engine_time_delta());
        if (keys[SDL_SCANCODE_D])
        {
            vector3d_add(self->position, self->position, move);
        }
        if (keys[SDL_SCANCODE_A])
        {
            vector3d_add(self->position, self->position, -move);
        }
        if (keys[SDL_SCANCODE_Z])
            self->position.z -= 20 * engine_time_delta();
        if (keys[SDL_SCANCODE_SPACE])
            self->position.z += 20 * engine_time_delta();

        if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= GFC_PI * engine_time_delta();
        if (keys[SDL_SCANCODE_LEFT])self->rotation.z += GFC_PI * engine_time_delta();

        if (self->rotation.x >= (GFC_HALF_PI * .9f)) self->rotation.x = GFC_HALF_PI * .9f;
        if (self->rotation.x <= -(GFC_HALF_PI * .9f))self->rotation.x = -GFC_HALF_PI * .9f;

        if (self->position.x < -300)    self->position.x = -300;
        else if (self->position.x > 300)    self->position.x = 300;
        if (self->position.y < -300)    self->position.y = -300;
        else if (self->position.y > 300)    self->position.y = 300;
    }
    if (script_manager_getdata()->turn == Player && script_manager_getgamestate() == COMBAT) {
        Bool changed = false;
        PlayerData* data = script_player_getplayerdata();
        if (!data->diceEntity)
            createDiceEntity();
        if (gfc_input_keycode_released(SDL_SCANCODE_LEFT)) {
            data->selectedDiceIndex = data->selectedDiceIndex - 1;
            changed = true;
        }
        if (gfc_input_keycode_released(SDL_SCANCODE_RIGHT)) {
            data->selectedDiceIndex = data->selectedDiceIndex + 1;
            changed = true;
        }
        if (data->selectedDiceIndex < 0) {
            data->selectedDiceIndex = gfc_list_get_count(data->inventory->diceLoadout) - 1;
        }
        else if (data->selectedDiceIndex >= gfc_list_get_count(data->inventory->diceLoadout)) {
            data->selectedDiceIndex = 0;
        }

        if (changed) {
            if (!data->diceEntity) return;
            entity_free(data->diceEntity);
            createDiceEntity();
        }

    }
}

/**
 * @brief Called when a script is created.
 */
static void Update(Entity* self, Script* script) {
    if (!self)return;

    if (script_manager_getgamestate() == GROW)
    {
        Vector3D position;
        vector3d_add(position, self->position, vector3d(0, 0, 100));
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(self->rotation);
    }
    else if (script_manager_getgamestate() == BATTLE)
    {
        Vector3D position;
        vector3d_add(position, self->position, vector3d(0, 0, 300));
        Vector3D rotation = vector3d(-1.2, self->rotation.y, self->rotation.z);
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(rotation);
    }
    else if (script_manager_getgamestate() == COMBAT) {
        Vector3D position = vector3d(-15, 50, 15);
        vector3d_rotate_about_z(&position, self->rotation.z + GFC_HALF_PI);
        vector3d_add(position, position, self->position);
        Vector3D rotation = vector3d(self->rotation.x, self->rotation.y, self->rotation.z - GFC_HALF_PI);
        gf3d_camera_set_position(position);
        gf3d_camera_set_rotation(rotation);
        if (script_manager_getdata()->turn != Player) {
            return;
        }
        PlayerData* data = script_player_getplayerdata();
        if (data->diceEntity) {
            if (!data->throwingDice) {
                Vector3D position = engine_utility_mouseprojectray();
                vector3d_scale(position, position, 40);
                Vector3D cameraPos = { 0 };
                gf3d_camera_get_position(&cameraPos);
                vector3d_add(position, position, cameraPos);
                data->diceEntity->position = vector3d(position.x, position.y, position.z);

                data->diceEntity->rotation.x += 2 * GFC_2PI * engine_time_delta();
                data->diceEntity->rotation.z += 2 * GFC_2PI * engine_time_delta();
            }
            else {

                //  Translational velocity
                Vector3D velocity;
                vector3d_scale(velocity, data->diceVelocity, engine_time_delta());

                vector3d_add(data->diceEntity->position, data->diceEntity->position, velocity);

                //  Rotational Velocity
                Vector3D angularVelocity;
                vector3d_scale(angularVelocity, data->angularVelocity, engine_time_delta());

                vector3d_add(data->diceEntity->rotation, data->diceEntity->rotation, angularVelocity);

                //  https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdf

                if(!data->stopped)
                    ImpulseResolveCollision(data);


                //  Stop if velocity is low enough
                if (vector3d_magnitude(data->angularVelocity) < .1 || data->stopped) {
                    data->diceVelocity.x = 0;
                    data->diceVelocity.y = 0;
                    data->diceVelocity.z = 0;
                    data->angularVelocity.x = 0;
                    data->angularVelocity.y = 0;
                    data->angularVelocity.z = 0;

                    data->timeDelta += engine_time_delta();
                    data->stopped = true;
                }
                else {
                    //  Gravity Decay   -   not strictly mathematically accurate due to derivatives with time
                    data->diceVelocity.z -= engine_time_delta() * 400;
                }

                if (data->stopped) {
                    if (data->timeDelta < .5) {
                        data->diceEntity->rotation.x += 2 * GFC_PI * engine_time_delta();
                        data->diceEntity->position.z += 200 * engine_time_delta();
                    }
                }

                //  Camera position
                Vector3D newPos = vector3d(-data->diceVelocity.x, -data->diceVelocity.y, 50);
                vector3d_add(newPos, newPos, data->diceEntity->position);
                gf3d_camera_set_position(newPos);
                gf3d_camera_set_rotation(vector3d(-1.4, 0, 0));

                if (data->timeDelta > 1) {
                    Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
                    if (!dice || !data->diceEntity) return;
                    dice_activate_effect(dice, data->chosenSide);
                    entity_free(data->diceEntity);
                    data->diceEntity = NULL;
                    script_manager_getdata()->turn = Monster;
                    gfc_sound_play(data->soundDice, 0, 1, -1, -1);
                    data->throwingDice = false;
                }
            }
        }
        if (!data->throwingDice && engine_utility_isleftmousereleased()) {
            data->throwingDice = true;
            data->diceVelocity = engine_utility_mouseprojectray();
            data->diceVelocity.z = 10;
            data->chosenSide = dice_choose_side(gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex));

            vector3d_scale(data->diceVelocity, data->diceVelocity, 15 + 5 * gfc_random());
            data->angularVelocity.x = 2 + 2 * gfc_random();
            data->angularVelocity.z = 2 + 2 * gfc_random();
            data->timeDelta = 0;
            data->stopped = false;

            /*
                Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
                if (!dice || !data->diceEntity) return;
                dice_activate_effect(dice);
                entity_free(data->diceEntity);
                data->diceEntity = NULL;
                script_manager_getdata()->turn = Monster;
                gfc_sound_play(data->soundDice, 0, 1, -1, -1);
            */
        }
        else if (data->throwingDice && engine_utility_isleftmousereleased()) {
            Dice* dice = gfc_list_get_nth(data->inventory->diceLoadout, data->selectedDiceIndex);
            if (!dice || !data->diceEntity) return;
            dice_activate_effect(dice, data->chosenSide);
            entity_free(data->diceEntity);
            data->diceEntity = NULL;
            script_manager_getdata()->turn = Monster;
            gfc_sound_play(data->soundDice, 0, 1, -1, -1);
            data->throwingDice = false;
            data->stopped = false;
        }
    }
}

/**
 * @brief Called when a script is created.
 */
static void Destroy(Entity* self, Script* script) {
    script_player_freeplayerdata(script);
    player = NULL;
}
static void Arguments(Entity* self, Script* script, SJson* json) {
}
Script* script_new_player() {
    return script_new("player", &Start, &Think, &Update, &Destroy, &Arguments);
}


/*eol@eof*/
