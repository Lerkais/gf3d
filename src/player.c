#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"

#include "oratime.h"

Vector3D deltaPosition;
ObjectLinked* firstchild = NULL;

void player_think(Entity *self);
void player_update(Entity *self);

void addChild(Object* child)
{
    ObjectLinked* temp = malloc(sizeof(ObjectLinked));
    if (!temp) return;
    temp->next = firstchild;
    temp->self = child;
    firstchild = temp;
    
}

void deleteChild(Object* child) //END BUFFER MUST BE CREATED VIA PLAYER NEW
{
    if (!child) return; //child null
    if (!(firstchild->next))return; //No children

    ObjectLinked* last = firstchild;
    ObjectLinked* current = firstchild->next;

    if (last->self == child) { //Firstchild to be removed
        free(last);
        firstchild = current;
        return;
    }
    while (current->self) { //iterate until end buffer
        if (current->self == child) {
            last->next = current->next;
            free(current);
        }
    }
}

ObjectLinked* getChildLink()
{
    return firstchild;
}



//--------------------------------------------------------------------------

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;

    deltaPosition = vector3d(0, 0, 0);


    int a = sizeof(ObjectLinked);
    ObjectLinked* endBuffer = malloc(a);
    if (!endBuffer) { slog("Failed to allocate end buffer for the player's children"); return; }
    endBuffer->next = NULL;
    endBuffer->self = NULL;

    firstchild = endBuffer;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = player_think;
    ent->update = player_update;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    ent->hidden = 1;
    return ent;
}


void player_think(Entity *self)
{
    if(!(self->canMove))
        return;

    Vector3D forward = {0};
    Vector3D right = {0};
    Vector2D w,mouse;
    Vector3D scaled;
    float speed = 10;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    //forward.x = w.x;
    //forward.y = w.y;
    forward.x = 0;
    forward.y = 1;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = 1;
    right.y = 0;
    if (keys[SDL_SCANCODE_LSHIFT]) speed = 20;
    else speed = 10;

    //record position before changes
    Vector3D positionPrevious;
    vector3d_copy(positionPrevious, self->position);


    if (keys[SDL_SCANCODE_W])
    {   

        vector3d_copy(scaled,forward);
        vector3d_scale(scaled, scaled, speed*deltaTime);
        vector3d_add(self->position,self->position,scaled);
        
    }
    if (keys[SDL_SCANCODE_S])
    {
        vector3d_copy(scaled, -forward);
        vector3d_scale(scaled, scaled, speed*deltaTime);
        vector3d_add(self->position,self->position,scaled);        
    }
    if (keys[SDL_SCANCODE_D])
    {
        vector3d_copy(scaled, right);
        vector3d_scale(scaled, scaled, speed*deltaTime);
        vector3d_add(self->position,self->position,scaled);
    }
    if (keys[SDL_SCANCODE_A])    
    {
        vector3d_copy(scaled, -right);
        vector3d_scale(scaled, scaled, speed*deltaTime);
        vector3d_add(self->position,self->position,scaled);
    }
    if (keys[SDL_SCANCODE_SPACE])self->position.z += speed*deltaTime;
    if (keys[SDL_SCANCODE_LCTRL])self->position.z -= speed*deltaTime;

    deltaPosition = vector3d_subbed(self->position, positionPrevious);

    for (ObjectLinked* iter = firstchild;iter->self; iter = iter->next)
    {
        vector3d_add(iter->self->position,iter->self->position, deltaPosition);
        
    }

    
    //if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    //if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    //if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    //if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;
    
    //if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    //if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    
}

void player_update(Entity *self)
{
    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;
    
    if (!self)return;
    
    vector3d_copy(position,self->position);
    vector3d_copy(rotation,self->rotation);
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);


}

/*eol@eof*/
