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
#include "customentities.h"


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
	// auto settingsComp = resources->get_component<SettingsComponent>();
	
	// int fps_index = 1;
	// float volume = 0.5f;
	// int volume_int;
	// float sfx_volume = 0.5f;
	// int sfx_volume_int;

	// Create the window and OpenGL context
	initWorldSize();
   auto worldBorder = resources->get_component<WorldBorder>();
	InitWindow(worldBorder->width, worldBorder->height, "RISE OF THE WIELKI PIEC");
	SetTargetFPS(60);
	InitAudioDevice();				  // Initialize audio device
	initResources();
	initMusicResources();
	initAmmoCounter();
	initKeyBinds();
	initSettingsComponent();
	SetExitKey(KEY_NULL);

	Rectangle resumeBtn = {395, 190, 200, 45};	
	Rectangle exitBtn = {395, 400, 200, 45};
	Rectangle settBtn = {395, 240, 200, 45};

	int x_text_position;// = worldBorder->width / 2 - 50;

	SearchAndSetResourceDir("resources");
	

	int exit_int = 0;
	int type = 1;

	//WIELKI PIEC


    spawnEnemy({.x = 500, .y = 500});
    spawnEnemy({.x = 500, .y = 100});
    spawnEnemy({.x = 200, .y = 300});
    spawnEnemy({.x = 300, .y = 300});

    spawnPlayer();
	// game loop
	
	auto keyb = resources->get_component<KeyBinds>();
	auto musicRes = resources->get_component<MusicResources>();
	auto res = resources->get_component<soundTextureResources>();
	auto settingsComp = resources->get_component<SettingsComponent>();

	SetMusicVolume(musicRes->backgroundMusic, settingsComp->volume);
	SetSoundVolume(res->shootingsfx, settingsComp->sfx_volume);

	

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

        removeHitbox();
    	detectCollision();
        suckToBlack(d);
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
			settingsSystem(&Datalog, &menuANDsett, &Menu, &Sett, mousePosition, KeybindsBtt);
		}
		else if (Menu){ //Menu startowe
			menuSystem(&Menu, &Sett, &Pause, &menuANDsett, mousePosition, &exit_int);
			if(exit_int == 1){
				break; // Exit the game loop
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
