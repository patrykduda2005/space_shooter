#include "components.h"
#include "ecs.h"
#include "raylib.h"
#include "resources.h"
#include "customentities.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;


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
                entities->attach(basicBullet(*pos));
                //std::cout << "Shooting!\n";
                shootComp->cooldown = 0.25; // half a second cooldown
            }
            else if (IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[0] > 0 && shootComp->cooldown <= 0 && tab == 2) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);

                //entities->attach(blackBullet(*pos));
#if 1
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
                
                if (ammoComp->currentAmmo[0] > 0) ammoComp->currentAmmo[0] -= 1;
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[1] > 0 && shootComp->cooldown <= 0 && tab == 3) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);

                entities->attach(sniperBullet(*pos));

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
            else if(IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[2] > 0 && shootComp->cooldown <= 0 && tab == 4) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);

                entities->attach(blackBullet(*pos));

                if (ammoComp->currentAmmo[2] > 0) ammoComp->currentAmmo[2] -= 1;
                shootComp->cooldown = 0.25; // half a second cooldown
            }
            else if(IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[3] > 0 && shootComp->cooldown <= 0 && tab == 5) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);

                entities->attach(shatterBullet(*pos));

                if (ammoComp->currentAmmo[3] > 0) ammoComp->currentAmmo[3] -= 1;
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

void outlineColliders(bool Datalog) {
    if (!Datalog) return;
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
    auto res = resources->get_component<soundTextureResources>();
    
    if(type == 1){
			DrawText("o", 875,645,35,WHITE);
			DrawText("o", 885,645,35,WHITE);
			//DrawText("1", 920,655,35,WHITE);
            DrawTexture(res->bull, 920, 645, WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[0]).c_str(), 875,675,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,705,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,735,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[3]).c_str(), 875,765,25,WHITE);
		}
		else if(type == 2){ 
			DrawText("o", 875,645,25,WHITE);
			DrawText("o", 885,645,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[0]).c_str(), 875,675,35,WHITE);
			//DrawText("2", 920,685,35,WHITE);
            DrawTexture(res->bull, 920, 675, WHITE);
            DrawTexture(res->bull, 930, 680, WHITE);
            DrawTexture(res->bull, 940, 685, WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,705,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,735,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[3]).c_str(), 875,765,25,WHITE);
		}
		else if(type == 3){ 
			DrawText("o", 875,645,25,WHITE);
			DrawText("o", 885,645,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[0]).c_str(), 875,675,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,705,35,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,735,25,WHITE);
			//DrawText("3", 920,730,35,WHITE);
            DrawTexture(res->sniper_bullet, 920, 705, WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[3]).c_str(), 875,765,25,WHITE);
		}
        else if(type == 4){ 
			DrawText("o", 875,645,25,WHITE);
			DrawText("o", 885,645,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[0]).c_str(), 875,675,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,705,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,735,35,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[3]).c_str(), 875,765,25,WHITE);
			//DrawText("4", 920,775,35,WHITE);
            DrawTexture(res->blackhole, 920, 735, WHITE);
		}
        else if(type == 5){ 
			DrawText("o", 875,645,25,WHITE);
			DrawText("o", 885,645,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[0]).c_str(), 875,675,25,WHITE);
			DrawText(std::to_string(ammoComp->currentAmmo[1]).c_str(), 875,705,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[2]).c_str(), 875,735,25,WHITE);
            DrawText(std::to_string(ammoComp->currentAmmo[3]).c_str(), 875,765,35,WHITE);
			//DrawText("5", 920,775,35,WHITE);
            DrawTexture(res->shatter, 920, 765, WHITE);
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
                auto cash = resources->get_component<Money>();
                if(cash)cash->money += 10;
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

