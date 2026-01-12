/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include <iostream>
#include <string>
#include "raylib.h"
#include "ecs.h"
#include "components.h"
#include "resources.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

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
            .collisionBox = size,
            });
    rabbit->add_component<Hp>({.hp = 10});
    rabbit->add_component<HpOffset>({
            .global = false,
            .vec = size.left_up_corner - Vec2(0, 25),
            });
}

bool Pause = true;
bool Datalog = false;
bool Sett = false;
bool KeybindsBtt[8] = {false};
bool Menu = true;
bool menuANDsett = false;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	//SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	std::string fps[5] = {"30", "60", "120", "144", "Unlimited"};
	int fps_index = 1;
	float volume = 0.5f;
	int volume_int;
	float sfx_volume = 0.5f;
	int sfx_volume_int;

	// Create the window and OpenGL context
	initWorldSize();
   auto worldBorder = resources->get_component<WorldBorder>();
	InitWindow(worldBorder->width, worldBorder->height, "RISE OF THE WIELKI PIEC");
	SetTargetFPS(std::stoi(fps[fps_index]));
	InitAudioDevice();				  // Initialize audio device
	initResources();
	initMusicResources();
	initAmmoCounter();
	initKeyBinds();
	SetExitKey(KEY_NULL);

	Rectangle resumeBtn = {395, 190, 200, 45};	
	Rectangle exitBtn = {395, 400, 200, 45};
	Rectangle settBtn = {395, 240, 200, 45};

	int x_text_position;// = worldBorder->width / 2 - 50;

	SearchAndSetResourceDir("resources");
	

	int key_number = 0;
	int type = 1;

	//WIELKI PIEC

    Entity* enemy = entities->new_entity();
    enemy->add_component<Position>({.x = 500, .y = 500});
    enemy->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    enemy->add_component<Hitbox>({
            .layer = HitboxLayer::Enemies,
            .interactsWith = HitboxLayer::Players,
            .collisionBox = Area(Vec2(-50,-50), Vec2(50,50)),
            .applies = {create_component<Destroy>({})}
            });
    enemy->add_component<Hp>({.hp = 10});

    Entity* enemy2 = entities->new_entity();
    enemy2->add_component<Position>({.x = 500, .y = 100});
    enemy2->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    enemy2->add_component<Hitbox>({
            .layer = HitboxLayer::Enemies,
            .interactsWith = HitboxLayer::Players,
            .collisionBox = Area(Vec2(-50,-50), Vec2(50,50)),
            .applies = {create_component<Destroy>({})}
            });
    enemy2->add_component<Hp>({.hp = 10});

    spawnPlayer();
	// game loop
	
	auto keyb = resources->get_component<KeyBinds>();
	auto musicRes = resources->get_component<MusicResources>();
	auto res = resources->get_component<soundTextureResources>();

	SetMusicVolume(musicRes->backgroundMusic, volume);
	SetSoundVolume(res->shootingsfx, sfx_volume);

	

	while (!WindowShouldClose()) {		// run the loop untill the user presses ESCAPE or presses the Close button on the window 
      
		float d = GetFrameTime();
		Vector2 mousePosition = GetMousePosition();
		  
		//if(IsKeyPressed(KEY_ESCAPE)) Pause = !Pause;
		if(IsKeyPressed(KEY_F3)) Datalog = !Datalog;

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(WHITE);

		if (!Pause && !Menu){ //Główna gra
			if(IsKeyPressed(KEY_ESCAPE)) Pause = !Pause;

			DrawTexturePro(res->background, {0.0f, 0.0f, (float)res->background.width, (float)res->background.height}, {0.0f, 0.0f, worldBorder->width, worldBorder->height}, {0.0f, 0.0f}, 0.0f, WHITE);
			
			//Ammo counter
			DrawText("Ammo", 875,650,25,WHITE);
			
			if(IsMusicStreamPlaying(musicRes->menuMusic)){
				StopMusicStream(musicRes->menuMusic);
				ResumeMusicStream(musicRes->backgroundMusic);
			}

			if(IsMusicStreamPlaying(musicRes->backgroundMusic)){
				UpdateMusicStream(musicRes->backgroundMusic);
			} else {
				PlayMusicStream(musicRes->backgroundMusic);
			}

			if (IsKeyPressed(keyb->type_shoot1)) type = 1;
			if (IsKeyPressed(keyb->type_shoot2)) type = 2;
			if (IsKeyPressed(keyb->type_shoot3)) type = 3;
			
			if(Datalog){
				DrawText("FPS:", 50,700,15,WHITE);
				DrawText(std::to_string(GetFPS()).c_str(), 120,700,15,WHITE); 
				DrawText("Entities:", 50,720,15,WHITE);
				DrawText(std::to_string(entities->get().size()).c_str(), 150,720,15,WHITE);
				DrawText("x:", 50,740,15,WHITE);
				DrawText(std::to_string(mousePosition.x).c_str(), 80,740,15,WHITE);
				DrawText("y:", 175,740,15,WHITE);	
				DrawText(std::to_string(mousePosition.y).c_str(), 205,740,15,WHITE);
			
			}

    	detectCollision();
            delay();
            spawn();
		shoot(type);
	   updateGravity(d);
		updateVelocity(d);
      renderThings(d);
      arrowMovement(d);
      restrictToWorld(d);
      destroyBeyondWorld();
      outlineColliders();
		ammoCounter(type);
        displayhp();
        damage();
        die();
        destroy();
		} 
		else if((Sett && !Menu) || (Sett && Pause)){ //Ustawienia
			if(IsMusicStreamPlaying(musicRes->backgroundMusic)){
				StopMusicStream(musicRes->backgroundMusic);
			}

			if(IsMusicStreamPlaying(musicRes->menuMusic)){
				UpdateMusicStream(musicRes->menuMusic);
			} else {
				PlayMusicStream(musicRes->menuMusic);
			}

			x_text_position = worldBorder->width / 2 - MeasureText("Settings", 50)/2;

			if(Datalog){
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

			x_text_position = worldBorder->width / 2 - MeasureText(fps[fps_index].c_str(), 30)/2;
			DrawText(fps[fps_index].c_str(), x_text_position, 130, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 130, 30, 30})){
				DrawText("<", x_text_position - 100, 130, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && fps_index > 0){
					fps_index--;
					SetTargetFPS(std::stoi(fps[fps_index]));
				}
			} else {
				DrawText("<", x_text_position - 100, 130, 30, RED);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(fps[fps_index].c_str(), 30)) + 100, 130, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(fps[fps_index].c_str(), 30)) + 100, 130, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && fps_index < 4){
					fps_index++;
					if(fps_index == 4) SetTargetFPS(0);
					else SetTargetFPS(std::stoi(fps[fps_index]));
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(fps[fps_index].c_str(), 30)) + 100, 130, 30, RED);
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
			volume_int = (int)(volume * 10);

			x_text_position += MeasureText("Volume", 30)/2;
			DrawText(std::to_string(volume_int).c_str(), x_text_position, 590, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 590, 30, 30})){
				DrawText("<", x_text_position - 100, 590, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && volume > 0.0f){
					volume -= 0.1f;
					SetMusicVolume(musicRes->backgroundMusic, volume);
					SetMusicVolume(musicRes->menuMusic, volume);
				}
			} else {
				DrawText("<", x_text_position - 100, 590, 30, RED);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(std::to_string(volume_int).c_str(), 30)) + 100, 590, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(std::to_string(volume_int).c_str(), 30)) + 100, 590, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && volume < 1){
					volume += 0.1f;
					SetMusicVolume(musicRes->backgroundMusic, volume);
					SetMusicVolume(musicRes->menuMusic, volume);
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(std::to_string(volume_int).c_str(), 30)) + 100, 590, 30, RED);
			}

			DrawText("Sounds:", 100, 630, 25, RED);
			sfx_volume_int = (int)(sfx_volume * 10);

			DrawText(std::to_string(sfx_volume_int).c_str(), x_text_position, 630, 30, BLACK);

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position - 100, 630, 30, 30})){
				DrawText("<", x_text_position - 100, 630, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && sfx_volume > 0.0f){
					sfx_volume -= 0.1f;
					SetSoundVolume(res->shootingsfx, sfx_volume);
				}
			} else {
				DrawText("<", x_text_position - 100, 630, 30, RED);
			}
			if(CheckCollisionPointRec(mousePosition, {(float)(x_text_position + MeasureText(std::to_string(sfx_volume_int).c_str(), 30)) + 100, 630, 30, 30})){
				DrawText(">", (x_text_position + MeasureText(std::to_string(sfx_volume_int).c_str(), 30)) + 100, 630, 30, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && sfx_volume < 1){
					sfx_volume += 0.1f;
					SetSoundVolume(res->shootingsfx, sfx_volume);
				}
			} else {
				DrawText(">", (x_text_position + MeasureText(std::to_string(sfx_volume_int).c_str(), 30)) + 100, 630, 30, RED);
			}

			//Wyjście z ustawień
			x_text_position = worldBorder->width / 2 - MeasureText("Back", 25)/2;

			if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 700, 100, 30})){
				DrawText("Back", x_text_position, 700, 25, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					if(menuANDsett){
						Sett = false;
						Menu = true;
						menuANDsett = false;
					}
					else Sett = false; // Exit the settings menu
				}
		  } else {
				DrawText("Back", x_text_position, 700, 25, RED);
		  }

		}
		else if (Menu){ //Menu startowe
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
					 Menu = false; 
					 Pause = false;
				}
		  } else {
				DrawText("Start", x_text_position, 210, 50, BLACK);
		  }

		  x_text_position = worldBorder->width / 2 - MeasureText("Settings", 50)/2;
		  if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 310, 200, 45})){
				DrawText("Settings", x_text_position, 310, 50, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					 Sett = true;
					 Menu = false;
					 Pause = true;
					 menuANDsett = true;
				}
		  } else {
				DrawText("Settings", x_text_position, 310, 50, BLACK);
		  }

		  x_text_position = worldBorder->width / 2 - MeasureText("Exit", 50)/2;
		  if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 412, 200, 45})){
				DrawText("Exit", x_text_position, 412, 50, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					 break; // Exit the game loop
				}
		  } else {
				DrawText("Exit", x_text_position, 412, 50, BLACK);
		  }

		}
		else { //Menu pauzy
			DrawText("PAUSED", 400,100,50,RED);

			if(IsMusicStreamPlaying(musicRes->backgroundMusic)){
				PauseMusicStream(musicRes->backgroundMusic);
			}

			if(IsMusicStreamPlaying(musicRes->menuMusic)){
				UpdateMusicStream(musicRes->menuMusic);
			} else {
				PlayMusicStream(musicRes->menuMusic);
			}

			if (CheckCollisionPointRec(mousePosition, resumeBtn)) {
				DrawText("Resume", 450, 200, 25, DARKGRAY);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Pause = false; // Wracamy do gry
            }
        } else {
				DrawText("Resume", 450, 200, 25, RED);
		  }

		  if(CheckCollisionPointRec(mousePosition, exitBtn)){
				DrawText("Exit", 470, 412, 25, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					 break; // Exit the game loop
				}
		  } else {
				DrawText("Exit", 470, 412, 25, RED);
		  }

		  if(CheckCollisionPointRec(mousePosition, settBtn)){
				DrawText("Settings", 445, 250, 25, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					 Sett = true;
				}
		  } else {
				DrawText("Settings", 445, 250, 25, RED);
		  }

		  x_text_position = worldBorder->width / 2 - MeasureText("Back to Menu", 25)/2;
		  if(CheckCollisionPointRec(mousePosition, {(float)x_text_position, 300, 200, 30})){
				DrawText("Back to Menu", x_text_position, 300, 25, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					 Menu = true;
				}
		  } else {
				DrawText("Back to Menu", x_text_position, 300, 25, RED);
		  }
		}
		EndDrawing();
	}
		
	unLoadResources();
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
