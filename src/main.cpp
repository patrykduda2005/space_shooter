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
bool KeybindsBtt[9] = {false};
bool Menu = true;
bool Shop = false;
bool hasStarted = false;


void initGame() {
	// Create the window and OpenGL context
	initWorldSize();
    auto worldBorder = resources->get_component<WorldBorder>();
	InitWindow(worldBorder->width, worldBorder->height, "RISE OF THE WIELKI PIEC");
	InitAudioDevice();				  // Initialize audio device
	initResources();
	initMusicResources();
	initAmmoCounter();
	initKeyBinds();
	initSettingsComponent();
	SetExitKey(KEY_MINUS);

	SearchAndSetResourceDir("resources");
    spawnPlayer();
	
	


	Entity* waveManager = entities->new_entity();
	WaveManager wm;
	wm.currentWave = 1;
	wm.enemiesInWave = 8;
	wm.enemiesKilled = 0;
	wm.waveActive = true;
	wm.waveTimer = 0.0f;
	wm.spawnTimer = 0.5f;
	waveManager->add_component<WaveManager>(wm);
}

void resetAll() {
    for (Entity* ent : entities->get()) {
        entities->kill_entity(ent);
    }
    resources->remove_component<AmmoCounter>();
    resources->remove_component<Money>();

    initAmmoCounter();
    spawnPlayer();
	Entity* waveManager = entities->new_entity();
	WaveManager wm;
	wm.currentWave = 1;
	wm.enemiesInWave = 8;
	wm.enemiesKilled = 0;
	wm.waveActive = true;
	wm.waveTimer = 0.0f;
	wm.spawnTimer = 0.5f;
	waveManager->add_component<WaveManager>(wm);
}

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	//SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	
    initGame();
	int x_text_position;// = worldBorder->width / 2 - 50;

	

	int exit_int = 0;
	int type = 1;

	std::string fps[5] = {"30", "60", "120", "144", "Unlimited"};
	auto keyb = resources->get_component<KeyBinds>();
	auto musicRes = resources->get_component<MusicResources>();
	auto res = resources->get_component<soundTextureResources>();
	auto settingsComp = resources->get_component<SettingsComponent>();
	auto cash = resources->get_component<Money>();
    auto worldBorder = resources->get_component<WorldBorder>();
	

	if(settingsComp->fps_index < 4) SetTargetFPS(std::stoi(fps[settingsComp->fps_index]));
	else SetTargetFPS(0);


	while (!WindowShouldClose()) {		// run the loop untill the user presses ESCAPE or presses the Close button on the window 
      SetMusicVolume(musicRes->backgroundMusic, (float)settingsComp->volume/10);
		SetMusicVolume(musicRes->menuMusic, (float)settingsComp->volume/10);
		SetSoundVolume(res->shootingsfx, (float)settingsComp->sfx_volume/10);

		float d = GetFrameTime();
		Vector2 mousePosition = GetMousePosition();
		  
		//if(IsKeyPressed(KEY_ESCAPE)) Pause = !Pause;
		if(IsKeyPressed(KEY_F3)) Datalog = !Datalog;

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(WHITE);

		if (!Menu && !Pause && !Shop && !Sett){ //Główna gra
            hasStarted = true;
			if(IsKeyPressed(KEY_ESCAPE)) Pause = !Pause;

			DrawTexturePro(res->background, {0.0f, 0.0f, (float)res->background.width, (float)res->background.height}, {0.0f, 0.0f, worldBorder->width, worldBorder->height}, {0.0f, 0.0f}, 0.0f, WHITE);
			
			//Ammo counter
			DrawText("Ammo", 875,650,25,WHITE);
			DrawText("Money: ", 800, 20, 20, WHITE);
			DrawText(std::to_string(cash->money).c_str(), 880,20,20,WHITE);
			
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
			if( IsKeyPressed(keyb->type_shoot4)) type = 4;
			
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
	   updateWaveManager(d);
	   bounceOffWalls();
		updateVelocity(d);
      renderThings(d);
      arrowMovement(d);
      restrictToWorld(d);
      destroyBeyondWorld();
      outlineColliders(Datalog);
		ammoCounter(type);
        displayhp();
		updateInvulnerability(d);
		updateEnemyShooting(d);
        damage();
		collectDropItems();
		updateDropItems(d);
		collectDropItems();
        die();
        destroy();
		} 
		else if(Sett){ //Ustawienia
			settingsSystem(&Datalog, &Pause, &Menu, &Sett, mousePosition, KeybindsBtt);
		}		
		else if (Shop){
			shopSystem(&Shop, &Pause, &Menu, &Datalog, mousePosition);
		}
		else if (Menu){ //Menu startowe
            if (hasStarted) resetAll();
			menuSystem(&Datalog, &Menu, &Sett, &Pause, &Shop, mousePosition, &exit_int);
			if(exit_int == 1){
				break; // Exit the game loop
			}
		}
		else if (Pause){ //Menu pauzy
			pauseSystem(mousePosition, &Pause, &Sett, &Menu, &Shop, &exit_int);
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
