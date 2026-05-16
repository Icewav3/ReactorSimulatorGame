#ifndef REACTORSIM_INPUTBUS_H
#define REACTORSIM_INPUTBUS_H

#include <optional>

// Frame-buffered write side of the SimIO contract. Widgets queue intents
// here during Update; ApplyTo(ReactorManager&) drains the queue at the
// frame boundary so the sim sees one atomic batch of inputs per tick.
// UI code never touches ReactorManager directly. Pairs with OutputSnapshot.

class ReactorManager;

class InputBus {
public:
	void SetControlRod(float position);
	void SetCoolantValve(float position);
	void RequestScram();

	void ApplyTo(ReactorManager& sim);
	void Clear();

private:
	std::optional<float> controlRod_;
	std::optional<float> coolantValve_;
	bool                 scram_ = false;
};

#endif //REACTORSIM_INPUTBUS_H
