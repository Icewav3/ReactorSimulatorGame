#include <memory>
#include "raylib.h"
#include "Classes/Management/CanvasManager.h"
#include "Classes/Management/GameManager.h"
#include "Classes/Reactor/ReactorManager.h"
#include "Classes/UI/VirtualScreen.h"

int main() {
	constexpr int kInitialWidth = 1920;
	constexpr int kInitialHeight = 1080;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(kInitialWidth, kInitialHeight, "Reactor Simulator");
	SetTargetFPS(60);

	// Virtual framebuffer must be created after InitWindow (needs GL context).
	VirtualScreen::Init();

	GameManager gameManager;
	CanvasManager canvasManager;
	std::unique_ptr<ReactorManager> reactorManager;

	while (!WindowShouldClose()) {
		const float deltaTime = GetFrameTime();
		const GameScene currentScene = gameManager.GetCurrentScene();

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			switch (currentScene) {
				case GameScene::MAIN_MENU:
					gameManager.SetScene(GameScene::INTRO_SEQUENCE);
					break;

				case GameScene::INTRO_SEQUENCE:
					reactorManager = std::make_unique<ReactorManager>();
					gameManager.Reset();
					gameManager.SetScene(GameScene::PLAYING);
					break;

				case GameScene::END_SCREEN:
					reactorManager.reset();
					gameManager.Reset();
					break;

				case GameScene::PLAYING:
					break;
			}
		}

		if (currentScene == GameScene::PLAYING && reactorManager) {
			gameManager.Update(deltaTime, reactorManager.get());
			reactorManager->Update(deltaTime);
		}

		BeginDrawing();
		VirtualScreen::Begin();

		switch (currentScene) {
			case GameScene::MAIN_MENU:
				canvasManager.RenderMainMenu();
				break;

			case GameScene::INTRO_SEQUENCE:
				canvasManager.RenderIntroSequence();
				break;

			case GameScene::PLAYING:
				if (reactorManager) {
					canvasManager.RenderPlayMode(reactorManager.get(), &gameManager, deltaTime);
				}
				break;

			case GameScene::END_SCREEN:
				canvasManager.RenderEndScreen(gameManager.GetStatistics());
				break;
		}

		VirtualScreen::End();
		VirtualScreen::Present();
		EndDrawing();
	}

	VirtualScreen::Shutdown();
	CloseWindow();
	return 0;
}
