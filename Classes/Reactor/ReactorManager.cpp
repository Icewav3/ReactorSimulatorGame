#include "ReactorManager.h"
#include <memory>

ReactorManager::ReactorManager()
	: reactor_(std::make_unique<Reactor>(1000.0f, 60.0f, 5000.0f))
	  , turbine_(std::make_unique<Turbine>(3000, 10000, 2.0f, 300))
	  , coolant_(std::make_unique<Coolant>(100.0f, 0.1f))
	  , totalRevenue_(0.0f)
	  , revenueRate_(0.001f) { // $0.001 per MW-second
}

float ReactorManager::GetReactorTemp() const {
	return reactor_->GetTemp();
}

float ReactorManager::GetReactorPressure() const {
	return reactor_->GetPressure();
}

float ReactorManager::GetCoolantTemp() const {
	return coolant_->GetCoolantTempOut();
}

float ReactorManager::GetTurbineRPM() const {
	return turbine_->GetRPM();
}

float ReactorManager::GetTurbinePowerOut() const {
	return turbine_->GetPowerOut();
}

float ReactorManager::GetRevenue() const {
	return totalRevenue_;
}

bool ReactorManager::IsMeltdown() const {
	return reactor_->IsMeltdown();
}

bool ReactorManager::IsKaboom() const {
	return reactor_->IsKaboom();
}

void ReactorManager::SetControlRodPosition(float position) {
	reactor_->UpdateControlRodPosition(position);
}

void ReactorManager::SetCoolantValve(float position) {
	coolant_->UpdateCoolantValve(position);
}

void ReactorManager::Update(float deltaTime) {
	// The simulation loop follows the physical process of the power plant.
	// 1. Reactor generates heat through fission.
	reactor_->Update(deltaTime);

	// 2. Coolant calculates how much heat it can extract from the reactor.
	coolant_->Update(deltaTime, reactor_->GetTemp());

	// 3. The calculated amount of heat is removed from the reactor.
	float heatExtracted = coolant_->GetHeatExtracted();
	reactor_->RemoveHeat(heatExtracted);

	// 4. The extracted heat is transferred to the turbine.
	float thermalTransfer = coolant_->GetCoolantThermalTransfer();
	turbine_->Update(deltaTime, thermalTransfer);

	// 5. The turbine converts heat into power.
	float powerOutput = turbine_->GetPowerOut();

	// 6. Revenue is generated based on power output.
	totalRevenue_ += powerOutput * revenueRate_ * deltaTime;
}
