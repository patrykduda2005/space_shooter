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
				DrawLine(395, 0, 395, 800, BLACK);
				DrawLine(605, 0, 603	, 800, BLACK);
			}
			DrawText("Settings", x_text_position, 28, 50, RED);
			
			//Ilość fpsów

			x_text_position = worldBorder->width / 2 - MeasureText("FPS", 30)/2;
			DrawText("FPS", x_text_position, 100, 30, RED);

			x_text_position = worldBorder->width / 2 - MeasureText(fps[settingsComp->fps_index].c_str(), 30)/2;
			DrawText(fps[settingsComp->fps_index].c_str(), x_text_position, 130, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 130, 30, 30})){
				DrawText("<", x_text_position - 100, 130, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->fps_index > 0){
					settingsComp->fps_index--;
					SetTargetFPS(std::stoi(fps[settingsComp->fps_index]));
				}
			} else {
				DrawText("<", x_text_position - 100, 130, 30, RED);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(fps[settingsComp->fps_index].c_str(), 30)) + 100, 130, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(fps[settingsComp->fps_index].c_str(), 30)) + 100, 130, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->fps_index < 4){
					settingsComp->fps_index++;
					if(settingsComp->fps_index == 4) SetTargetFPS(0);
					else SetTargetFPS(std::stoi(fps[settingsComp->fps_index]));
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(fps[settingsComp->fps_index].c_str(), 30)) + 100, 130, 30, RED);
			}

			x_text_position = worldBorder->width / 2 - MeasureText("Key binds", 30)/2;
			DrawText("Key binds", x_text_position, 180, 30, RED);

			DrawText("Up: ", 100, 220, 25, RED);
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

			DrawText("Down: ", 100, 260, 25, RED);
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
			DrawText("Left: ", 100, 300, 25, RED);
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

			DrawText("Right: ", 100, 340, 25, RED);
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
			DrawText("Shoot: ", 100, 380, 25, RED);
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
			DrawText("Shoot 1: ", 100, 420, 25, RED);
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
			DrawText("Shoot 2: ", 100, 460, 25, RED);
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

			DrawText("Shoot 3: ", 100, 500, 25, RED);
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
			//Głośność muzyki i dźwięków
			x_text_position = worldBorder->width / 2 - MeasureText("Volume", 30)/2;
			DrawText("Volume", x_text_position, 550, 30, RED);
			DrawText("Music:", 100, 590, 25, RED);
			//volume_int = (int)(settingsComp->volume * 10);

			x_text_position += MeasureText("Volume", 30)/2;
			DrawText(std::to_string(settingsComp->volume).c_str(), x_text_position, 590, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 590, 30, 30})){
				DrawText("<", x_text_position - 100, 590, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->volume > 0){
					settingsComp->volume -= 1;
					//settingsComp->volume -= 0.1f;
					SetMusicVolume(musicRes->backgroundMusic, (float)settingsComp->volume/10);
					SetMusicVolume(musicRes->menuMusic, (float)settingsComp->volume/10);
				}
			} else {
				DrawText("<", x_text_position - 100, 590, 30, RED);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(std::to_string(settingsComp->volume).c_str(), 30)) + 100, 590, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->volume).c_str(), 30)) + 100, 590, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->volume < 10){
					settingsComp->volume += 1;
					SetMusicVolume(musicRes->backgroundMusic, (float)settingsComp->volume/10);
					SetMusicVolume(musicRes->menuMusic, (float)settingsComp->volume/10);
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->volume).c_str(), 30)) + 100, 590, 30, RED);
			}

			DrawText("Sounds:", 100, 630, 25, RED);
			//sfx_volume_int = (int)(settingsComp->sfx_volume * 10);

			DrawText(std::to_string(settingsComp->sfx_volume).c_str(), x_text_position, 630, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 630, 30, 30})){
				DrawText("<", x_text_position - 100, 630, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->sfx_volume > 0){
					settingsComp->sfx_volume -= 1;
					SetSoundVolume(res->shootingsfx, (float)settingsComp->sfx_volume/10);
				}
			} else {
				DrawText("<", x_text_position - 100, 630, 30, RED);
			}

			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(std::to_string(settingsComp->sfx_volume).c_str(), 30)) + 100, 630, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->sfx_volume).c_str(), 30)) + 100, 630, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && settingsComp->sfx_volume < 10){
					settingsComp->sfx_volume += 1;
					SetSoundVolume(res->shootingsfx, (float)settingsComp->sfx_volume/10);
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(std::to_string(settingsComp->sfx_volume).c_str(), 30)) + 100, 630, 30, RED);
			}

			//Wyjście z ustawień
			x_text_position = worldBorder->width / 2 - MeasureText("Back", 25)/2;

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 700, 100, 30})){
				DrawText("Back", x_text_position, 700, 25, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
						  *Sett = false;
                }
		    } else {
			    DrawText("Back", x_text_position, 700, 25, RED);
		    }
}

