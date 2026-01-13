#pragma once
#include "ecs.h"
#include "components.h"


Entity* basicBullet(Position pos);
Entity* sniperBullet(Position pos);
Entity* shatterBullet(Position pos);
Entity* blackBullet(Position pos);
Entity* boom(Position pos);
void spawnPlayer();
void spawnEnemy(Position pos);
