#ifndef CPP_PROJECTS_TURBINE_H
#define CPP_PROJECTS_TURBINE_H
#include "ITurbine.h"

/**
 * @class Turbine
 * @brief Simulates a power-generating turbine.
 * This class converts thermal energy (heat) from the coolant system into rotational speed (RPM)
 * and then into electrical power output.
 */
class Turbine : public ITurbine{
public:
	Turbine(int maxRPM, int maxElectricityOut, float rpmToElectricCharge, float minRPMToGenerate);

	/// Updates the turbine's state based on heat input.
	void Update(float deltaTime, float heatInput);

	// --- Accessors ---
	float GetRPM() const;
	float GetPowerOut() const;

private:
	/// Calculates the turbine's RPM based on heat input.
	void CalculateRPM(float deltaTime);
	/// Calculates the power output based on the current RPM.
	void CalculatePowerOut();

	// Configuration
	int maxRPM_;
	float minRPMToGenerate_;
	int maxElectricityOut_;
	float rpmToElectricCharge_;

	// State
	float rpm_;
	float powerOut_;
	float currentHeatInput_;
};


#endif //CPP_PROJECTS_TURBINE_H
