#ifndef CPP_PROJECTS_REACTORMANAGER_H
#define CPP_PROJECTS_REACTORMANAGER_H

#include "Reactor.h"
#include "Turbine.h"
#include "Coolant.h"
#include <memory>

/**
 * @class ReactorManager
 * @brief Orchestrates the reactor simulation by managing its core components.
 * This class owns the Reactor, Turbine, and Coolant objects and updates them
 * in the correct order to simulate the power generation process.
 */
class ReactorManager {
public:
	ReactorManager();

	// The destructor is no longer needed; smart pointers handle cleanup automatically.

	/// Updates all reactor components for a single frame.
	void Update(float deltaTime);

	// --- Accessors for UI ---
	float GetReactorTemp() const;
	float GetReactorPressure() const;
	float GetCoolantTemp() const;
	float GetTurbineRPM() const;
	float GetTurbinePowerOut() const;
	float GetRevenue() const;
	bool IsMeltdown() const;
	bool IsKaboom() const;

	// --- Input Methods ---
	void SetControlRodPosition(float position);
	void SetCoolantValve(float position);

private:
	// Components are managed by smart pointers for safe, automatic memory management.
	std::unique_ptr<IReactor> reactor_;
	std::unique_ptr<ITurbine> turbine_;
	std::unique_ptr<ICoolant> coolant_;

	// Member variables for tracking simulation state.
	float totalRevenue_;
	float revenueRate_; // Dollars per MW-second
};

#endif //CPP_PROJECTS_REACTORMANAGER_H
