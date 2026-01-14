#ifndef CPP_PROJECTS_REACTOR_H
#define CPP_PROJECTS_REACTOR_H

/**
 * @class Reactor
 * @brief Simulates the nuclear reactor core.
 * This class manages the reactor's temperature, pressure, and heat output based on the control rod positions.
 * It also tracks failure conditions like meltdowns or over-pressurization.
 */
class Reactor {
public:
	Reactor(float maxTemp, float maxPressure, float maxHeatOutput);

	/// Updates the reactor's internal state over a time delta.
	void Update(float deltaTime);

	/// Sets the target position for the control rods.
	void UpdateControlRodPosition(float position);

	/// Removes a specified amount of heat, typically by the coolant system.
	void RemoveHeat(float heatAmount);

	// --- Accessors ---
	float GetPressure() const;
	float GetTemp() const;
	float GetHeatOutput() const;
	bool IsMeltdown() const;
	bool IsKaboom() const;

private:
	// Configuration
	float maxHeatOutput_;
	float maxTemp_;
	float maxPressure_;

	// State
	float controlRodPosition_;
	float targetControlRodPosition_;
	float heatOutput_;
	float currentTemp_;
	float currentPressure_;
	float currentHeatDelta_; // Note: This member seems unused. Consider removing it.
	bool meltdown_;
	bool kaboom_;
};

#endif //CPP_PROJECTS_REACTOR_H
