#include "components.h"
#include "ecs.h"
#include "raylib.h"
#include "resources.h"
#include "customentities.h"
#include <cmath>
#include <string>
#include <iostream>



void updateGravity(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto grav = ent->get_component<Gravity>();
        auto pos = ent->get_component<Position>();
        if (pos && grav) {
            pos->y += grav->g * d;
        }
    }
}

void updateVelocity(float d){
    for(int i = 0; i < entities->get().size(); i++){
        auto ent = entities->get()[i];
        auto vel = ent->get_component<Velocity>();
        auto pos = ent->get_component<Position>();
        if(pos && vel){
            float maxSpeed = 1500;
            pos->x += vel->x * d;
            pos->y += vel->y * d;
            float speed = vel->x * vel->x + vel->y * vel->y;
            if (speed > maxSpeed*maxSpeed) {
                float scalar = maxSpeed*maxSpeed / speed;
                vel->x *= scalar;
                vel->y *= scalar;
            }
        }
    }
}

void renderThings(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto ren = ent->get_component<Render>();
        auto pos = ent->get_component<Position>();
        
        // Sprawdź najpierw czy to item
        auto itemRender = ent->get_component<ItemRender>();
        if (itemRender && pos) {
            DrawTexture(itemRender->txt, pos->x - itemRender->txt.width/2, pos->y - itemRender->txt.height/2, WHITE);
            continue; // Nie renderuj dalej
        }
        
        // Normalne renderowanie
        if (ren && pos) {
            DrawTexture(ren->txt, pos->x - ren->txt.width/2, pos->y - ren->txt.height/2, WHITE);
        }
    }
}

void shoot(int tab){//, int *ammoPointer) {
    auto keyb = resources->get_component<KeyBinds>();
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto shootComp = ent->get_component<Shooting>();
        auto vel = ent->get_component<Velocity>();
        auto res = resources->get_component<soundTextureResources>();   
        auto ammoComp = resources->get_component<AmmoCounter>();

        if (pos && shootComp) {
            if (shootComp->cooldown > 0) shootComp->cooldown -= GetFrameTime();
            if (IsKeyDown(keyb->shoot) && shootComp->cooldown <= 0 && tab == 1) {
                // Create a new bullet entity
                PlaySound(res->shootingsfx);
                entities->attach(shatterBullet(*pos));
                //std::cout << "Shooting!\n";
                shootComp->cooldown = 0.25; // half a second cooldown
            }
            else if (IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[2] > 0 && shootComp->cooldown <= 0 && tab == 3) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);

                entities->attach(blackBullet(*pos));
#if 0
                entities->attach(basicBullet(*pos));
              
                // Create a second bullet entity
                Entity* bullet2 = basicBullet(*pos);
                bullet2->get_component<Velocity>()->x = 150;
                entities->attach(bullet2);
                
                // Create a third bullet entity
                Entity* bullet3 = basicBullet(*pos);
                bullet3->get_component<Velocity>()->x = -150;
                entities->attach(bullet3);
#endif
                
                if (ammoComp->currentAmmo[2] > 0) ammoComp->currentAmmo[2] -= 1;
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[1] > 0 && shootComp->cooldown <= 0 && tab == 2) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);

                entities->attach(basicBullet(*pos));
#if 0                
                Entity* bullet = basicBullet(*pos);
                bullet->get_component<Velocity>()->x = -100;
                entities->attach(bullet);
              
                // Create a second bullet entity
                Entity* bullet2 = basicBullet(*pos);
                bullet2->get_component<Velocity>()->x = 100;
                entities->attach(bullet2);
#endif
             
                if(ammoComp->currentAmmo[1] > 0) ammoComp->currentAmmo[1] -= 1;
                shootComp->cooldown = 0.25; // half a second cooldown
            }
        }
    }
}

void arrowMovement(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto arr = ent->get_component<ArrowMovement>();
        auto pos = ent->get_component<Position>();
        auto keyb = resources->get_component<KeyBinds>();
        if (arr && pos) {
            if (IsKeyDown(keyb->right)) pos->x += arr->eastSpeed * d;
            if (IsKeyDown(keyb->left)) pos->x -= arr->westSpeed * d;
            if (IsKeyDown(keyb->up)) pos->y -= arr->northSpeed * d;
            if (IsKeyDown(keyb->down)) pos->y += arr->southSpeed * d; 
        }
    }
}

