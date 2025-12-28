/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include <iostream>
#include "raylib.h"
#include "ecs.h"
#include "components.h"
#include "resources.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

int main ()
{
	SetTargetFPS(144);
    initResources();
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
    auto worldBorder = resources->get_component<WorldBorder>();
	InitWindow(worldBorder->width, worldBorder->height, "AGH TALES: RISE OF THE WIELKI PIEC");
	InitAudioDevice();				  // Initialize audio device
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	Sound shootingsfx = LoadSound("shoot.wav");

	int type = 1;
	int ammoCount[3] = {0,21,31};
	int *ammoPointer = &ammoCount[0];

	//WIELKI PIEC
    Entity* rabbit = entities->new_entity();
    rabbit->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    rabbit->add_component<Position>({.x = 500, .y = 700});
    rabbit->add_component<ArrowMovement>({200, 400, 200, 400});
    rabbit->add_component<RestrictToWorld>({});	
	rabbit->add_component<Shooting>({.cooldown = 0.0});
	
/*
	// ammunicja
		Entity* ammo = entities->new_entity();
		ammo->add_component<Render>({.txt = LoadTexture("wabbit_alpha.png")});
		ammo->add_component<Position>({.x = 500, .y = 100});
		ammo->add_component<Gravity>({.g = 0.0});
		ammo->add_component<DestroyBeyondWorld>({});
*/

	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
        float d = GetFrameTime();
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(WHITE);

		DrawText("Ammo", 875,650,25,BLACK);
		DrawText("Press 1, 2 or 3 to change shooting type", 50,30,25,BLACK);
		//DrawText(std::to_string(type).c_str(), 50,75,25,BLACK);

		if (IsKeyPressed(KEY_ONE)) type = 1;
		if (IsKeyPressed(KEY_TWO)) type = 2;
		if (IsKeyPressed(KEY_THREE)) type = 3;
		
		
		DrawText("FPS:", 50,700,25,BLACK);
		DrawText(std::to_string(GetFPS()).c_str(), 120,700,25,BLACK); 

		shoot(type, ammoPointer, shootingsfx);
	   updateGravity(d);
		updateVelocity(d);
      renderThings(d);
      arrowMovement(d);
      restrictToWorld(d);
      destroyBeyondWorld();
		ammoCounter(type, ammoPointer);
        //std::cout << "Entities: " << entities->get().size() << "\n";
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	//UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	UnloadSound(shootingsfx); // Usuwamy dźwięk z pamięci
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
