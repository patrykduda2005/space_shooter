#include "components.h"
#include "ecs.h"
#include "raylib.h"
#include "resources.h"
#include <cmath>
#include <string>

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

void shoot(int tab, int *ammoPointer, Sound shootingsfx, Texture2D bulletTexture) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto shootComp = ent->get_component<Shooting>();
        auto vel = ent->get_component<Velocity>();
        if (pos && shootComp) {
            if (shootComp->cooldown > 0) shootComp->cooldown -= GetFrameTime();
            if (IsKeyDown (KEY_SPACE) && shootComp->cooldown <= 0 &&tab == 1) {
                // Create a new bullet entity
                PlaySound(shootingsfx);

                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = bulletTexture});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet->add_component<Gravity>({.g = -700.0});
                bullet->add_component<DestroyBeyondWorld>({});
                bullet->add_component<Hitbox>({
                        .layer = HitboxLayer::Bullets, 
                        .interactsWith = HitboxLayer::Enemies, 
                        .collisionBox = Area(Vec2(0,0), Vec2(100,200)), 
                        .applies = {create_component<Gravity>({.g = 100})}
                        });
              //  cout << "Shooting!\n";
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (KEY_SPACE) && ammoPointer[2] >= 3 && shootComp->cooldown <= 0 && tab == 3) {
                // Create a first bullet entity
                PlaySound(shootingsfx);

                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = bulletTexture});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet->add_component<Gravity>({.g = -500.0});
                bullet->add_component<DestroyBeyondWorld>({});
              
                // Create a second bullet entity
                Entity* bullet2 = entities->new_entity();
                bullet2->add_component<Render>({.txt = bulletTexture});
                bullet2->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet2->add_component<Velocity>({.x = 150});
                bullet2->add_component<Gravity>({.g = -500.0});
                bullet2->add_component<DestroyBeyondWorld>({});
                
                // Create a third bullet entity
                Entity* bullet3 = entities->new_entity();
                bullet3->add_component<Render>({.txt = bulletTexture});
                bullet3->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet3->add_component<Velocity>({.x = -150});
                bullet3->add_component<Gravity>({.g = -500.0});
                bullet3->add_component<DestroyBeyondWorld>({});
                
                if (ammoPointer[2] >= 3) ammoPointer[2] -= 3;
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (KEY_SPACE) && ammoPointer[1] >= 2 && shootComp->cooldown <= 0 && tab == 2) {
                // Create a first bullet entity
                PlaySound(shootingsfx);
                
                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = bulletTexture});
                bullet->add_component<Velocity>({.x = -100});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet->add_component<Gravity>({.g = -600.0});
                bullet->add_component<DestroyBeyondWorld>({});
              
                // Create a second bullet entity
                Entity* bullet2 = entities->new_entity();
                bullet2->add_component<Render>({.txt = bulletTexture});
                bullet2->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet2->add_component<Velocity>({.x = 100});
                bullet2->add_component<Gravity>({.g = -600.0});
                bullet2->add_component<DestroyBeyondWorld>({});
             
                if(ammoPointer[1] >= 2) ammoPointer[1] -= 2;
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
        if (arr && pos) {
            if (IsKeyDown(KEY_RIGHT)) pos->x += arr->eastSpeed * d;
            if (IsKeyDown(KEY_LEFT)) pos->x -= arr->westSpeed * d;
            if (IsKeyDown(KEY_UP)) pos->y -= arr->northSpeed * d;
            if (IsKeyDown(KEY_DOWN)) pos->y += arr->southSpeed * d; 
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
void ammoCounter(int type, int *ammoPointer){
    if(type == 1){
			DrawText("o", 875,675,35,BLACK);
			DrawText("o", 885,675,35,BLACK);
			DrawText("1x", 920,675,35,BLACK);
			DrawText(std::to_string(ammoPointer[1]).c_str(), 875,705,25,BLACK);
			DrawText(std::to_string(ammoPointer[2]).c_str(), 875,730,25,BLACK);
		}
		else if(type == 2){ 
			DrawText("o", 875,675,25,BLACK);
			DrawText("o", 885,675,25,BLACK);
			DrawText(std::to_string(ammoPointer[1]).c_str(), 875,700,35,BLACK);
			DrawText("2x", 920,700,35,BLACK);
			DrawText(std::to_string(ammoPointer[2]).c_str(), 875,730,25,BLACK);
		}
		else if(type == 3){ 
			DrawText("o", 875,675,25,BLACK);
			DrawText("o", 885,675,25,BLACK);
			DrawText(std::to_string(ammoPointer[1]).c_str(), 875,700,25,BLACK);
			DrawText(std::to_string(ammoPointer[2]).c_str(), 875,730,35,BLACK);
			DrawText("3x", 920,730,35,BLACK);
		}

}