void restrictToWorld(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto restrict = ent->get_component<RestrictToWorld>();
        auto wb = resources->get_component<WorldBorder>();
        if (wb && pos && restrict) {
            if (pos->x > wb->x + wb->width) pos->x = wb->x + wb->width;
            if (pos->y > wb->y + wb->height) pos->y = wb->y + wb->height;
            if (pos->x < wb->x) pos->x = wb->x;
            if (pos->y < wb->y) pos->y = wb->y;
        }
    }
}

void destroyBeyondWorld() {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto destroy = ent->get_component<DestroyBeyondWorld>();
        auto wb = resources->get_component<WorldBorder>();
        if (wb && pos && destroy) {
            if (pos->x > wb->x + wb->width 
                    || pos->y > wb->y + wb->height
                    || pos->x < wb->x
                    || pos->y < wb->y)
                entities->kill_entity(ent);
        }
    }
}

// O brachu..
typedef struct {
    Entity *ent;
    std::vector<Hitbox *> hitboxes;
    Position *pos;
} CollideCandidate;

void detectCollision() {
    // Szuka entity z pozycja i hitboxem
    std::vector<CollideCandidate> colliders;
    std::vector<CollideCandidate> collidees;
    for (Entity *collider : entities->get()) {
        CollideCandidate candidate = {.ent = collider};
        candidate.hitboxes = collider->get_components<Hitbox>();
        candidate.pos = collider->get_component<Position>();
        if (!candidate.pos || candidate.hitboxes.empty()) continue; 
        colliders.push_back(candidate);
        collidees.push_back(candidate);
    }

    // Szukamy teraz czy cos ze soba kolliduje
    for (CollideCandidate collider : colliders) {
        for (CollideCandidate collidee : collidees) {
            if (collider.ent == collidee.ent) continue;
            for (Hitbox *colliderHitbox : collider.hitboxes) {
                for (Hitbox *collideeHitbox : collidee.hitboxes) {
                    if ((colliderHitbox->interactsWith & collideeHitbox->layer) == 0) continue; // Sprawdzenie layerow
                    Area globalColliderHitbox = colliderHitbox->collisionBox + Vec2(collider.pos->x, collider.pos->y);
                    Area globalCollideeHitbox = collideeHitbox->collisionBox + Vec2(collidee.pos->x, collidee.pos->y);
                    if (globalColliderHitbox.overlaps(globalCollideeHitbox) == false) continue; // Sprawdzenie czy sie kolliduja
                    for (ComponentHandle component : colliderHitbox->receives) {
                        collider.ent->add_component(component);
                    }
                    for (ComponentHandle component : colliderHitbox->applies) {
                        collidee.ent->add_component(component);
                    }
                }
            }
        }
    }

}

void removeHitbox() {
    for (Entity *ent : entities->get()) {
        auto removeHitbox = ent->get_component<RemoveHitbox>();
        if (!removeHitbox) continue;
        ent->remove_component<Hitbox>();
    }
}

void outlineColliders() {
    for (Entity *ent : entities->get()) {
        auto col = ent->get_components<Hitbox>();
        auto pos = ent->get_component<Position>();
        if (!pos || col.empty()) continue;
        for (Hitbox *hitbox : col) {
            Area global_col = hitbox->collisionBox + Vec2(pos->x, pos->y);
            DrawRectangleLines(global_col.left_up_corner.x, global_col.left_up_corner.y, global_col.getWidth(), global_col.getHeight(), YELLOW);
        }
    }
}

void destroy() {
    for (Entity *ent : entities->get()) {
        auto del = ent->get_component<Destroy>();
        if (!del) continue;
        
        if (ent->get_component<ArrowMovement>()) {
            ent->remove_component<Destroy>();
            continue;
        }
        
        entities->kill_entity(ent);
    }
}

void ammoCounter(int type){ 
    auto ammoComp = resources->get_component<AmmoCounter>();
    
    if(type == 1){
			DrawText("o", 875,675,35,WHITE);
			DrawText("o", 885,675,35,WHITE);
			DrawText("1x", 920,675,35,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,705,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,730,25,WHITE);
		}
		else if(type == 2){ 
			DrawText("o", 875,675,25,WHITE);
			DrawText("o", 885,675,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,700,35,WHITE);
			DrawText("2x", 920,700,35,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,730,25,WHITE);
		}
		else if(type == 3){ 
			DrawText("o", 875,675,25,WHITE);
			DrawText("o", 885,675,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,700,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,730,35,WHITE);
			DrawText("3x", 920,730,35,WHITE);
		}

}