void savingSettings(){
	ofstream ustawienia("settings.txt");
	auto settingsComp = resources->get_component<SettingsComponent>();
	auto keyb = resources->get_component<KeyBinds>();

	//ustawienia.open();

	if(ustawienia.is_open()){
		ustawienia <<"FPS_INDEX "<< (int)settingsComp->fps_index << endl;
		ustawienia <<"VOLUME "<< settingsComp->volume << endl;
		ustawienia <<"SFX_VOLUME "<< settingsComp->sfx_volume << endl;
		ustawienia <<"KEY_UP "<< (int)keyb->up << endl;
		ustawienia <<"KEY_DOWN "<< (int)keyb->down << endl;
		ustawienia <<"KEY_LEFT "<< (int)keyb->left << endl;
		ustawienia <<"KEY_RIGHT "<< (int)keyb->right << endl;
		ustawienia <<"KEY_SHOOT "<< (int)keyb->shoot << endl;
		ustawienia <<"KEY_TYPE_SHOOT1 "<< (int)keyb->type_shoot1 << endl;
		ustawienia <<"KEY_TYPE_SHOOT2 "<< (int)keyb->type_shoot2 << endl;
		ustawienia <<"KEY_TYPE_SHOOT3 "<< (int)keyb->type_shoot3 << endl;
        ustawienia <<"KEY_TYPE_SHOOT4 "<< (int)keyb->type_shoot4 << endl;
        ustawienia <<"KEY_TYPE_SHOOT5 "<< (int)keyb->type_shoot5 << endl;
		ustawienia.close();

		cout<<"Settings saved"<<endl;
	}
	else cout<<"Unable to open file"<<endl;
	

}

