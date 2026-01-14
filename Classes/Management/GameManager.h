#ifndef CPP_PROJECTS_GAMEMANAGER_H
#define CPP_PROJECTS_GAMEMANAGER_H

#include <string>

class ReactorManager;

/// Defines the different scenes or states of the game.
enum class GameScene {
	MAIN_MENU,
	INTRO_SEQUENCE,
	PLAYING,
	END_SCREEN
};

/// Defines the possible ways a game can end in failure.
enum class FailureType {
	NONE,
	MELTDOWN,
	OVERPRESSURE
};

/// Holds all statistical data for a single game session.
struct GameStatistics {
	float totalTime = 0.0f;
	float revenue = 0.0f;
	float highestTemp = 0.0f;
	float highestPressure = 0.0f;
	float averagePowerOutput = 0.0f;
	FailureType failureType = FailureType::NONE;
};

/**
 * @class GameManager
 * @brief Manages the overall game state, including scene transitions, timing, and statistics.
 */
class GameManager {
public:
	GameManager();

	/// Updates the game state, checks for win/loss conditions, and tracks statistics.
	void Update(float deltaTime, ReactorManager* reactorManager);

	/// Resets the game to its initial state.
	void Reset();

	// Scene management
	GameScene GetCurrentScene() const;
	void SetScene(GameScene scene);

	// Timer methods
	float GetElapsedTime() const;
	bool IsTimerRunning() const;
	/// Returns the elapsed time as a formatted string (MM:SS).
	std::string GetTimerFormatted() const;

	// Statistics
	const GameStatistics& GetStatistics() const;

private:
	/// Updates statistics during gameplay.
	void UpdateStatistics(ReactorManager* reactorManager, float deltaTime);
	/// Finalizes statistics at the end of a game.
	void FinalizeStatistics(ReactorManager* reactorManager);

	GameScene currentScene_;
	float elapsedTime_;
	bool timerRunning_;
	const float POWER_THRESHOLD_ = 100.0f; // Power output in MW to start the timer.

	// Statistics tracking
	GameStatistics stats_;
	float totalPowerAccumulated_;
	int powerSampleCount_;
};

#endif //CPP_PROJECTS_GAMEMANAGER_H