void displayhp() {
    for (Entity* ent : entities->get()) {
        auto pos = ent->get_component<Position>();
        auto hp = ent->get_component<Hp>();
        if (!pos || !hp) continue;
        HpOffset *hpoffset;
        char hpText[100];
        std::snprintf(hpText, 100, "Hp: %d", hp->hp);
        if ((hpoffset = ent->get_component<HpOffset>())) {
            if (hpoffset->global)
                DrawText(hpText,
                        hpoffset->vec.x,
                        hpoffset->vec.y, 25, RED);
            else
                DrawText(hpText,
                        pos->x + hpoffset->vec.x,
                        pos->y + hpoffset->vec.y, 25, RED);
        } else
            DrawText(hpText, pos->x, pos->y, 25, RED);
    }
}

void damage() {
    for (Entity* ent : entities->get()) {
        auto dmg = ent->get_component<Damage>();
        auto hp = ent->get_component<Hp>();
        auto invuln = ent->get_component<Invulnerable>();

        if(invuln && invuln->timer > 0) {
            ent->remove_component<Damage>();
            continue;
        }

        if (!dmg || !hp) continue;
        hp->hp -= dmg->dmg;
        ent->remove_component<Damage>();

        if (ent->get_component<ArrowMovement>()) {
            ent->add_component<Invulnerable>({.timer = 1.0f});
        }
    }
}

void die() {
    for (Entity* ent : entities->get()) {
        auto hp = ent->get_component<Hp>();
        if (hp && hp->hp <= 0) {
            // Sprawdź czy to wróg (nie gracz)
            auto hitbox = ent->get_component<Hitbox>();
            if (hitbox && (hitbox->layer & HitboxLayer::Enemies)) {
                auto pos = ent->get_component<Position>();
                if (pos) {
                    // 45% szansy na drop
                    int chance = GetRandomValue(1, 100);
                    if (chance <= 45) {
                        // Losowy typ itemu: 1 - ammo2, 2 - ammo3, 3 - hp
                        int dropType = GetRandomValue(1, 3);
                        spawnDropItem(*pos, dropType);
                    }
                }
            }
            entities->kill_entity(ent);
        }
    }
}

const char* GetKeyText(int key) {
    switch (key) {
        // Ręczna obsługa klawiszy, których Raylib nie wyświetla poprawnie
        case KEY_SPACE:         return "SPACE";
        case KEY_LEFT_SHIFT:    return "L-SHIFT";
        case KEY_RIGHT_SHIFT:   return "R-SHIFT";
        case KEY_LEFT_CONTROL:  return "L-CTRL";
        case KEY_RIGHT_CONTROL: return "R-CTRL";
        case KEY_LEFT_ALT:      return "L-ALT";
        case KEY_TAB:           return "TAB";
        case KEY_ENTER:         return "ENTER";
        case KEY_BACKSPACE:     return "BACK";
        
        // Strzałki (często też są puste w standardowym GetKeyName)
        case KEY_UP:    return "UP";
        case KEY_DOWN:  return "DOWN";
        case KEY_LEFT:  return "LEFT";
        case KEY_RIGHT: return "RIGHT";

        // Dla całej reszty używamy wbudowanej funkcji Rayliba
        default: return GetKeyName(key);
    }
}

void spawn() {
    for (Entity* ent : entities->get()) {
        auto spawn = ent->get_component<Spawn>();
        if (!spawn) continue;
        for (Entity* spawned : spawn->comps) {
            Position* hostPos;
            Delay* delay;
            if ((hostPos = ent->get_component<Position>()))
                *spawned->get_component<Position>() = *hostPos;
            if ((delay = spawned->get_component<Delay>())) {
                delay->timestamp = GetTime();
                //spawned->add_component<Delay>(*delay);
            }
            entities->attach(spawned);
        }
        ent->remove_component<Spawn>();
    }
}

void delay() {
    for (Entity* ent : entities->get()) {
        auto delays = ent->get_components<Delay>();
        if (delays.empty()) continue;
        for (Delay* delay : delays) {
            if ((GetTime() - delay->timestamp) > delay->delay) {
                for (ComponentHandle comp : delay->comps) {
                    if (registry->_get_id(typeid(Delay).name()) == comp.id)
                        (*(Delay*)(comp.comp)).timestamp = GetTime(); //Oh man ten system..
                    ent->add_component(comp);
                }
                ent->remove_component<Delay>();
            }
        }
    }
}

