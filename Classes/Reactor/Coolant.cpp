#include "Coolant.h"

#include <algorithm>

Coolant::Coolant(float maxFlow, float heatCapacity)
	: maxFlow_(maxFlow),
	  heatCapacity_(heatCapacity),
	  flowRate_(0.0f),
	  tempIn_(20.0f), // Assume coolant starts at ambient temperature
	  tempOut_(20.0f),
	  thermalTransfer_(0.0f),
	  heatExtracted_(0.0f) {
}

void Coolant::Update(float deltaTime, float reactorTemp) {
	CalculateThermalTransfer(deltaTime, reactorTemp);
}

void Coolant::UpdateCoolantValve(float input) {
	flowRate_ = input * maxFlow_;
}

float Coolant::GetCoolantThermalTransfer() const {
	return thermalTransfer_;
}

float Coolant::GetCoolantFlowRate() const {
	return flowRate_;
}

float Coolant::GetCoolantTempIn() const {
	return tempIn_;
}

float Coolant::GetCoolantTempOut() const {
	return tempOut_;
}

float Coolant::GetHeatExtracted() const {
	return heatExtracted_;
}

// Calculates the amount of heat to transfer from the reactor to the turbine.
void Coolant::CalculateThermalTransfer(float deltaTime, float reactorTemp) {
	if (flowRate_ > 0.0f && deltaTime > 0.0f) {
		// Temperature difference between reactor and coolant input
		float tempDiff = std::max(0.0f, reactorTemp - tempIn_);

		// Heat extracted from reactor per second
		heatExtracted_ = flowRate_ * heatCapacity_ * tempDiff * deltaTime;

		// Update coolant output temperature. Avoid division by zero.
		tempOut_ = tempIn_ + (heatExtracted_ / (flowRate_ * heatCapacity_ * deltaTime));

		// Thermal transfer to turbine is the same as the heat extracted from the reactor.
		thermalTransfer_ = heatExtracted_;
	} else {
		heatExtracted_ = 0.0f;
		thermalTransfer_ = 0.0f;
		tempOut_ = tempIn_;
	}
}