void settingsSystem(bool* Datalog, bool* Pause, bool* Menu, bool* Sett, Vector2 mousePosition, bool* KeybindsBtt) {
    auto settingsComp = resources->get_component<SettingsComponent>();
    auto worldBorder = resources->get_component<WorldBorder>();
    auto keyb = resources->get_component<KeyBinds>();
	auto musicRes = resources->get_component<MusicResources>();
	auto res = resources->get_component<soundTextureResources>();

    int x_text_position;
    int key_number;
   //  int sfx_volume_int;
   //  int volume_int;
    
    std::string fps[5] = {"30", "60", "120", "144", "Unlimited"};
    

    if(IsMusicStreamPlaying(musicRes->backgroundMusic)){
				StopMusicStream(musicRes->backgroundMusic);
			}

			if(IsMusicStreamPlaying(musicRes->menuMusic)){
				UpdateMusicStream(musicRes->menuMusic);
			} else {
				PlayMusicStream(musicRes->menuMusic);
			}

			x_text_position = worldBorder->width / 2 - MeasureText("Settings", 50)/2;

			if(*Datalog){
				DrawText("x:", 50,700,15,BLACK);
				DrawText(std::to_string(mousePosition.x).c_str(), 80,700,15,BLACK);
				DrawText("y:", 175,700,15,BLACK);	
				DrawText(std::to_string(mousePosition.y).c_str(), 205,700,15,BLACK);
				//DrawLine(395, 0, 395, 800, BLACK);
				//DrawLine(605, 0, 603	, 800, BLACK);
			}
			DrawText("Settings", x_text_position, 28, 50, BLACK);
			
			//Ilość fpsów

			x_text_position = worldBorder->width / 2 - MeasureText("FPS", 30)/2;
			DrawText("FPS", x_text_position, 100, 30, BLACK);

			x_text_position = worldBorder->width / 2 - MeasureText(fps[settingsComp->fps_index].c_str(), 30)/2;
			DrawText(fps[settingsComp->fps_index].c_str(), x_text_position, 130, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 130, 30, 30})){
				DrawText("<", x_text_position - 100, 130, 30, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->fps_index > 0){
					settingsComp->fps_index--;
					SetTargetFPS(std::stoi(fps[settingsComp->fps_index]));
				}
			} else {
				DrawText("<", x_text_position - 100, 130, 30, BLACK);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(fps[settingsComp->fps_index].c_str(), 30)) + 100, 130, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(fps[settingsComp->fps_index].c_str(), 30)) + 100, 130, 30, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->fps_index < 4){
					settingsComp->fps_index++;
					if(settingsComp->fps_index == 4) SetTargetFPS(0);
					else SetTargetFPS(std::stoi(fps[settingsComp->fps_index]));
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(fps[settingsComp->fps_index].c_str(), 30)) + 100, 130, 30, BLACK);
			}

			x_text_position = worldBorder->width / 2 - MeasureText("Key binds", 30)/2;
			DrawText("Key binds", x_text_position, 180, 30, BLACK);

			DrawText("Up: ", 100, 220, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 220, 400, 30})){
				DrawRectangle(250, 220, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[0]) DrawText(GetKeyText(keyb->up), 300, 225, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[0] = true;
				}
			} else {
				DrawRectangle(250, 220, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->up), 300, 225, 25, BLACK);
			}
			if(KeybindsBtt[0]) {
				DrawText("Press a key...", 410, 225, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->up = (KeyboardKey)key_number;
					KeybindsBtt[0] = false;
				}
			}

			DrawText("Down: ", 100, 260, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 260, 400, 30})){
				DrawRectangle(250, 260, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[1]) DrawText(GetKeyText(keyb->down), 300, 265, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[1] = true;
				}
			} else {
				DrawRectangle(250, 260, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->down), 300, 265, 25, BLACK);
			}
			if(KeybindsBtt[1]) {
				DrawText("Press a key...", 410, 265, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->down = (KeyboardKey)key_number;
					KeybindsBtt[1] = false;
				}
			}
			DrawText("Left: ", 100, 300, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 300, 400, 30})){
				DrawRectangle(250, 300, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[2]) DrawText(GetKeyText(keyb->left), 300, 305, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[2] = true;
				}
			} else {
				DrawRectangle(250, 300, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->left), 300, 305, 25, BLACK);
			}
			if(KeybindsBtt[2]) {
				DrawText("Press a key...", 410, 305, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->left = (KeyboardKey)key_number;
					KeybindsBtt[2] = false;
				}
			}

			DrawText("Right: ", 100, 340, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 340, 400, 30})){
				DrawRectangle(250, 340, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[3]) DrawText(GetKeyText(keyb->right), 300, 345, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[3] = true;
				}
			} else {
				DrawRectangle(250, 340, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->right), 300, 345, 25, BLACK);
			}
			if(KeybindsBtt[3]) {
				DrawText("Press a key...", 410, 345, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->right = (KeyboardKey)key_number;
					KeybindsBtt[3] = false;
				}
			}
			DrawText("Shoot: ", 100, 380, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 380, 400, 30})){
				DrawRectangle(250, 380, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[4]) DrawText(GetKeyText(keyb->shoot), 300, 385, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[4] = true;
				}
			} else {
				DrawRectangle(250, 380, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->shoot), 300, 385, 25, BLACK);
			}
			if(KeybindsBtt[4]) {
				DrawText("Press a key...", 410, 385, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->shoot = (KeyboardKey)key_number;
					KeybindsBtt[4] = false;
				}
			}
			DrawText("Shoot 1: ", 100, 420, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 420, 400, 30})){
				DrawRectangle(250, 420, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[5]) DrawText(GetKeyText(keyb->type_shoot1), 300, 425, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[5] = true;
				}
			} else {
				DrawRectangle(250, 420, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->type_shoot1), 300, 425, 25, BLACK);
			}
			if(KeybindsBtt[5]) {
				DrawText("Press a key...", 410, 425, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->type_shoot1 = (KeyboardKey)key_number;
					KeybindsBtt[5] = false;
				}
			}
			DrawText("Shoot 2: ", 100, 460, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 460, 400, 30})){
				DrawRectangle(250, 460, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[6]) DrawText(GetKeyText(keyb->type_shoot2), 300, 465, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[6] = true;
				}
			} else {
				DrawRectangle(250, 460, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->type_shoot2), 300, 465, 25, BLACK);
			}
			if(KeybindsBtt[6]) {
				DrawText("Press a key...", 410, 465, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->type_shoot2 = (KeyboardKey)key_number;
					KeybindsBtt[6] = false;
				}
			}

			DrawText("Shoot 3: ", 100, 500, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 500, 400, 30})){
				DrawRectangle(250, 500, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[7]) DrawText(GetKeyText(keyb->type_shoot3), 300, 505, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[7] = true;
				}
			} else {
				DrawRectangle(250, 500, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->type_shoot3), 300, 505, 25, BLACK);
			}
			if(KeybindsBtt[7]) {
				DrawText("Press a key...", 410, 505, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->type_shoot3 = (KeyboardKey)key_number;
					KeybindsBtt[7] = false;
				}
			}

            DrawText("Shoot 4: ", 100, 540, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 540, 400, 30})){
				DrawRectangle(250, 540, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[8]) DrawText(GetKeyText(keyb->type_shoot4), 300, 545, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[8] = true;
				}
			} else {
				DrawRectangle(250, 540, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->type_shoot4), 300, 545, 25, BLACK);
			}
			if(KeybindsBtt[8]) {
				DrawText("Press a key...", 410, 545, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->type_shoot4 = (KeyboardKey)key_number;
					KeybindsBtt[8] = false;
				}
			}
			
            DrawText("Shoot 5: ", 100, 580, 25, BLACK);
			if(CheckCollisionPointRec(mousePosition, {250, 580, 400, 30})){
				DrawRectangle(250, 580, 400, 30, LIGHTGRAY);
				if(!KeybindsBtt[9]) DrawText(GetKeyText(keyb->type_shoot5), 300, 585, 25, BLACK);

				if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
					KeybindsBtt[9] = true;
				}
			} else {
				DrawRectangle(250, 580, 400, 30, GRAY);
				DrawText(GetKeyText(keyb->type_shoot5), 300, 585, 25, BLACK);
			}
			if(KeybindsBtt[9]) {
				DrawText("Press a key...", 410, 585, 25, BLACK);
				key_number = GetKeyPressed();
				if(key_number != 0){
					keyb->type_shoot5 = (KeyboardKey)key_number;
					KeybindsBtt[9] = false;
				}
			}
			
			//Głośność muzyki i dźwięków
			x_text_position = worldBorder->width / 2 - MeasureText("Volume", 30)/2;
			DrawText("Volume", x_text_position, 630, 30, BLACK);
			DrawText("Music:", 100, 670, 25, BLACK);
			//volume_int = (int)(settingsComp->volume * 10);

			x_text_position += MeasureText("Volume", 30)/2;
			DrawText(std::to_string(settingsComp->volume).c_str(), x_text_position, 670, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 670, 30, 30})){
				DrawText("<", x_text_position - 100, 670, 30, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->volume > 0){
					settingsComp->volume -= 1;
					//settingsComp->volume -= 0.1f;
					SetMusicVolume(musicRes->backgroundMusic, (float)settingsComp->volume/10);
					SetMusicVolume(musicRes->menuMusic, (float)settingsComp->volume/10);
				}
			} else {
				DrawText("<", x_text_position - 100, 670, 30, BLACK);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(std::to_string(settingsComp->volume).c_str(), 30)) + 100, 670, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->volume).c_str(), 30)) + 100, 670, 30, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->volume < 10){
					settingsComp->volume += 1;
					SetMusicVolume(musicRes->backgroundMusic, (float)settingsComp->volume/10);
					SetMusicVolume(musicRes->menuMusic, (float)settingsComp->volume/10);
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->volume).c_str(), 30)) + 100, 670, 30, BLACK);
			}

			DrawText("Sounds:", 100, 710, 25, BLACK);
			//sfx_volume_int = (int)(settingsComp->sfx_volume * 10);

			DrawText(std::to_string(settingsComp->sfx_volume).c_str(), x_text_position, 710, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 710, 30, 30})){
				DrawText("<", x_text_position - 100, 710, 30, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->sfx_volume > 0){
					settingsComp->sfx_volume -= 1;
					SetSoundVolume(res->shootingsfx, (float)settingsComp->sfx_volume/10);
				}
			} else {
				DrawText("<", x_text_position - 100, 710, 30, BLACK);
			}

			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(std::to_string(settingsComp->sfx_volume).c_str(), 30)) + 100, 710, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->sfx_volume).c_str(), 30)) + 100, 710, 30, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->sfx_volume < 10){
					settingsComp->sfx_volume += 1;
					SetSoundVolume(res->shootingsfx, (float)settingsComp->sfx_volume/10);
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->sfx_volume).c_str(), 30)) + 100, 710, 30, BLACK);
			}

			//Wyjście z ustawień
			x_text_position = worldBorder->width / 8 - MeasureText("Back", 25)/2;

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 28, 100, 30})){
				DrawText("Back", x_text_position, 28, 25, GRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
						  *Sett = false;
                }
		    } else {
			    DrawText("Back", x_text_position, 28, 25, BLACK);
		    }
}

