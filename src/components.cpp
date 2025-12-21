#include "components.h"
#include "ecs.h"
#include "resources.h"
#include <cmath>

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

void shoot(int tab, int *ammoPointer, Sound shootingsfx) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto shootComp = ent->get_component<Shooting>();
        auto vel = ent->get_component<Velocity>();
        if (pos && shootComp) {
            if (shootComp->cooldown > 0) shootComp->cooldown -= GetFrameTime();
            if (IsKeyDown (KEY_SPACE) && shootComp->cooldown <= 0 &&tab == 1) {
                // Create a new bullet entity
                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = LoadTexture("surowka.png")});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet->add_component<Gravity>({.g = -700.0});
                bullet->add_component<DestroyBeyondWorld>({});
              //  cout << "Shooting!\n";
                PlaySound(shootingsfx);
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (KEY_SPACE) && ammoPointer[2] >= 3 && shootComp->cooldown <= 0 && tab == 3) {
                // Create a first bullet entity
                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = LoadTexture("surowka.png")});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet->add_component<Gravity>({.g = -500.0});
                bullet->add_component<DestroyBeyondWorld>({});
              
                // Create a second bullet entity
                Entity* bullet2 = entities->new_entity();
                bullet2->add_component<Render>({.txt = LoadTexture("surowka.png")});
                bullet2->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet2->add_component<Velocity>({.x = 150});
                bullet2->add_component<Gravity>({.g = -500.0});
                bullet2->add_component<DestroyBeyondWorld>({});
                
                // Create a third bullet entity
                Entity* bullet3 = entities->new_entity();
                bullet3->add_component<Render>({.txt = LoadTexture("surowka.png")});
                bullet3->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet3->add_component<Velocity>({.x = -150});
                bullet3->add_component<Gravity>({.g = -500.0});
                bullet3->add_component<DestroyBeyondWorld>({});
                
                if (ammoPointer[2] >= 3) ammoPointer[2] -= 3;
                shootComp->cooldown = 0.25; // half a second cooldown

            }
            else if (IsKeyDown (KEY_SPACE) && ammoPointer[1] >= 2 && shootComp->cooldown <= 0 && tab == 2) {
                // Create a first bullet entity
                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = LoadTexture("surowka.png")});
                bullet->add_component<Velocity>({.x = -100});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y-45});
                bullet->add_component<Gravity>({.g = -600.0});
                bullet->add_component<DestroyBeyondWorld>({});
              
                // Create a second bullet entity
                Entity* bullet2 = entities->new_entity();
                bullet2->add_component<Render>({.txt = LoadTexture("surowka.png")});
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