void menuSystem(bool* Menu, bool* Sett, bool* Pause, Vector2 mousePosition, int* exit_int) {
    auto worldBorder = resources->get_component<WorldBorder>();
	auto musicRes = resources->get_component<MusicResources>();
	auto res = resources->get_component<soundTextureResources>();

    int x_text_position;

    x_text_position = worldBorder->width / 2 - MeasureText("Rise of the Wielki Piec", 50)/2;
	DrawText("Rise of the Wielki Piec", x_text_position,100,50,BLACK);

	if(IsMusicStreamPlaying(musicRes->menuMusic)){
		UpdateMusicStream(musicRes->menuMusic);
	} else {
		PlayMusicStream(musicRes->menuMusic);
	}

	x_text_position = worldBorder->width / 2 - MeasureText("Start", 50)/2;
	if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 210, 200, 45})){
		DrawText("Start", x_text_position, 210, 50, DARKGRAY);
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			 *Menu = false; 
			 *Pause = false;
		}
	} else {
		DrawText("Start", x_text_position, 210, 50, BLACK);
	}

	x_text_position = worldBorder->width / 2 - MeasureText("Settings", 50)/2;
	if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 310, 200, 45})){
	    DrawText("Settings", x_text_position, 310, 50, DARKGRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
		   *Sett = true;
        }
		} else {
			DrawText("Settings", x_text_position, 310, 50, BLACK);
		}

		x_text_position = worldBorder->width / 2 - MeasureText("Exit", 50)/2;
		if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 412, 200, 45})){
		    DrawText("Exit", x_text_position, 412, 50, DARKGRAY);
			if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
				*exit_int = 1; // Exit the game loop
			}
		} else {
			DrawText("Exit", x_text_position, 412, 50, BLACK);
		}
}

void pauseSystem(Vector2 mousePosition, bool* Pause, bool* Sett, bool* Menu, int* exit_int) {
    auto worldBorder = resources->get_component<WorldBorder>();
    auto musicRes = resources->get_component<MusicResources>();
    Rectangle resumeBtn = {395, 190, 200, 45};	
	Rectangle exitBtn = {395, 400, 200, 45};
	Rectangle settBtn = {395, 240, 200, 45};

    int x_text_position = worldBorder->width/2 - MeasureText("PAUSE", 100)/2;

    DrawText("PAUSE", x_text_position, 100, 50, RED);

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
		DrawText("Resume", x_text_position, 200, 25, DARKGRAY);
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        *Pause = false; // Wracamy do gry
    }
    } else {
	    DrawText("Resume", x_text_position, 200, 25, RED);
    }

    x_text_position = worldBorder->width / 2 - MeasureText("Exit", 25)/2;
	if(CheckCollisionPointRec(mousePosition, exitBtn)){
		DrawText("Exit", x_text_position, 412, 25, DARKGRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*exit_int = 1; // Exit the game loop
		}
	} else {
		DrawText("Exit", x_text_position, 412, 25, RED);
	}

    x_text_position = worldBorder->width / 2 - MeasureText("Settings", 25)/2;
	if(CheckCollisionPointRec(mousePosition, settBtn)){
		DrawText("Settings", x_text_position, 250, 25, DARKGRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*Sett = true;
		}
		} else {
			DrawText("Settings", x_text_position, 250, 25, RED);
		}

		x_text_position = worldBorder->width / 2 - MeasureText("Back to Menu", 25)/2;
    if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 300, 200, 30})){
		DrawText("Back to Menu", x_text_position, 300, 25, DARKGRAY);
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
			*Menu = true;
         *Pause = false;
		}
	} else {
		DrawText("Back to Menu", x_text_position, 300, 25, RED);
    }
}