void menuSystem(bool* Datalog, bool* Menu, bool* Sett, bool* Pause, bool *Shop, Vector2 mousePosition, int* exit_int) {
   auto worldBorder = resources->get_component<WorldBorder>();
	auto musicRes = resources->get_component<MusicResources>();
	auto res = resources->get_component<soundTextureResources>();

	int x_text_position;


	if(Datalog){
		DrawText("x:", 50,700,15,WHITE);
		DrawText(std::to_string(mousePosition.x).c_str(), 80,700,15,BLACK);
		DrawText("y:", 175,700,15,WHITE);	
		DrawText(std::to_string(mousePosition.y).c_str(), 205,700,15,BLACK);
	}


   x_text_position = worldBorder->width / 2 - MeasureText("Rise of the Wielki Piec", 50)/2;
	DrawText("Rise of the Wielki Piec", x_text_position,100,50,BLACK);

	if(IsMusicStreamPlaying(musicRes->menuMusic)){
		UpdateMusicStream(musicRes->menuMusic);
	} else {
		PlayMusicStream(musicRes->menuMusic);
	}

	x_text_position = worldBorder->width / 2 - MeasureText("Start", 50)/2;
	if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 210, 200, 45})){
		DrawText("Start", x_text_position, 210, 50, GRAY);
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			 *Menu = false; 
			 *Pause = false;
		}
	} else {
		DrawText("Start", x_text_position, 210, 50, BLACK);
	}

	x_text_position = worldBorder->width / 2 - MeasureText("Shop", 50)/2;
	if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 310, (float)MeasureText("Shop", 50), 45})){
	   DrawText("Shop", x_text_position, 310, 50, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
		   *Shop = true;
      }
	} else {
		DrawText("Shop", x_text_position, 310, 50, BLACK);
	}

	x_text_position = worldBorder->width / 2 - MeasureText("Settings", 50)/2;
	if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 410, (float)MeasureText("Settings", 50), 45})){
	   DrawText("Settings", x_text_position, 410, 50, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
		   *Sett = true;
      }
	} else {
		DrawText("Settings", x_text_position, 410, 50, BLACK);
	}

	x_text_position = worldBorder->width / 2 - MeasureText("Exit", 50)/2;
	if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 512, 200, 45})){
		DrawText("Exit", x_text_position, 512, 50, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*exit_int = 1; // Exit the game loop
		}
	} else {
		DrawText("Exit", x_text_position, 512, 50, BLACK);
	}
}

