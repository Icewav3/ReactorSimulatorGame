#include "Classes/Management/CanvasManager.h"
#include "Classes/Reactor/ReactorManager.h"
#include "Classes/SimIO/OutputSnapshot.h"
#include "Classes/SimIO/InputBus.h"
#include "Classes/UI/ReactorConsole.h"
#include "Classes/UI/Theme.h"
#include "Classes/UI/VirtualScreen.h"
#include <cstdio>
#include <format>

CanvasManager::CanvasManager()
	: fontLoaded_(false) {
	// Load 7-segment font
	sevenSegmentFont_ = LoadFont("Resources/7segment.ttf");
	if (sevenSegmentFont_.texture.id == 0) {
		// Fallback for different working directory
		sevenSegmentFont_ = LoadFont("resources/7segment.ttf");
	}
	fontLoaded_ = sevenSegmentFont_.texture.id != 0;

	// All drawing now targets the fixed virtual framebuffer, so the viewport
	// is anchored to virtual dimensions instead of GetScreenWidth/Height
	// (which return the resizable OS window). The top band leaves room for
	// the timer (centered) and revenue (right). Must stay in sync with
	// DrawRevenue / DrawTimer.
	constexpr float kTopBand = 100.0f;
	const Rectangle viewport{
		0.0f,
		kTopBand,
		static_cast<float>(VirtualScreen::Width()),
		static_cast<float>(VirtualScreen::Height()) - kTopBand,
	};
	playConsole_ = std::make_unique<ReactorConsole>(viewport);
}

CanvasManager::~CanvasManager() {

	if (fontLoaded_) {
		UnloadFont(sevenSegmentFont_);
	}
}

void CanvasManager::ResetPlayConsole() {
	if (playConsole_) {
		playConsole_->Reset();
	}
}

void CanvasManager::RenderMainMenu() {
	ClearBackground(RAYWHITE);

	const char* title = "REACTOR SIMULATOR";
	int titleFontSize = 72;
	int titleWidth = MeasureText(title, titleFontSize);
	DrawText(title, (VirtualScreen::Width() - titleWidth) / 2, VirtualScreen::Height() / 3, titleFontSize, DARKGRAY);

	const char* prompt = "Click anywhere to start";
	int promptFontSize = 32;
	int promptWidth = MeasureText(prompt, promptFontSize);
	DrawText(prompt, (VirtualScreen::Width() - promptWidth) / 2, VirtualScreen::Height() / 2 + 50, promptFontSize, GRAY);
}

void CanvasManager::RenderIntroSequence() {
	ClearBackground(Theme::kPanelBeige);

	const char* line1 = "You always wanted a nuclear reactor, so you ordered one off Temu.";
	const char* line2 = "You could've sworn it was called the CANDU reactor,";
	const char* line3 = "but the label says CANDONT...";

	int fontSize = 32;
	int lineSpacing = 50;
	int startY = VirtualScreen::Height() / 3;

	int line1Width = MeasureText(line1, fontSize);
	int line2Width = MeasureText(line2, fontSize);
	int line3Width = MeasureText(line3, fontSize);

	DrawText(line1, (VirtualScreen::Width() - line1Width) / 2, startY, fontSize, DARKGRAY);
	DrawText(line2, (VirtualScreen::Width() - line2Width) / 2, startY + lineSpacing, fontSize, DARKGRAY);
	DrawText(line3, (VirtualScreen::Width() - line3Width) / 2, startY + lineSpacing * 2, fontSize, DARKGRAY);

	const char* continueText = "Click to continue...";
	int continueFontSize = 24;
	int continueWidth = MeasureText(continueText, continueFontSize);
	DrawText(continueText, (VirtualScreen::Width() - continueWidth) / 2, VirtualScreen::Height() - 100, continueFontSize, GRAY);
}

void CanvasManager::RenderPlayMode(ReactorManager* reactorManager, GameManager* gameManager, float deltaTime) {
	ClearBackground(Theme::kPanelBeige);

	if (!reactorManager || !gameManager) {
		return;
	}

	// Build the SimIO contract for this frame and let the console own the
	// instrument lifecycle. Sliders push into bus; dials read from snap.
	OutputSnapshot snap = OutputSnapshot::From(*reactorManager);
	InputBus bus;

	if (playConsole_) {
		playConsole_->Update(deltaTime, snap, bus);
		playConsole_->Draw();
	}

	DrawRevenue(reactorManager->GetRevenue());
	DrawTimer(gameManager);

	bus.ApplyTo(*reactorManager);
}

void CanvasManager::DrawSegmentDisplay(const std::string& text, Vector2 position, float fontSize, Color color, const std::string& label) {
	if (fontLoaded_) {
		DrawTextEx(sevenSegmentFont_, text.c_str(), position, fontSize, 2, color);
		DrawText(label.c_str(), position.x, position.y + fontSize + 5, 20, DARKGRAY);
	}
	else {
		DrawText(text.c_str(), position.x, position.y, fontSize, color);
	}
}


