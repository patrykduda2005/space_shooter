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

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
void updateGravity() {
    for (int i = 0; i < entities->ents.size(); i++) {
        auto ent = entities->ents[i];
        auto grav = ent->get_component<Gravity>();
        auto pos = ent->get_component<Position>();
        if (pos && grav) {
            pos->y -= grav->g;
        }
    }
}

int main ()
{
    // Tworzenie nowego entity
    Entity* statek = entities->new_entity();
    // Dodanie componentow do statku
    statek->add_component<Gravity>({.g = 0.9});
    statek->add_component<Position>({.x = 0.0, .y = 10.0});

    std::cout << "Wypisanie wartosci aktualnych" << std::endl;
    std::cout << statek->get_component<Gravity>()->g << std::endl;
    std::cout << statek->get_component<Position>()->y << std::endl;

    updateGravity();

    std::cout << "Wypisanie wartosci aktualnych" << std::endl;
    std::cout << statek->get_component<Gravity>()->g << std::endl;
    std::cout << statek->get_component<Position>()->y << std::endl;
    return 0;
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1000, 800, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText("Hello Raylib", 200,200,20,WHITE);

		// draw our texture to the screen
		DrawTexture(wabbit, 400, 200, WHITE);
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