void pauseSystem(Vector2 mousePosition, bool* Pause, bool* Sett, bool* Menu, bool* Shop, int* exit_int) {
    auto worldBorder = resources->get_component<WorldBorder>();
    auto musicRes = resources->get_component<MusicResources>();
    Rectangle resumeBtn = {395, 190, 200, 45};	
	Rectangle exitBtn = {395, 400, 200, 45};
	Rectangle settBtn = {395, 240, 200, 45};

    int x_text_position = worldBorder->width / 2 - MeasureText("PAUSE", 50)/2;

   DrawText("PAUSE", x_text_position, 100, 50, BLACK);

	if(IsMusicStreamPlaying(musicRes->backgroundMusic)){
		PauseMusicStream(musicRes->backgroundMusic);
	}

	if(IsMusicStreamPlaying(musicRes->menuMusic)){
		UpdateMusicStream(musicRes->menuMusic);
	} else {
		PlayMusicStream(musicRes->menuMusic);
	}

    x_text_position = worldBorder->width / 2 - MeasureText("Resume", 25)/2;
	if (CheckCollisionPointRec(mousePosition, resumeBtn)) {
		DrawText("Resume", x_text_position, 200, 25, GRAY);
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        *Pause = false; // Wracamy do gry
    }
    } else {
	    DrawText("Resume", x_text_position, 200, 25, BLACK);
    }

    x_text_position = worldBorder->width / 2 - MeasureText("Exit", 25)/2;
	if(CheckCollisionPointRec(mousePosition, exitBtn)){
		DrawText("Exit", x_text_position, 412, 25, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*exit_int = 1; // Exit the game loop
		}
	} else {
		DrawText("Exit", x_text_position, 412, 25, BLACK);
	}

    x_text_position = worldBorder->width / 2 - MeasureText("Settings", 25)/2;
	if(CheckCollisionPointRec(mousePosition, settBtn)){
		DrawText("Settings", x_text_position, 250, 25, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*Sett = true;
		}
		} else {
			DrawText("Settings", x_text_position, 250, 25, BLACK);
		}

		x_text_position = worldBorder->width / 2 - MeasureText("Back to Menu", 25)/2;
    if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 350, 200, 30})){
		DrawText("Back to Menu", x_text_position, 350, 25, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*Menu = true;
         *Pause = false;
		}
	} else {
		DrawText("Back to Menu", x_text_position, 350, 25, BLACK);
   }

	x_text_position = worldBorder->width / 2 - MeasureText("Shop", 25)/2;
	if(CheckCollisionPointRec(mousePosition,{(float)x_text_position,300, (float)MeasureText("Shop", 25),30})){
		DrawText("Shop", x_text_position, 300, 25, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*Shop = true;
		}
	} else {
		DrawText("Shop", x_text_position, 300, 25, BLACK);
	}
}



