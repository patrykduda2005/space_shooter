#pragma once
#include "ecs.h"
#include "components.h"


Entity* basicBullet(Position pos);
Entity* sniperBullet(Position pos);
Entity* shatterBullet(Position pos);
Entity* blackBullet(Position pos);
Entity* boom(Position pos);
void spawnPlayer();
Entity* spawnEnemy(Position pos);
void spawnBasicEnemy(Position pos);
void spawnFormationEnemy(Position pos);
void spawnStrongEnemy(Position pos);
void spawnFastEnemy(Position pos);
Entity* createEnemyBullet(Position pos);
Entity* wall(Position pos);
