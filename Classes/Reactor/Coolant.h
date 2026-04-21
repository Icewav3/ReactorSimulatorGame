#ifndef CPP_PROJECTS_COOLANT_H
#define CPP_PROJECTS_COOLANT_H
#include "ICoolant.h"

/**
 * @class Coolant
 * @brief Simulates the coolant system of the reactor.
 * This class calculates the amount of heat transferred from the reactor core
 * based on the coolant flow rate, which is controlled by a valve.
 */
class Coolant : public ICoolant {
public:
	explicit Coolant(float maxFlow, float heatCapacity);

	/// Updates the coolant's state and calculates heat transfer.
	void Update(float deltaTime, float reactorTemp);

	/// Sets the coolant valve position, which determines the flow rate.
	void UpdateCoolantValve(float input);

	// --- Accessors ---
	float GetCoolantThermalTransfer() const;
	float GetCoolantFlowRate() const;
	float GetCoolantTempIn() const;
	float GetCoolantTempOut() const;
	float GetHeatExtracted() const;

private:
	/// Calculates the amount of heat to transfer from the reactor to the coolant.
	void CalculateThermalTransfer(float deltaTime, float reactorTemp);

	// Configuration
	float maxFlow_;
	float heatCapacity_;

	// State
	float flowRate_;
	float tempIn_;
	float tempOut_;
	float thermalTransfer_;
	float heatExtracted_; // Heat removed from reactor
};


#endif //CPP_PROJECTS_COOLANT_H
