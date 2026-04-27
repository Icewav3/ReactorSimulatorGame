#ifndef REACTORSIM_OUTPUTSNAPSHOT_H
#define REACTORSIM_OUTPUTSNAPSHOT_H

// Frame-stable, read-only view of the reactor sim, built once per frame.
// The UI reads from an OutputSnapshot instead of querying ReactorManager
// directly, so every widget in a frame sees the same picture and the sim
// stays headless-testable. Pairs with InputBus (the write side).

class ReactorManager;

struct OutputSnapshot {
	float reactorTemp;
	float reactorPressure;
	float coolantTemp;
	float turbineRPM;
	float turbinePowerMW;
	float revenue;
	bool  meltdown;
	bool  kaboom;

	static OutputSnapshot From(const ReactorManager& sim);
};

#endif //REACTORSIM_OUTPUTSNAPSHOT_H