void suckToBlack(float d) {
    for (Entity* ent : entities->get()) {
        auto black = ent->get_component<BlackHole>();
        auto pos = ent->get_component<Position>();
        if (!black || !pos) continue;
        for (Entity* suckee : entities->get()) {
            if (suckee == ent || !suckee->get_component<SuckedToBlack>()) continue;
            auto suckeeVel = suckee->get_component<Velocity>();
            auto suckeePos = suckee->get_component<Position>();
            auto suckeeblack = suckee->get_component<BlackHole>();
            auto suckeeArrow = suckee->get_component<ArrowMovement>();
            if (!suckeePos || !suckeeVel || suckeeblack || suckeeArrow) continue;
            // F = (G * m1 * m2) / (r^2) powiedzmy ze g = m1, a m2 jest stala
            Vec2 dystans = Vec2(pos->x - suckeePos->x, pos->y - suckeePos->y);
            if (dystans.x * dystans.x + dystans.y * dystans.y > 300*300) continue;
            if (dystans.x != 0)
                suckeeVel->x += ((dystans.x / std::abs(dystans.x)) * d * black->g * 0.01)/dystans.x * dystans.x;
            if (dystans.y != 0)
                suckeeVel->y += ((dystans.y / std::abs(dystans.y)) * d * black->g * 0.01)/dystans.y * dystans.y;
        }
    }
}

int getTotalEnemiesForWave(int wave) {
    switch(wave) {
        case 1:
            return 5;

        case 2:
            return 8;

        case 3:
            return 3;

        case 4:
            return 5;

        default:
            return 0;
    }
}

void checkWaveCompletion(WaveManager* waveMgr) {
    int aliveEnemies = 0;
    
    for (Entity* ent : entities->get()) {
        auto hitbox = ent->get_component<Hitbox>();
        if (hitbox && (hitbox->layer & HitboxLayer::Enemies)) {
            auto hp = ent->get_component<Hp>();
            if (hp && hp->hp > 0) {
                aliveEnemies++;
            }
        }
    }

    if (waveMgr->enemiesInWave == 0 && aliveEnemies == 0) {
        waveMgr->waveActive = false;
        waveMgr->waveTimer = 1.0f;
        waveMgr->enemiesKilled = 0;
    }
}

void startNextWave(WaveManager* waveMgr) {
    static int lastWave = 0;
    if (lastWave != waveMgr->currentWave) {
        lastWave = waveMgr->currentWave;
    }
    waveMgr->currentWave++;

    switch(waveMgr->currentWave) {
        case 1:
            waveMgr->enemiesInWave = 8;
            break;

        case 2:
            waveMgr->enemiesInWave = 16;
            break;

        case 3:
            waveMgr->enemiesInWave = 18;
            break;
    }
    waveMgr->enemiesKilled = 0;
    waveMgr->waveActive = true;
    waveMgr->spawnTimer = 1.0f;
}

void updateWaveManager(float d) {
    int i;
    for (i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto waveMgr = ent->get_component<WaveManager>();
        if(!waveMgr) continue;

        // Jesli fala jest aktywna
        if(waveMgr->waveActive) {
            waveMgr->spawnTimer -= d;

            // Spawn nowego przeciwnika
            if (waveMgr->spawnTimer <= 0 && waveMgr->enemiesInWave > 0) {
                spawnEnemyInWave(waveMgr->currentWave);
                waveMgr->enemiesInWave--;
                waveMgr->spawnTimer = 0.3f;
            }

            checkWaveCompletion(waveMgr);
        }
        else {
            // Oczekiwanie na nastepna fale
            waveMgr->waveTimer -= d;
            if (waveMgr->waveTimer <= 0) {
                startNextWave(waveMgr);
            }
        }
    }
}

void spawnEnemyInWave(int waveNumber) {
    auto wb = resources->get_component<WorldBorder>();
    if (!wb) return;
    
    static int formationIndex = 0;
    Position spawnPos;
    
    switch(waveNumber) {
        case 1:
            spawnPos.x = (float)GetRandomValue(100, (int)wb->width - 100);
            spawnPos.y = (float)GetRandomValue(100, 300);
            spawnBasicEnemy(spawnPos);
            formationIndex = (formationIndex + 1) % 8;
            break;
            
        case 2:
            if (formationIndex < 10) {
                spawnPos.x = 100 + (formationIndex * 80);
                if (formationIndex % 2 == 0) {
                spawnPos.y = 170;
                } else {
                spawnPos.y = 270;
                }
                spawnTankEnemy(spawnPos);
            } 
            else if (formationIndex < 16) {
                int shootingIndex = formationIndex - 10;
                spawnPos.x = 150 + (shootingIndex * 120);
                spawnPos.y = 70;
                spawnShootingEnemy(spawnPos);
            }
            formationIndex = (formationIndex + 1) % 16;
            break;
            
        case 3:
            if (formationIndex < 10) {
                spawnPos.x = (float)GetRandomValue(100, (int)wb->width - 100);
                spawnPos.y = (float)GetRandomValue(100, 300);
                spawnBasicEnemy(spawnPos);
            }
            else if (formationIndex < 18){
                int shootingIndex = formationIndex - 10;
                spawnPos.x = 80+ (shootingIndex * 120);
                spawnPos.y = 50;
                spawnShootingEnemy(spawnPos);
            }
            formationIndex = (formationIndex + 1) % 18;
            break;
    }
}

