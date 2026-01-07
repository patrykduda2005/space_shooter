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


bool Pause = false;
bool Datalog = false;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	//SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	initWorldSize();
   auto worldBorder = resources->get_component<WorldBorder>();
	InitWindow(worldBorder->width, worldBorder->height, "RISE OF THE WIELKI PIEC");
	SetTargetFPS(150);
	InitAudioDevice();				  // Initialize audio device
	initResources();
	initAmmoCounter();

	Rectangle resumeBtn = {400, 185, 200, 50};	
	Rectangle exitBtn = {400, 445, 200, 50};

	SetExitKey(KEY_NULL);

	SearchAndSetResourceDir("resources");
	

	int type = 1;

	//WIELKI PIEC
    Entity* rabbit = entities->new_entity();
    rabbit->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    rabbit->add_component<Position>({.x = 500, .y = 700});
    rabbit->add_component<ArrowMovement>({200, 400, 200, 400});
    rabbit->add_component<RestrictToWorld>({});	
	 rabbit->add_component<Shooting>({.cooldown = 0.0});

    Entity* enemy = entities->new_entity();
    enemy->add_component<Position>({.x = 500, .y = 100});
    enemy->add_component<Render>({.txt = LoadTexture("wielki_piec.png")});
    enemy->add_component<Hitbox>({.layer = HitboxLayer::Enemies, .interactsWith = HitboxLayer::Nothing, .collisionBox = Area(Vec2(0,0), Vec2(100,100))});
	// game loop
	

	while (!WindowShouldClose()) {		// run the loop untill the user presses ESCAPE or presses the Close button on the window 
      
		float d = GetFrameTime();
		Vector2 mousePosition = GetMousePosition();
		  
		if(IsKeyPressed(KEY_ESCAPE)) Pause = !Pause;
		if(IsKeyPressed(KEY_F3)) Datalog = !Datalog;

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(WHITE);

		if (!Pause){
			DrawText("Ammo", 875,650,25,BLACK);
			DrawText("Press 1, 2 or 3 to change shooting type", 50,30,25,BLACK);
			//DrawText(std::to_string(type).c_str(), 50,75,25,BLACK);

			if (IsKeyPressed(KEY_ONE)) type = 1;
			if (IsKeyPressed(KEY_TWO)) type = 2;
			if (IsKeyPressed(KEY_THREE)) type = 3;
			
			if(Datalog){
				DrawText("FPS:", 50,700,15,BLACK);
				DrawText(std::to_string(GetFPS()).c_str(), 120,700,15,BLACK); 
				DrawText("Entities:", 50,720,15,BLACK);
				DrawText(std::to_string(entities->get().size()).c_str(), 150,720,15,BLACK);
				DrawText("x:", 50,740,15,BLACK);
				DrawText(std::to_string(mousePosition.x).c_str(), 80,740,15,BLACK);
				DrawText("y:", 175,740,15,BLACK);	
				DrawText(std::to_string(mousePosition.y).c_str(), 205,740,15,BLACK);
			
			}

			shoot(type);
		   updateGravity(d);
			updateVelocity(d);
	      renderThings(d);
   	   arrowMovement(d);
  		   restrictToWorld(d);
   	   destroyBeyondWorld();
   	 	detectCollision();
     		outlineColliders();
			ammoCounter(type);
		} 
		else {
			DrawText("PAUSED", 400,100,50,RED);

			if(Datalog){
				DrawText("x:", 50,700,15,BLACK);
				DrawText(std::to_string(mousePosition.x).c_str(), 80,700,15,BLACK);
				DrawText("y:", 175,700,15,BLACK);	
				DrawText(std::to_string(mousePosition.y).c_str(), 205,700,15,BLACK);
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
				DrawText("Exit", 470, 460, 25, DARKGRAY);
				if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
					 break; // Exit the game loop
				}
		  } else {
				DrawText("Exit", 470, 460, 25, RED);
		  }

		  //DrawText("")
		
		
		}
		EndDrawing();
	}
		
	unLoadResources();
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
