#ifndef CPP_PROJECTS_CANVASMANAGER_H
#define CPP_PROJECTS_CANVASMANAGER_H

#include <raylib.h>
#include <memory> // For std::unique_ptr
#include "GameManager.h"

class ReactorManager;
class ReactorConsole;

/**
 * @class CanvasManager
 * @brief Manages all rendering operations for the application's UI.
 * This class is responsible for drawing all game scenes, including the main menu,
 * gameplay UI, and end screen. Owns the play-mode Console and the segment font;
 * the console owns the panels and instruments.
 */
class CanvasManager {
public:
	CanvasManager();
	~CanvasManager();

	/// Renders the main menu screen.
	void RenderMainMenu();

	/// Renders the introductory story sequence.
	void RenderIntroSequence();

	/// Renders the main gameplay screen, including all instruments and data.
	void RenderPlayMode(ReactorManager* reactorManager, GameManager* gameManager, float deltaTime);

	/// Renders the end screen with final game statistics.
	void RenderEndScreen(const GameStatistics& stats);

	/// Restores all play-mode console widgets to their constructed defaults.
	/// Must be called when a new ReactorManager is built, otherwise the prior
	/// run's slider/lever/handwheel positions will be pushed into the fresh
	/// sim on the first PLAYING frame and overwrite its default state.
	void ResetPlayConsole();

private:
	/// Draws the revenue display.
	void DrawRevenue(float revenue);

	/// Draws the elapsed time display.
	void DrawTimer(GameManager* gameManager);

	/// Helper function to draw text with the 7-segment display font.
	void DrawSegmentDisplay(const std::string& text, Vector2 position, float fontSize, Color color, const std::string& label);

	std::unique_ptr<ReactorConsole> playConsole_;

	Font sevenSegmentFont_;
	bool fontLoaded_;
};

#endif
