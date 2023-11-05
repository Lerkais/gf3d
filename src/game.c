
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

#include "collision.h"

#include "special_attack.h"

#define CHARENEMY_FILEPATH "\\Ora\\data\\charenemyData\.json"
//C:/Users/funru/source/repos/projectOra/Ora/data/charenemyData.json

//void explore();

//void battle();

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

    Scene* exploreSceneInstance;
    Scene* battleSceneInstance;

    exploreSceneInstance = NewScene(explorationScene); 
    Object* battler1;
    Object* battler2;

    LoadScene(exploreSceneInstance);

    printf("\n");
    printf(exploreSceneInstance->name);

    

    printf("\nScenes Size Before Adding: %i", sceneManager->size);

    AddSceneToManager(exploreSceneInstance);
    sceneManager->activeScene = exploreSceneInstance;

    printf("\nScenes Size After Adding: %i", sceneManager->size);


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


    Object** woofs = NewObject();
    

    //INIT CHARACTERS
    for (int i = 0; i < 9; i++)
    {
        char buff[32];
        char filepath[100];
        woofs[i] = NewObject();
        char* str = "models/c";
        
        char* str2 = _itoa(i, buff, 10);
        char* str3 = ".model";
        strcpy(filepath,str);
        strcat(filepath, str2);
        strcat(filepath, str3);
        woofs[i]->model = gf3d_model_load(filepath);

        woofs[i]->position.x = -100 + i * 20;
        woofs[i]->position.y = 100;

        woofs[i]->Type = charenemy;
        ObjectData cobj;
        cobj.charenemyObj.charenemy = charas[i]; //todo shuffle master list to randomize players AFTER INITIALIZING
        woofs[i]->Data = cobj;
        

        vector3d_scale(woofs[i]->scale, woofs[i]->scale, .2f);

        AddSceneObject(exploreSceneInstance, woofs[i]);

    }

    battler1 = woofs[1];
    battler2 = woofs[0];

    initAttacks();
     
    //Special cases will need more than one proj


    //---------------------------------------------------------------------------

    Vector3D camPos;
    vector3d_copy(camPos,woofs[0]->position);

    camPos.y -= 200;
    camPos.z += 100;



    
    Entity* player = player_new(camPos);
    player->canMove = true;
    addChild(woofs[0]);
    addChild(woofs[1]);
    
    Object* camTarget = woofs[0];
    

    //PopulateButtons();
    float xys = 0;
    unsigned char cds;

    oratimeinit();
    slog("Ora Explore begin");
    while(!done)
    {
        tick();

        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex, &mousey);

        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);


        //logical/physics updates 
        entity_think_all();

        UpdateObjects(sm->activeScene);

        entity_update_all();

        gf3d_vgraphics_render_start();

        gf3d_model_draw_sky(sky, skyMat, gfc_color(1, 1, 1, 1));

        switch (sm->activeScene->type) 
        {
        case explorationScene:
            for (ObjectLinked* iter = getChildLink(); iter->self != NULL; iter = iter->next)
            {
                for (int i = 0; i < sm->activeScene->objectCount; i++)
                {
                    Object* otherobj = sm->activeScene->objects[i];
                    if (iter->self == otherobj) continue;
                    if (isColliding(iter->self, otherobj))
                    {
                        //otherobj->isActive = false;
                        battleSceneInstance = NewScene(battleScene);

                        battler1 = iter->self;
                        battler2 = otherobj;
                        Object* temp1 = NewObject();
                        Object* temp2 = NewObject();
                        memcpy(temp1, battler1, sizeof(Object));
                        memcpy(temp2, battler2, sizeof(Object));
                        battleSceneInstance->data.battleData.playerChar = temp1;
                        battleSceneInstance->data.battleData.enemyChar = temp2;
                        AddSceneObject(battleSceneInstance,temp1);
                        AddSceneObject(battleSceneInstance,temp2);
                        temp1->position = vector3d(-50, 0, 0);
                        temp2->position = vector3d(50, 0, 0);
                        
                        camTarget = NewObject();
                        camTarget->position = vector3d(0, 0, 0);

                        LoadScene(battleSceneInstance);

                        camPos = player->position;

                        player->position = vector3d(0, -200, 100);

                        player->canMove = false;
                    }
                }
            }
            gf3d_camera_look_at(player->position, camTarget->position, vector3d(0, 0, 1));
            gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

            break;
        case battleScene:

            



            //logic
            if (true) {
                Object* battlers[2];
                Object* creator = sm->activeScene->data.battleData.playerChar;
                Object* target = sm->activeScene->data.battleData.enemyChar;
                battlers[0] = creator;
                battlers[1] = target;

                if (target->Data.charenemyObj.charenemy->isDead)
                {
                    RemoveAndDestroySceneObject(sm->activeScene, target);
                    RemoveAndDestroySceneObject(sm->activeScene, creator);
                    LoadScene(exploreSceneInstance);
                    RemoveSceneObject(exploreSceneInstance, battler2);
                    player->position = camPos;
                    camTarget = woofs[0];
                    player->canMove = true;
                    break;
                }

                for (unsigned char i = 0; i < 2; i++)
                {
                    cds = tickcds(battlers[i]->Data.charenemyObj.charenemy);

                    if (cds & 0b100)
                    {
                        Object* projtoadd = basicAttack(battlers[i], battlers[i^1]);
                        AddObjectToActiveScene(projtoadd);
                    }
                    if (cds & 0b010)
                    {
                        Object* projtoadd = heavyAttack(battlers[i], battlers[i ^ 1]);
                        AddObjectToActiveScene(projtoadd);
                    }
                    if (cds & 0b001)
                    {
                        //special attack
                    }
                }


                
            }
            



            //Names

            gf2d_draw_rect_filled(gfc_rect(600, 52, 200, 40), gfc_color8(128, 128, 10, 255));
            gf2d_font_draw_line_tag(sm->activeScene->data.battleData.enemyChar->Data.charenemyObj.charenemy->name, FT_H1, gfc_color(1, 1, 1, 1), vector2d(600, 52));

            gf2d_draw_rect_filled(gfc_rect(10, 52, 200, 40), gfc_color8(128, 128, 10, 255));
            gf2d_font_draw_line_tag(sm->activeScene->data.battleData.playerChar->Data.charenemyObj.charenemy->name, FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52));




            //Health

            char buff[30];

            gf2d_draw_rect_filled(gfc_rect(600, 52 + 25, 200, 34), gfc_color8(128, 128, 10, 255));
            gf2d_font_draw_line_tag(_itoa(sm->activeScene->data.battleData.enemyChar->Data.charenemyObj.charenemy->health,buff,10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(600, 52 + 25));

            gf2d_draw_rect_filled(gfc_rect(10, 52 + 25, 200, 32), gfc_color8(128, 128, 10, 255));
            gf2d_font_draw_line_tag(_itoa(sm->activeScene->data.battleData.playerChar->Data.charenemyObj.charenemy->health,buff,10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52 + 25));

            //crits

           

            //Camera


            gf3d_camera_look_at(player->position, camTarget->position, vector3d(0, 0, 1));
            gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

            break;
        }

        



        //3D draws

        if (sm->activeScene->type == battleScene)
        {
            sm->activeScene->data.battleData.playerChar->rotation.z += .1 * deltaTime;
            sm->activeScene->data.battleData.enemyChar->rotation.z += .1 * deltaTime;
        }

        DrawSceneObjects(sm->activeScene);

        

        for (int i = 0; i < 9; i++) {
            woofs[i]->rotation.z += .1*deltaTime;
        }

        /*for (a = 0; a < 100; a++)
        {
            particle[a].position.z += 1*deltaTime;
            particle[a].size -= 1*deltaTime;
            gf3d_particle_draw(&particle[a]);
        }*/
        
        //2D draws
        gf2d_draw_rect_filled(gfc_rect(10, 10, 1000, 32), gfc_color8(128, 128, 128, 255));
        gf2d_font_draw_line_tag("Press ALT+F4 to exit", FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 10));

       

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


void explore()
{

}

    /*eol@eof*/