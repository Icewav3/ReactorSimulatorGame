#include <iostream>
#include <memory> // For std::unique_ptr
#include "raylib.h"
#include "Classes/Management/CanvasManager.h"
#include "Classes/Management/GameManager.h"
#include "Classes/Reactor/ReactorManager.h"

int main() {
	// Initialize the window
	const int screenWidth = GetMonitorWidth(0);
	const int screenHeight = GetMonitorHeight(0);

	InitWindow(screenWidth, screenHeight, "Reactor Simulator");
	SetTargetFPS(60);

	// Create managers
	GameManager gameManager;
	CanvasManager canvasManager;
	// Use a smart pointer for automatic memory management.
	std::unique_ptr<ReactorManager> reactorManager = nullptr;

	while (!WindowShouldClose()) {
		float deltaTime = GetFrameTime();

		// Handle scene transitions
		GameScene currentScene = gameManager.GetCurrentScene();

		// Input handling for scene transitions
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			switch (currentScene) {
				case GameScene::MAIN_MENU:
					gameManager.SetScene(GameScene::INTRO_SEQUENCE);
					break;

				case GameScene::INTRO_SEQUENCE:
					// Create a new reactor for play mode using a smart pointer.
					reactorManager = std::make_unique<ReactorManager>();
					gameManager.Reset(); // Reset statistics for the new game.
					gameManager.SetScene(GameScene::PLAYING);
					break;

				case GameScene::END_SCREEN:
					// Clean up and return to menu. .reset() handles deletion.
					reactorManager.reset();
					gameManager.Reset();
					break;

				case GameScene::PLAYING:
					// No click action during play
					break;
			}
		}

		// Update game logic
		if (currentScene == GameScene::PLAYING && reactorManager) {
			// Pass raw pointers or references to functions that do not take ownership.
			gameManager.Update(deltaTime, reactorManager.get());
			reactorManager->Update(deltaTime);
		}

		// Render
		BeginDrawing();

		switch (currentScene) {
			case GameScene::MAIN_MENU:
				canvasManager.RenderMainMenu();
				break;

			case GameScene::INTRO_SEQUENCE:
				canvasManager.RenderIntroSequence();
				break;

			case GameScene::PLAYING:
				if (reactorManager) {
					// Pass raw pointers or references to functions that do not take ownership.
					canvasManager.RenderPlayMode(reactorManager.get(), &gameManager, deltaTime);
				}
				break;

			case GameScene::END_SCREEN:
				canvasManager.RenderEndScreen(gameManager.GetStatistics());
				break;
		}
		EndDrawing();
	}

	// Cleanup is now automatic thanks to std::unique_ptr.

	CloseWindow();
	return 0;
}
