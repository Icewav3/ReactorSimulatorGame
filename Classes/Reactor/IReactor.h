#ifndef CPP_PROJECTS_IREACTOR_H
#define CPP_PROJECTS_IREACTOR_H

/**
 * @interface IReactor
 * @brief Interface for reactor implementations
 * This allows swapping physics models without changing dependent code
 */
class IReactor {
public:
	virtual ~IReactor() = default;

	// Update
	virtual void Update(float deltaTime) = 0;

	// Control
	virtual void UpdateControlRodPosition(float position) = 0;
	virtual void RemoveHeat(float heatAmount) = 0;

	// State queries
	virtual float GetPressure() const = 0;
	virtual float GetTemp() const = 0;
	virtual float GetHeatOutput() const = 0;
	virtual bool IsMeltdown() const = 0;
	virtual bool IsKaboom() const = 0;
};

#endif //CPP_PROJECTS_IREACTOR_H