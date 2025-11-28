#include "components.h"
#include <iostream>
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

void renderThings(float d) {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto ren = ent->get_component<Render>();
        auto pos = ent->get_component<Position>();
        if (ren && pos) {
            DrawTexture(ren->txt, pos->x, pos->y, WHITE);
        }
    }
}

void shoot() {
    for (int i = 0; i < entities->get().size(); i++) {
        auto ent = entities->get()[i];
        auto pos = ent->get_component<Position>();
        auto shootComp = ent->get_component<Shooting>();
        if (pos && shootComp) {
            if (shootComp->cooldown > 0) shootComp->cooldown -= GetFrameTime();
            if (IsKeyDown (KEY_SPACE) && shootComp->cooldown <= 0 && shootComp->cooldown <= 0) {
                // Create a new bullet entity
                Entity* bullet = entities->new_entity();
                bullet->add_component<Render>({.txt = LoadTexture("wabbit_alpha.png")});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y});
                bullet->add_component<Gravity>({.g = -250.0});
              //  cout << "Shooting!\n";
                shootComp->cooldown = 0.5; // half a second cooldown

            }
        }
    }
}

