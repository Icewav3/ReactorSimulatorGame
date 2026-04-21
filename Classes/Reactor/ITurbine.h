#ifndef CPP_PROJECTS_ITURBINE_H
#define CPP_PROJECTS_ITURBINE_H

class ITurbine {
public:
	virtual ~ITurbine() = default;

	virtual void Update(float deltaTime, float heatInput) = 0;
	virtual float GetRPM() const = 0;
	virtual float GetPowerOut() const = 0;
};

#endif //CPP_PROJECTS_ITURBINE_H