void shopSystem(bool* Shop, bool* Pause, bool* Menu, bool* Datalog, Vector2 mousePosition){
	auto cash = resources->get_component<Money>();
	auto worldBorder = resources->get_component<WorldBorder>();
	auto res = resources->get_component<soundTextureResources>();
    auto ammoComp = resources->get_component<AmmoCounter>();
    auto musicRes = resources->get_component<MusicResources>();
		
	int x_text_position, y_text_position;
	y_text_position = worldBorder->height - 100;

	if(IsMusicStreamPlaying(musicRes->backgroundMusic)){
		PauseMusicStream(musicRes->backgroundMusic);
	}

	if(IsMusicStreamPlaying(musicRes->menuMusic)){
		UpdateMusicStream(musicRes->menuMusic);
	} else {
		PlayMusicStream(musicRes->menuMusic);
	}

	DrawTexturePro(res->shopTexture, {0.0f, 0.0f, (float)res->shopTexture.width, (float)res->shopTexture.height}, {0.0f, 0.0f, worldBorder->width, worldBorder->height}, {0.0f, 0.0f}, 0.0f, WHITE);

	if(*Datalog){
		DrawText("x:", 50,700,15,WHITE);
		DrawText(std::to_string(mousePosition.x).c_str(), 80,700,15,WHITE);
		DrawText("y:", 175,700,15,WHITE);	
		DrawText(std::to_string(mousePosition.y).c_str(), 205,700,15,WHITE);
	}
	
	x_text_position = 720;

	DrawText("Money: ", x_text_position, 40, 50, WHITE);
	x_text_position += 180;
	DrawText(std::to_string(cash->money).c_str(), x_text_position, 40, 50, WHITE);
		
	DrawTexture(res->bull, 217, 625, WHITE);
    DrawTexture(res->bull, 227, 635, WHITE);
    DrawTexture(res->bull, 237, 630, WHITE);
	
	DrawText("50", 230, 745, 25, BLACK);
	if(CheckCollisionPointRec(mousePosition, {200, 590, 100, 100})){	
		DrawRectangleRounded({200, 590, 100, 100}, 0.2f, 0.5f, Fade(GRAY, 0.2f));
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			if(cash && cash->money >= 50) {
            cash->money -= 50;
            ammoComp->currentAmmo[1] += 10;
            }
		}
	}
    //Texture2D blacktxt = LoadTexture("resources/surowka-black.png");
    DrawTexture(res->sniper_bullet, 400, 625, WHITE);

	DrawText("50", 400, 745, 25, BLACK);
	if(CheckCollisionPointRec(mousePosition, {370, 590, 100, 100})){	
		DrawRectangleRounded({370, 590, 100, 100}, 0.2f, 0.5f, Fade(GRAY, 0.2f));
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			if(cash && cash->money >= 50){ 
                cash->money -= 50;
                ammoComp->currentAmmo[2] += 5;
            }
		}
	}

    //Texture2D bombastic = LoadTexture("resources/surowka-boom.png");
    DrawTexture(res->blackhole, 560, 625, WHITE);

	DrawText("50", 565, 745, 25, BLACK);
	if(CheckCollisionPointRec(mousePosition, {535, 590, 100, 100})){	
		DrawRectangleRounded({535, 590, 100, 100}, 0.2f, 0.5f, Fade(GRAY, 0.2f));
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			if(cash && cash->money >= 50) {
                cash->money -= 50;
                ammoComp->currentAmmo[3] += 1;
            }
		}
	}

    DrawTexture(res->shatter, 720, 625, BLACK);
    DrawText("50", 730, 745, 25, BLACK);
	if(CheckCollisionPointRec(mousePosition, {700, 590, 100, 100})){	
		DrawRectangleRounded({700, 590, 100, 100}, 0.2f, 0.5f, Fade(GRAY, 0.2f));
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			if(cash && cash->money >= 50) {
                cash->money -= 50;
                ammoComp->currentAmmo[3] += 1;
            }
		}
	}

	x_text_position = worldBorder->width / 8 - MeasureText("Back" , 25)/2;

	if(CheckCollisionPointRec(mousePosition,{(float)x_text_position,100, (float)MeasureText("Back",25),30})){
		DrawText("Back", x_text_position, 100, 25, GRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*Shop = false;
		}
	} else DrawText("Back", x_text_position, 100, 25, WHITE);
}
void spawnDropItem(Position pos, int type) {
    Entity* drop = new Entity();
    drop->add_component<Position>(pos);
    drop->add_component<DropItem>({.type = type, .value = (type == 3) ? 1 : 5}); // 5 ammo lub 1 hp

    soundTextureResources* textures = resources->get_component<soundTextureResources>();
    
    Texture2D tempTexture;
    switch(type) {
        case 1:
            tempTexture = textures->bull;
            break;

        case 2:
            tempTexture = textures->blackhole;
            break;

        case 3:
            tempTexture = textures->hp;
            break;

        default:
            tempTexture = textures->bull;
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


void GameOver(){
    auto worldBorder = resources->get_component<WorldBorder>();
    auto res = resources->get_component<soundTextureResources>();

    DrawTexturePro(res->gameover, {0.0f, 0.0f, (float)res->gameover.width, (float)res->gameover.height}, {0.0f, 0.0f, worldBorder->width, worldBorder->height}, {0.0f, 0.0f}, 0.0f, WHITE);

    
}
