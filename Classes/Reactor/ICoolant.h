#ifndef CPP_PROJECTS_ICOOLANT_H
#define CPP_PROJECTS_ICOOLANT_H

class ICoolant {
public:
	virtual ~ICoolant() = default;

	virtual void Update(float deltaTime, float reactorTemp) = 0;
	virtual void UpdateCoolantValve(float input) = 0;

	virtual float GetCoolantThermalTransfer() const = 0;
	virtual float GetCoolantFlowRate() const = 0;
	virtual float GetCoolantTempIn() const = 0;
	virtual float GetCoolantTempOut() const = 0;
	virtual float GetHeatExtracted() const = 0;
};

#endif //CPP_PROJECTS_ICOOLANT_H