#ifndef CPP_PROJECTS_CANVASMANAGER_H
#define CPP_PROJECTS_CANVASMANAGER_H

#include <raylib.h>
#include <memory> // For std::unique_ptr
#include "Classes/UIElements/Dial.h"
#include "Classes/UIElements/Slider.h"
#include "GameManager.h"

class ReactorManager;

/**
 * @class CanvasManager
 * @brief Manages all rendering operations for the application's UI.
 * This class is responsible for drawing all game scenes, including the main menu,
 * gameplay UI, and end screen. It owns and manages all UI elements.
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

private:
	/// Updates the state of UI controls based on player input.
	void UpdatePlayModeControls(ReactorManager* reactorManager, float deltaTime);

	/// Draws the main gameplay UI elements.
	void DrawPlayModeUI(float deltaTime);

	/// Draws the revenue display.
	void DrawRevenue(float revenue);

	/// Draws the elapsed time display.
	void DrawTimer(GameManager* gameManager);

	/// Helper function to draw text with the 7-segment display font.
	void DrawSegmentDisplay(const std::string& text, Vector2 position, float fontSize, Color color, const std::string& label);

	// UI Components are managed by smart pointers for automatic memory management.
	std::unique_ptr<Slider> coolantSlider_;
	std::unique_ptr<Slider> controlRodSlider_;

	Dial tempDial_;
	Dial rpmDial_;
	Dial pressureDial_;
	Dial powerOutputDial_;

	Font sevenSegmentFont_;
	bool fontLoaded_;
};

#endif
