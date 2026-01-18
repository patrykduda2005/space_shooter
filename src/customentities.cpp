#include "customentities.h"
#include "ecs.h"
#include "components.h"
#include "resources.h"

Entity* basicBullet(Position pos) {
    auto res = resources->get_component<soundTextureResources>();   
    Area hitboxSize = Area(
            Vec2(res->bull.width, res->bull.height) * -0.5,
            Vec2(res->bull.width, res->bull.height) * 0.5
            );
    Entity* bullet = new Entity();
    bullet->add_component<Render>({.txt = res->bull});
    bullet->add_component<Position>({.x = pos.x, .y = pos.y-45});
    //bullet->add_component<Gravity>({.g = -700.0});
    bullet->add_component<DestroyBeyondWorld>({});
    bullet->add_component<Hitbox>({
            .layer = HitboxLayer::Bullets, 
            .interactsWith = HitboxLayer::Enemies, 
            .collisionBox = hitboxSize,
            .receives = {create_component<Destroy>({})},
            .applies = {create_component<Damage>({.dmg = 1})}
            });
    bullet->add_component<SuckedToBlack>({});
    bullet->add_component<Velocity>({.x = 0, .y = -700});
    return bullet;
}

Entity* sniperBullet(Position pos) {
    Entity* bullet = basicBullet(pos);
    bullet->get_component<Render>()->txt = LoadTexture("surowka-boom.png");
    bullet->get_component<Velocity>()->y *= 2;
    Hitbox* hitbox = bullet->get_component<Hitbox>();
    hitbox->applies = {};
    hitbox->receives.push_back(create_component<Spawn>({
                .comps = {boom(pos)}
                }));
    return bullet;
}

Entity* shatteredBullet(Position pos, Hitbox* hitbox) {
    return basicBullet(pos)->remove_component<Hitbox>()
        ->add_component<Delay>({
                .comps = {
                create_component(*hitbox)
                },
                .delay = 0.25,
                });
}

Entity* shatterBullet(Position pos) {
    Entity* bullet = basicBullet(pos);
    bullet->get_component<Render>()->txt = LoadTexture("surowka-shattering.png");
    Hitbox* hitbox = bullet->get_component<Hitbox>();
    Entity* bullet2 = shatteredBullet(pos, hitbox);
    bullet2->get_component<Velocity>()->x = 500;
    Entity* bullet3 = shatteredBullet(pos, hitbox);
    Entity* bullet4 = shatteredBullet(pos, hitbox);
    bullet4->get_component<Velocity>()->x = -500;

    hitbox->receives.push_back(create_component<Spawn>({.comps = {
                bullet2,
                bullet3,
                bullet4,
                }}));
    return bullet;
}

Entity* blackBullet(Position pos) {
    Entity* bullet = basicBullet(pos);
    bullet->get_component<Render>()->txt = LoadTexture("surowka-black.png");
    bullet->get_component<Velocity>()->y = -100;
    bullet->remove_component<Hitbox>();
    ComponentHandle die = create_component<Delay>({
            .comps = {create_component<Destroy>({})},
            .delay = 4,
            });
    bullet->add_component<Delay>({
            .comps = {create_component<BlackHole>({.g = 2 * 300000}), die},
            .timestamp = GetTime(),
            .delay = 1,
            });
    bullet->remove_component<SuckedToBlack>();
    return bullet;
}

Entity* boom(Position pos) {
    Entity* boom = new Entity();
    Texture2D txt = LoadTexture("boom.png");
    boom->add_component<Render>({.txt = txt});
    Area hitboxSize = Area(
            Vec2(txt.width, txt.height) * -0.5,
            Vec2(txt.width, txt.height) * 0.5
            );
    boom->add_component<Position>(pos);
    boom->add_component<Delay>({
            .comps = {create_component<Destroy>({})},
            .timestamp = GetTime(),
            .delay = 0.1,
            });
    boom->add_component<Hitbox>({
            .layer = HitboxLayer::Effects,
            .interactsWith = HitboxLayer::Enemies,
            .collisionBox = hitboxSize,
            .receives = {create_component<RemoveHitbox>({})},
            .applies = {create_component<Damage>({.dmg = 3})},
            });
    return boom;
}

void spawnPlayer() {
    Vec2 textureSize = Vec2(66, 120);
    Area size = Area(textureSize * -0.5, textureSize * 0.5);

    Entity* rabbit = entities->new_entity();
    rabbit->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    rabbit->add_component<Position>({.x = 500, .y = 700});
    rabbit->add_component<ArrowMovement>({200, 400, 200, 400});
    rabbit->add_component<RestrictToWorld>({});	
	rabbit->add_component<Shooting>({.cooldown = 0.0});
    rabbit->add_component<Hitbox>({
            .layer = HitboxLayer::Players,
            .interactsWith = 0,
            .collisionBox = size,
            });
    rabbit->add_component<Hp>({.hp = 3});
    rabbit->add_component<Invulnerable>({.timer = 0.0f});
    rabbit->add_component<HpOffset>({
            .global = false,
            .vec = size.left_up_corner - Vec2(0, 25),
            });
}

