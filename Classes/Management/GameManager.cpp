#include "Classes/Management/GameManager.h"
#include "Classes/Reactor/ReactorManager.h"
#include <cstdio>
#include <algorithm>
#include <format> // For std::format (C++20)

GameManager::GameManager()
	: currentScene_(GameScene::MAIN_MENU)
	  , elapsedTime_(0.0f)
	  , timerRunning_(false)
	  , totalPowerAccumulated_(0.0f)
	  , powerSampleCount_(0) {
}

void GameManager::Update(float deltaTime, ReactorManager* reactorManager) {
	if (currentScene_ != GameScene::PLAYING) {
		return;
	}

	// Update statistics
	UpdateStatistics(reactorManager, deltaTime);

	// Check win/loss conditions
	if (reactorManager->IsMeltdown() || reactorManager->IsKaboom()) {
		timerRunning_ = false;
		FinalizeStatistics(reactorManager);
		currentScene_ = GameScene::END_SCREEN;
		return;
	}

	// Start timer when power exceeds threshold
	if (!timerRunning_ && reactorManager->GetTurbinePowerOut() > POWER_THRESHOLD_) {
		timerRunning_ = true;
	}

	// Update timer if running
	if (timerRunning_) {
		elapsedTime_ += deltaTime;
	}
}

void GameManager::UpdateStatistics(ReactorManager* reactorManager, float deltaTime) {
	// Track highest values
	stats_.highestTemp = std::max(stats_.highestTemp, reactorManager->GetReactorTemp());
	stats_.highestPressure = std::max(stats_.highestPressure, reactorManager->GetReactorPressure());

	// Accumulate power for average calculation
	float currentPower = reactorManager->GetTurbinePowerOut();
	totalPowerAccumulated_ += currentPower;
	powerSampleCount_++;
}

void GameManager::FinalizeStatistics(ReactorManager* reactorManager) {
	stats_.totalTime = elapsedTime_;
	stats_.revenue = reactorManager->GetRevenue();

	// Calculate average power
	if (powerSampleCount_ > 0) {
		stats_.averagePowerOutput = totalPowerAccumulated_ / powerSampleCount_;
	}

	// Determine failure type
	if (reactorManager->IsMeltdown()) {
		stats_.failureType = FailureType::MELTDOWN;
	} else if (reactorManager->IsKaboom()) {
		stats_.failureType = FailureType::OVERPRESSURE;
	}
}

void GameManager::Reset() {
	elapsedTime_ = 0.0f;
	timerRunning_ = false;
	totalPowerAccumulated_ = 0.0f;
	powerSampleCount_ = 0;
	stats_ = GameStatistics();
	currentScene_ = GameScene::MAIN_MENU;
}

GameScene GameManager::GetCurrentScene() const {
	return currentScene_;
}

void GameManager::SetScene(GameScene scene) {
	currentScene_ = scene;
}

float GameManager::GetElapsedTime() const {
	return elapsedTime_;
}

bool GameManager::IsTimerRunning() const {
	return timerRunning_;
}

std::string GameManager::GetTimerFormatted() const {
	int totalSeconds = static_cast<int>(elapsedTime_);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	return std::format("{:02}:{:02}", minutes, seconds);
}

const GameStatistics& GameManager::GetStatistics() const {
	return stats_;
}
