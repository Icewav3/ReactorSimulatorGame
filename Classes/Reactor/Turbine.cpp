#include "Turbine.h"
#include <algorithm>
#include <cmath>

Turbine::Turbine(int maxRPM, int maxElectricityOut, float rpmToElectricCharge, float minRPMToGenerate)
	: maxRPM_(maxRPM)
	  , maxElectricityOut_(maxElectricityOut)
	  , rpmToElectricCharge_(rpmToElectricCharge)
	  , minRPMToGenerate_(minRPMToGenerate)
	  , rpm_(0.0f)
	  , powerOut_(0.0f)
	  , currentHeatInput_(0.0f) {
}

void Turbine::Update(float deltaTime, float heatInput) {
	currentHeatInput_ = heatInput;
	CalculateRPM(deltaTime);
	CalculatePowerOut();
}

float Turbine::GetRPM() const {
	return rpm_;
}

float Turbine::GetPowerOut() const {
	return powerOut_;
}

void Turbine::CalculateRPM(float deltaTime) {
	// A simplified model where heat input directly drives turbine speed.
	float targetRPM = (currentHeatInput_ / 100.0f) * maxRPM_;
	targetRPM = std::min(targetRPM, static_cast<float>(maxRPM_));

	// Smooth RPM changes for a more gradual ramp up/down effect.
	const float responseRate = 2.0f;
	float rpmChange = (targetRPM - rpm_) * responseRate * deltaTime;
	rpm_ += rpmChange;
	rpm_ = std::clamp(rpm_, 0.0f, static_cast<float>(maxRPM_));
}

void Turbine::CalculatePowerOut() {
	if (rpm_ > minRPMToGenerate_) {
		// Convert RPM to electrical power output (MW).
		float rpmRatio = (rpm_ - minRPMToGenerate_) / (static_cast<float>(maxRPM_) - minRPMToGenerate_);
		powerOut_ = rpmRatio * maxElectricityOut_ * rpmToElectricCharge_;
		powerOut_ = std::clamp(powerOut_, 0.0f, static_cast<float>(maxElectricityOut_));
	} else {
		powerOut_ = 0.0f;
	}
}
