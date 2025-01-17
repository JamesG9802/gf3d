#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gfc_audio.h"

#include "engine_time.h"
#include "event_manager.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "scene.h"

#include "script_defs.h"

extern int __DEBUG;
extern Entity* player;

int main(int argc,char *argv[])
{
    srand(time(0));

    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    float mouseFrame = 0;
    Matrix4 skyMat;
    Model *sky;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }

    // init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gfc_audio_init(64, 64, 8, 8, 1, 0);
    gf2d_draw_manager_init(1000);
    engine_time_init();
    event_manager_init();

    slog_sync();
    
    entity_system_init(1024);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    
    scene_load("config/main.scene");

    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    
    sky = gf3d_model_load("models/sky.model", NULL);
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    


    char mousepos[128];
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        engine_time_renew();
        gfc_input_update();
        engine_utility_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)
            mouseFrame = 0;

        entity_think_all();
        entity_update_all();

        gf3d_vgraphics_render_start();

        //3D draws
        entity_draw_all();

        if(player)
        {
            //2D draws
            gf2d_draw_rect_filled(gfc_rect(10, 10, 1000, 32), gfc_color8(128, 128, 128, 255));

            mousepos[0] = '\0';
            sprintf_s(mousepos, sizeof(char) * 128, "Rotation: (%.2f, %.2f, %.2f) Player (%.2f, %.2f, %.2f)",
                player->rotation.x, player->rotation.y, player->rotation.z,
                player->position.x, player->position.y, player->position.z);

            gf2d_font_draw_line_tag(mousepos, FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));

            gf2d_draw_rect(gfc_rect(10, 10, 1000, 32), gfc_color8(255, 255, 255, 255));

            gf2d_sprite_draw(mouse, vector2d(mousex, mousey), vector2d(2, 2), vector3d(8, 8, 0), gfc_color(0.3, .9, 1, 0.9), (Uint32)mouseFrame);
        }
        
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))
            done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