void bounceOffWalls() {
    auto wb = resources->get_component<WorldBorder>();
    if (!wb) return;
    
    for (Entity* ent : entities->get()) {
        auto bounce = ent->get_component<BounceOffWalls>();
        auto pos = ent->get_component<Position>();
        auto vel = ent->get_component<Velocity>();
        
        if (!bounce || !pos || !vel) continue;
        
        bool bounced = false;
        
        if (pos->x < wb->x && vel->x < 0) {
            vel->x = -vel->x * bounce->bounceStrength;
            pos->x = wb->x;
            bounced = true;
        }
        
        if (pos->x > wb->x + wb->width && vel->x > 0) {
            vel->x = -vel->x * bounce->bounceStrength;
            pos->x = wb->x + wb->width;
            bounced = true;
        }
        
        if (pos->y < wb->y && vel->y < 0) {
            vel->y = -vel->y * bounce->bounceStrength;
            pos->y = wb->y;
            bounced = true;
        }
        
        if (pos->y > wb->y + wb->height && vel->y > 0) {
            vel->y = -vel->y * bounce->bounceStrength;
            pos->y = wb->y + wb->height;
            bounced = true;
        }
    }
}


Entity* spawnEnemy(Position pos) {
    Entity* enemy = new Entity();
    enemy->add_component<Position>(pos);
//     enemy->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    enemy->add_component<Hitbox>({
        .layer = HitboxLayer::Enemies,
        .interactsWith = HitboxLayer::Players, 
        .collisionBox = Area(Vec2(-50,-50), Vec2(50,50)),
        .applies = {create_component<Damage>({.dmg = 1})}
    });
    enemy->add_component<Hp>({.hp = 10});
    enemy->add_component<Velocity>({
        .x = 0,
        .y = 0
    });
    
    return enemy;  
}


void spawnBasicEnemy(Position pos) {
        Entity* enemy = spawnEnemy(pos);
        enemy->add_component<Render>({.txt = LoadTexture("Normalny_przeciwnik.png")});
        Velocity* vel = enemy->get_component<Velocity>();
        if (vel) {
                vel->x = 80;
                vel->y = 60;
        }

        enemy->get_component<Hp>()->hp = 12;
        BounceOffWalls bounce;
        bounce.bounceStrength = 1.0f;
        enemy->add_component<BounceOffWalls>({.bounceStrength = 1.0f});
        entities->attach(enemy);
}

void spawnTankEnemy(Position pos) {
    Entity* enemy = spawnEnemy(pos);
    enemy->add_component<Render>({.txt = LoadTexture("Ciezki_przeciwnik.png")});
    enemy->add_component<Velocity>({.x = 100, .y = 40});
    enemy->get_component<Hp>()->hp = 25;
    entities->attach(enemy);
}

Entity* createEnemyBullet(Position pos) {
        Entity* bullet = new Entity();
        auto res = resources->get_component<soundTextureResources>();

        bullet->add_component<Render>({
                .txt = LoadTexture("surowka.png")
        });
        bullet->add_component<Position>(pos);
        bullet->add_component<Velocity>({
                .x = 0,
                .y = 300
        });
        bullet->add_component<DestroyBeyondWorld>({});
        bullet->add_component<SuckedToBlack>({});

        bullet->add_component<Hitbox>({
                .layer = HitboxLayer::EnemyBullets,
                .interactsWith = HitboxLayer::Players,
                .collisionBox = Area(
                        Vec2(-10, -20),
                        Vec2(10, 20)
                ),
                .applies = {create_component<Damage>({
                        .dmg = 1
                })}
        });

        return bullet;
}

void spawnShootingEnemy(Position pos) {
    Entity* enemy = spawnEnemy(pos);
    enemy->add_component<Render>({.txt = LoadTexture("strzelajacy_przeciwnik.png")});
    enemy->add_component<Velocity>({.x = 100, .y = 40});
    enemy->get_component<Hp>()->hp = 8;
    enemy->add_component<EnemyShooting>({.cooldown = 0.0f});

    entities->attach(enemy);
}

void updateInvulnerability(float d) {
    for (Entity* ent : entities->get()) {
        auto invuln = ent->get_component<Invulnerable>();
        if (!invuln) continue;
        
        invuln->timer -= d;
        if (invuln->timer <= 0) {
            ent->remove_component<Invulnerable>();
        }
    }
}

void updateShootingEnemies(float d) {
    for (Entity* ent : entities->get()) {
        auto enemyShooting = ent->get_component<EnemyShooting>();
        if (!enemyShooting) continue;
        
        if (enemyShooting->cooldown > 0) {
            enemyShooting->cooldown -= d;
        }
        
        if (enemyShooting->cooldown <= 0) {
            auto pos = ent->get_component<Position>();
            if (pos) {
                Entity* bullet = createEnemyBullet(*pos);
                
                float randomX = (float)GetRandomValue(-50, 50);
                if (auto vel = bullet->get_component<Velocity>()) {
                    vel->x += randomX;
                }
                
                entities->attach(bullet);
                enemyShooting->cooldown = 2.0f;
            }
        }
    }
}

void updateEnemyShooting(float d) {
    for (Entity* ent : entities->get()) {
        auto enemyShooting = ent->get_component<EnemyShooting>();
        if (!enemyShooting) continue;
        
        if (enemyShooting->cooldown > 0) {
            enemyShooting->cooldown -= d;
        }
        
        if (enemyShooting->cooldown <= 0) {
            auto pos = ent->get_component<Position>();
            if (pos) {
                Entity* bullet = createEnemyBullet(*pos);
                
                float randomX = (float)GetRandomValue(-50, 50);
                if (auto vel = bullet->get_component<Velocity>()) {
                    vel->x += randomX;
                }
                
                entities->attach(bullet);
                enemyShooting->cooldown = 2.0f;
            }
        }
    }
}