void CanvasManager::DrawRevenue(float revenue) {
	std::string revenueText = std::format("${:.2f}", revenue);
	int fontSize = 48;
	Color orangeColor = { 255, 140, 0, 255 };

	if (fontLoaded_) {
		Vector2 textSize = MeasureTextEx(sevenSegmentFont_, revenueText.c_str(), fontSize, 2);
		float xPos = VirtualScreen::Width() - textSize.x - 30;
		float yPos = 30;
		DrawSegmentDisplay(revenueText, { xPos, yPos }, fontSize, orangeColor, "REVENUE");
	}
	else {
		int textWidth = MeasureText(revenueText.c_str(), fontSize);
		float xPos = VirtualScreen::Width() - textWidth - 30;
		float yPos = 30;
		DrawText(revenueText.c_str(), xPos, yPos, fontSize, orangeColor);
	}
}

void CanvasManager::DrawTimer(GameManager* gameManager) {
	std::string timerText = gameManager->GetTimerFormatted();
	int fontSize = 48;
	Color redColor = { 220, 20, 20, 255 };

	// Top-center, inside the kTopBand reserved by the console viewport.
	const float yPos = 20.0f;

	if (fontLoaded_) {
		Vector2 textSize = MeasureTextEx(sevenSegmentFont_, timerText.c_str(), fontSize, 2);
		float xPos = (VirtualScreen::Width() - textSize.x) / 2.0f;
		DrawSegmentDisplay(timerText, { xPos, yPos }, fontSize, redColor, "ELAPSED TIME");
	}
	else {
		int textWidth = MeasureText(timerText.c_str(), fontSize);
		float xPos = (VirtualScreen::Width() - textWidth) / 2.0f;
		DrawText(timerText.c_str(), xPos, yPos, fontSize, redColor);
	}
}


void CanvasManager::RenderEndScreen(const GameStatistics& stats) {
	ClearBackground(Theme::kPanelBeige);

	// Title
	const char* title = stats.failureType == FailureType::MELTDOWN ? "MELTDOWN!" : "OVERPRESSURE!";
	int titleFontSize = 64;
	Color titleColor = {220, 20, 20, 255}; // Red
	int titleWidth = MeasureText(title, titleFontSize);
	DrawText(title, (VirtualScreen::Width() - titleWidth) / 2, 80, titleFontSize, titleColor);

	// Statistics
	int statsFontSize = 32;
	int lineSpacing = 50;
	int startY = 220;
	int centerX = VirtualScreen::Width() / 2;

	// Total Time
	int totalSeconds = static_cast<int>(stats.totalTime);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	std::string timeText = std::format("Total Time: {:02}:{:02}", minutes, seconds);
	int width = MeasureText(timeText.c_str(), statsFontSize);
	DrawText(timeText.c_str(), centerX - width / 2, startY, statsFontSize, DARKGRAY);

	// Revenue
	std::string revenueText = std::format("Revenue Earned: ${:.2f}", stats.revenue);
	width = MeasureText(revenueText.c_str(), statsFontSize);
	DrawText(revenueText.c_str(), centerX - width / 2, startY + lineSpacing, statsFontSize, DARKGRAY);

	// Highest Temperature (RED if caused meltdown)
	std::string tempText = std::format("Highest Temperature: {:.1f} C", stats.highestTemp);
	width = MeasureText(tempText.c_str(), statsFontSize);
	Color tempColor = stats.failureType == FailureType::MELTDOWN ? RED : DARKGRAY;
	DrawText(tempText.c_str(), centerX - width / 2, startY + lineSpacing * 2, statsFontSize, tempColor);

	// Highest Pressure (RED if caused kaboom)
	std::string pressureText = std::format("Highest Pressure: {:.1f} kPa", stats.highestPressure);
	width = MeasureText(pressureText.c_str(), statsFontSize);
	Color pressureColor = stats.failureType == FailureType::OVERPRESSURE ? RED : DARKGRAY;
	DrawText(pressureText.c_str(), centerX - width / 2, startY + lineSpacing * 3, statsFontSize, pressureColor);

	// Average Power Output
	std::string powerText = std::format("Average Power Output: {:.1f} MW", stats.averagePowerOutput);
	width = MeasureText(powerText.c_str(), statsFontSize);
	DrawText(powerText.c_str(), centerX - width / 2, startY + lineSpacing * 4, statsFontSize, DARKGRAY);

	// Prompt to restart
	const char* restartText = "Click to return to main menu";
	int restartFontSize = 24;
	int restartWidth = MeasureText(restartText, restartFontSize);
	DrawText(restartText, (VirtualScreen::Width() - restartWidth) / 2, VirtualScreen::Height() - 80, restartFontSize, GRAY);
}
