#include "components.h"
#include "ecs.h"
#include "raylib.h"
#include "resources.h"
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
            pos->x += vel->x * d;
            //pos->y += vel->y * d;
        }
    }
}

void renderThings(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto ren = ent->get_component<Render>();
        auto pos = ent->get_component<Position>();
        if (ren && pos) {
            DrawTexture(ren->txt, pos->x - ren->txt.width/2, pos->y - ren->txt.height/2, WHITE);
        }
    }
}

Entity* basicBullet(Position pos) {
    auto res = resources->get_component<soundTextureResources>();   
    Area hitboxSize = Area(
            Vec2(res->bull.width, res->bull.height) * -0.5,
            Vec2(res->bull.width, res->bull.height) * 0.5
            );
    Entity* bullet = new Entity();
    bullet->add_component<Render>({.txt = res->bull});
    bullet->add_component<Position>({.x = pos.x, .y = pos.y-45});
    bullet->add_component<Gravity>({.g = -700.0});
    bullet->add_component<DestroyBeyondWorld>({});
    bullet->add_component<Hitbox>({
            .layer = HitboxLayer::Bullets, 
            .interactsWith = HitboxLayer::Enemies, 
            .collisionBox = hitboxSize,
            .receives = {create_component<Destroy>({})},
            .applies = {create_component<Damage>({.dmg = 1})}
            });
    return bullet;
}

Entity* sniperBullet(Position pos) {
    Entity* bullet = basicBullet(pos);
    bullet->get_component<Gravity>()->g *= 2;
    Hitbox* hitbox = bullet->get_component<Hitbox>();
    hitbox->applies = {create_component<Damage>({.dmg = 3})};
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
    Hitbox* hitbox = bullet->get_component<Hitbox>();
    Entity* bullet2 = shatteredBullet(pos, hitbox);
    bullet2->add_component<Velocity>({.x = 500});
    Entity* bullet3 = shatteredBullet(pos, hitbox);
    Entity* bullet4 = shatteredBullet(pos, hitbox);
    bullet4->add_component<Velocity>({.x = -500});

    hitbox->receives.push_back(create_component<Spawn>({.comps = {
                bullet2,
                bullet3,
                bullet4,
                }}));
    return bullet;
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

                entities->attach(basicBullet(*pos));
              
                // Create a second bullet entity
                Entity* bullet2 = basicBullet(*pos);
                bullet2->add_component<Velocity>({.x = 150});
                entities->attach(bullet2);
                
                // Create a third bullet entity
                Entity* bullet3 = basicBullet(*pos);
                bullet3->add_component<Velocity>({.x = -150});
                entities->attach(bullet3);
                
                if (ammoComp->currentAmmo[2] > 0) ammoComp->currentAmmo[2] -= 1;
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (keyb->shoot) && ammoComp->currentAmmo[1] > 0 && shootComp->cooldown <= 0 && tab == 2) {
                // Create a first bullet entity
                PlaySound(res->shootingsfx);
                
                Entity* bullet = basicBullet(*pos);
                bullet->add_component<Velocity>({.x = -100});
                entities->attach(bullet);
              
                // Create a second bullet entity
                Entity* bullet2 = basicBullet(*pos);
                bullet2->add_component<Velocity>({.x = 100});
                entities->attach(bullet2);
             
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
        if (!dmg || !hp) continue;
        hp->hp -= dmg->dmg;
        ent->remove_component<Damage>();
    }
}

void die() {
    for (Entity* ent : entities->get()) {
        auto hp = ent->get_component<Hp>();
        if (hp && hp->hp <= 0) entities->kill_entity(ent);
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
        auto delay = ent->get_component<Delay>();
        if (!delay) continue;
        if ((GetTime() - delay->timestamp) > delay->delay) {
            for (ComponentHandle comp : delay->comps) {
                ent->add_component(comp);
            }
            ent->remove_component<Delay>();
        }
    }
}