void spawnFormationWave(int formationType, int count) {
    auto wb = resources->get_component<WorldBorder>();
    if (!wb) return;
    
    float startX = 100;
    float spacingX = (wb->width - 200) / (count + 1);
    
    for (int i = 0; i < count; i++) {
        Position pos;
        
        switch(formationType) {
            case 1:
                pos.x = startX + (spacingX * (i + 1));
                pos.y = -100 - (i * 20);
                spawnBasicEnemy(pos);
                break;
                
            case 2:
                pos.x = startX + (spacingX * (i + 1));
                if (i % 2 == 0) {
                    pos.y = -100;
                } else {
                    pos.y = -150;
                }
                spawnTankEnemy(pos);
                break;
                
            case 3:
                int row = i / 3;
                int col = i % 3;
                pos.x = (wb->width / 2) + (col - 1) * 150;
                pos.y = -100 - (row * 100);
                spawnShootingEnemy(pos);
                break;
        }
    }
}

void spawnDropItem(Position pos, int type) {
    Entity* drop = new Entity();
    drop->add_component<Position>(pos);
    drop->add_component<DropItem>({.type = type, .value = (type == 3) ? 1 : 5}); // 5 ammo lub 1 hp
    
    Texture2D tempTexture;
    switch(type) {
        case 1:
            tempTexture = LoadTexture("surowka.png");
            break;

        case 2:
            tempTexture = LoadTexture("surowka-black.png");
            break;

        case 3:
            tempTexture = LoadTexture("hp_drop.png");
            break;

        default:
            tempTexture = LoadTexture("surowka.png");
    }
    
    drop->add_component<Render>({.txt = tempTexture});
    
    drop->add_component<Hitbox>({
        .layer = HitboxLayer::Effects,
        .interactsWith = HitboxLayer::Players,
        .collisionBox = Area(Vec2(-15, -15), Vec2(15, 15)),
        .receives = {create_component<Destroy>({})},
        .applies = {create_component<CollectDrop>({.dropType = type})}
    });
    
    drop->add_component<Velocity>({.x = 0, .y = 50});
    
    drop->add_component<DestroyBeyondWorld>({});
    
    entities->attach(drop);
}

void updateDropItems(float d) {}

void collectDropItems() {
    for (Entity* ent : entities->get()) {
        auto collects = ent->get_components<CollectDrop>();
        if (collects.empty()) continue;
        
        for (auto collect : collects) {
            // Znajdź gracza
            Entity* player = nullptr;
            for (Entity* e : entities->get()) {
                if (e->get_component<ArrowMovement>()) {
                    player = e;
                    break;
                }
            }
            
            if (!player) {
                ent->remove_component<CollectDrop>();
                continue;
            }
            
            auto drop = ent->get_component<DropItem>();
            if (!drop) {
                drop = new DropItem();
                drop->type = collect->dropType;
                drop->value = (collect->dropType == 3) ? 1 : 5;
            }
            
            switch(drop->type) {
                case 1:
                {
                    auto ammoComp = resources->get_component<AmmoCounter>();
                    if (ammoComp) {
                        ammoComp->currentAmmo[1] += drop->value;
                        if (ammoComp->currentAmmo[1] > 99) ammoComp->currentAmmo[1] = 99;
                    }
                    break;
                }

                case 2:
                {
                    auto ammoComp = resources->get_component<AmmoCounter>();
                    if (ammoComp) {
                        ammoComp->currentAmmo[2] += drop->value;
                        if (ammoComp->currentAmmo[2] > 99) ammoComp->currentAmmo[2] = 99;
                    }
                    break;
                }

                case 3:
                {
                    auto playerHp = player->get_component<Hp>();
                    if (playerHp) {
                        if (playerHp->hp < 5) {
                            playerHp->hp += drop->value;
                            if (playerHp->hp > 5) playerHp->hp = 5;
                    }
                }
                break;
                }
            }
                  
            ent->add_component<Destroy>({});
        }
        
        ent->remove_component<CollectDrop>();
    }
}