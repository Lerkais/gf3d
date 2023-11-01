
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

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"

#include "charenemy.h"



extern int __DEBUG;

    
#include <stdio.h>
#include <stdlib.h>

#include "sceneManager.c"

#include "button.h"

#include "oratime.h"

#define CHARENEMY_FILEPATH "\\Ora\\data\\charenemyData\.json"
//C:/Users/funru/source/repos/projectOra/Ora/data/charenemyData.json

int main(int argc, char* argv[]) 
{
    char CWD[256];
    CWD[0] = '\0';
    getcwd(CWD, 256);
    unsigned char size = strlen(CWD);
    CWD[size-5] = '\0';
    char* path = strncat(CWD, CHARENEMY_FILEPATH, 37);



    slog(path);

    FILE* f = fopen(path, "r");

    Charenemy** charas = populateCharacters(path);
    //unsigned char lsize = getMasterListSize();

    DEBUGprintCharacters();

    //----------------------------------------------------

    InitSceneManager();
    SceneManager* sm = GetSceneManager();

    Scene* meow;

    meow = NewScene(otherscene); 

    LoadScene(meow);

    printf("\n");
    printf(meow->name);

    

    printf("\nScenes Size Before Adding: %i", sceneManager->size);

    AddSceneToManager(meow);
    sceneManager->activeScene = meow;

    printf("\nScenes Size After Adding: %i", sceneManager->size);

    

    Object* woof = NewObject();

    char* chirp = "chirpy";

    woof->Data.Other = chirp;

    AddSceneObject(meow, woof);

    //char* tst = (char*)(meow->objects[0]->Data.Other);

    printf("\n");
    //printf(tst);


    //-------------------------------------------------------------------------

    int done = 0;
    int a;

    Sprite* mouse = NULL;
    int mousex, mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World* w;
    Entity* agu;
    Particle particle[100];
    Matrix4 skyMat;
    Model* sky;

    for (a = 1; a < argc; a++)
    {
        if (strcmp(argv[a], "--debug") == 0)
        {
            __DEBUG = 1;
        }
    }

    init_logger("gf3d.log", 0);
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);

    slog_sync();

    entity_system_init(1024);

    mouse = gf2d_sprite_load("images/pointer.png", 32, 32, 16);


    agu = agumon_new(vector3d(0, 0, 0));
    if (agu)agu->selected = 1;
    w = world_load("config/testworld.json");

    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1, 1, 1));
    player_new(vector3d(-50, 0, 0));

    for (a = 0; a < 100; a++)
    {
        particle[a].position = vector3d(gfc_crandom() * 100, gfc_crandom() * 100, gfc_crandom() * 100);
        particle[a].color = gfc_color(0, 0, 0, 1);
        particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
        particle[a].size = 100 * gfc_random();
    }
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat, vector3d(100, 100, 100));





    //---------------------------------------------------------------------------

    woof->model = gf3d_model_load("models/troop.model");


    //PopulateButtons();

    float xys = 0;

    slog("gf3d main loop begin");
    oratimeinit();
    while (!done)
    {
        tick();

        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex, &mousey);

        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all();

        UpdateObjects(sm->activeScene);


        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

        //3D draws

        
        gf3d_model_draw_sky(sky, skyMat, gfc_color(1, 1, 1, 1));

        DrawSceneObjects(meow);

        woof->rotation.z += .5*deltaTime;
        //DrawObject(woof);



        for (a = 0; a < 100; a++)
        {
            particle[a].position.z += 1*deltaTime;
            particle[a].size -= 1*deltaTime;
            gf3d_particle_draw(&particle[a]);
        }

        
        //todo add delta time for each frame

        
        //2D draws
        gf2d_draw_rect_filled(gfc_rect(10, 10, 1000, 32), gfc_color8(128, 128, 128, 255));
        gf2d_font_draw_line_tag("Press ALT+F4 to exit", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));

        gf2d_draw_rect_filled(gfc_rect(10, 52, 200, 32), gfc_color8(128, 128, 10, 255));
        gf2d_font_draw_line_tag("Click Here", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52));

        gf2d_draw_rect(gfc_rect(10, xys, 1000, 32), gfc_color8(255, 255, 255, 255));

        xys += 1*deltaTime;

        //Button Draws


        //Mouse Draw
        gf2d_sprite_draw(mouse, vector2d(mousex, mousey), vector2d(2, 2), vector3d(8, 8, 0), gfc_color(0.3, .9, 1, 0.9), (Uint32)mouseFrame);

        

        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition


    }
    world_delete(w);
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());

    return 0;
}

    /*eol@eof*/