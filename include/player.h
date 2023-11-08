#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "object.h"

/**
 * @brief Create a new player entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an player entity pointer on success
 */

typedef struct ObjectLinked_S
{
	Object* self;
	Object* next;
}ObjectLinked;



ObjectLinked* getChildLink();

Entity *player_new(Vector3D position);


#endif
