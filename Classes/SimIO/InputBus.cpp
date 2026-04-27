#include "InputBus.h"
#include "../Reactor/ReactorManager.h"

void InputBus::SetControlRod(float position) {
	controlRod_ = position;
}

void InputBus::SetCoolantValve(float position) {
	coolantValve_ = position;
}

void InputBus::RequestScram() {
	scram_ = true;
}

void InputBus::ApplyTo(ReactorManager& sim) {
	// Scram wins over a same-frame rod request: fully insert rods.
	// TODO: route through a dedicated ReactorManager::Scram() once the sim exposes one.
	if (scram_) {
		sim.SetControlRodPosition(1.0f);
	} else if (controlRod_) {
		sim.SetControlRodPosition(*controlRod_);
	}

	if (coolantValve_) {
		sim.SetCoolantValve(*coolantValve_);
	}

	Clear();
}

void InputBus::Clear() {
	controlRod_.reset();
	coolantValve_.reset();
	scram_ = false;
}
