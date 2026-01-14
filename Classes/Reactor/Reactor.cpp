#include "Reactor.h"
#include <algorithm>
#include <cstdlib>

namespace {
	// Simulation constants for the reactor physics model.
	constexpr float ROD_SPEED = 0.25f;          // Speed at which control rods move.
	constexpr float REACTIVITY_SCALE = 2.0f;      // Multiplier for reactivity effect.
	constexpr float REACTIVITY_OFFSET = 0.7f;     // Base reactivity level.
	constexpr float MIN_HEAT_OUTPUT = 0.1f;       // Minimum heat output to prevent decay to zero.
	constexpr float AMBIENT_TEMP = 20.0f;         // Ambient temperature of the environment.
	constexpr float PRESSURE_CONSTANT = 0.1f;     // Factor for temperature-to-pressure conversion.
	constexpr float INITIAL_PRESSURE = 1.0f;      // Base pressure at ambient temperature.
}

Reactor::Reactor(float maxTemp, float maxPressure, float maxHeatOutput)
	: maxHeatOutput_(maxHeatOutput)
	  , maxTemp_(maxTemp)
	  , maxPressure_(maxPressure)
	  , controlRodPosition_(1.0f)
	  , targetControlRodPosition_(1.0f)
	  , heatOutput_(MIN_HEAT_OUTPUT)
	  , currentTemp_(AMBIENT_TEMP)
	  , currentPressure_(INITIAL_PRESSURE)
	  , currentHeatDelta_(0.0f)
	  , meltdown_(false)
	  , kaboom_(false) {
}

void Reactor::Update(float deltaTime) {
	// Simulate control rod movement lag.
	float positionDifference = targetControlRodPosition_ - controlRodPosition_;
	controlRodPosition_ += positionDifference * ROD_SPEED * deltaTime;

	// Calculate reactivity based on control rod position. Lowering rods (position -> 0) increases reactivity.
	float reactivity = REACTIVITY_OFFSET - controlRodPosition_;
	float changeInHeat = heatOutput_ * reactivity * REACTIVITY_SCALE * deltaTime;

	heatOutput_ += changeInHeat;
	heatOutput_ = std::clamp(heatOutput_, MIN_HEAT_OUTPUT, maxHeatOutput_);

	// Add heat from fission to the reactor core.
	currentTemp_ += heatOutput_ * deltaTime;

	// A simple pressure calculation based on temperature.
	currentPressure_ = INITIAL_PRESSURE + (currentTemp_ - AMBIENT_TEMP) * PRESSURE_CONSTANT;

	// Check for failure conditions.
	if (currentTemp_ > maxTemp_) {
		meltdown_ = true;
	}
	if (currentPressure_ > maxPressure_) {
		kaboom_ = true;
	}
}

void Reactor::UpdateControlRodPosition(float position) {
	targetControlRodPosition_ = position;
}

float Reactor::GetPressure() const {
	return currentPressure_;
}

float Reactor::GetTemp() const {
	return currentTemp_;
}

float Reactor::GetHeatOutput() const {
	return heatOutput_;
}

void Reactor::RemoveHeat(float heatAmount) {
	// Coolant removes heat from the reactor.
	currentTemp_ -= heatAmount;
	// Temperature cannot fall below ambient.
	if (currentTemp_ < AMBIENT_TEMP) {
		currentTemp_ = AMBIENT_TEMP;
	}
}

bool Reactor::IsMeltdown() const {
	return meltdown_;
}

bool Reactor::IsKaboom() const {
	return kaboom_;
}
