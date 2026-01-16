#include <iostream>
#include <string>
#include "raylib.h"
#include "ecs.h"
#include "components.h"
#include "resources.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "customentities.h"


bool Pause = false;
bool Datalog = false;
bool Sett = false;
bool KeybindsBtt[8] = {false};
bool Menu = true;
//bool menuANDsett = false;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	//SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	std::string fps[5] = {"30", "60", "120", "144", "Unlimited"};
	
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

	// Rectangle resumeBtn = {395, 190, 200, 45};	
	// Rectangle exitBtn = {395, 400, 200, 45};
	// Rectangle settBtn = {395, 240, 200, 45};

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

		if (!Menu && !Pause){ //Główna gra
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
		else if(Sett){ //Ustawienia
			settingsSystem(&Datalog, &Pause, &Menu, &Sett, mousePosition, KeybindsBtt);
		}
		else if (Menu){ //Menu startowe
			menuSystem(&Menu, &Sett, &Pause, mousePosition, &exit_int);
			if(exit_int == 1){
				break; // Exit the game loop
			}
		}
		else if (Pause){ //Menu pauzy
			pauseSystem(mousePosition, &Pause, &Sett, &Menu, &exit_int);
			if(exit_int == 1){
				break; // Exit the game loop
			}
		}
		EndDrawing();
	}

	savingSettings();
	unLoadResources();
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
