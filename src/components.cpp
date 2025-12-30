#include "components.h"
#include "ecs.h"
#include "raylib.h"
#include "resources.h"
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
                bullet->add_component<Render>({.txt = LoadTexture("surowka.png")});
                bullet->add_component<Position>({.x = pos->x, .y = pos->y});
                bullet->add_component<Gravity>({.g = -500.0});
                bullet->add_component<DestroyBeyondWorld>({});
                bullet->add_component<Hitbox>({.layer = HitboxLayer::Bullets, .interactsWith = HitboxLayer::Enemies, .collisionBox = Area(Vec2(0,0), Vec2(100,200))});
              //  cout << "Shooting!\n";
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
            if (pos->y > wb->y + wb->height) pos->y = wb->x + wb->height;
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

void dispatchColision(int events) {
}

void detectCollision() {
    for (Entity *ent : entities->get()) {
        auto col = ent->get_component<Hitbox>();
        auto pos = ent->get_component<Position>();
        if (!pos || !col) continue;
        for (Entity *ent_2 : entities->get()) {
            auto ent_2_col = ent_2->get_component<Hitbox>();
            auto ent_2_pos = ent_2->get_component<Position>();
            if (!ent_2_col || !ent_2_pos) continue;
            if (ent_2 == ent) continue;

            // Jako ze collisionBox jest relatywny do swojego entity to zamieniam na kordynaty absolutne
            Area global_col = col->collisionBox + Vec2(pos->x, pos->y);
            Area global_col_2 = ent_2_col->collisionBox + Vec2(ent_2_pos->x, ent_2_pos->y);
            if ((ent_2_col->layer & col->interactsWith) == 0) continue; // Jak jest na innym layerze to sie nie kolliduje
            if (!global_col.overlaps(global_col_2)) continue; // Jak nie koliduje to nie koliduje
            std::cout << "Elo kolizja" << "\n";
        }
    }
}

void outlineColliders() {
    for (Entity *ent : entities->get()) {
        auto col = ent->get_component<Hitbox>();
        auto pos = ent->get_component<Position>();
        if (!pos || !col) continue;
        Area global_col = col->collisionBox + Vec2(pos->x, pos->y);
        DrawRectangleLines(global_col.left_up_corner.x, global_col.left_up_corner.y, global_col.getWidth(), global_col.getHeight(), YELLOW);
    }
}
