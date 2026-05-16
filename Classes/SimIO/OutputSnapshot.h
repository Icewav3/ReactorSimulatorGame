#ifndef REACTORSIM_OUTPUTSNAPSHOT_H
#define REACTORSIM_OUTPUTSNAPSHOT_H

// Frame-stable, read-only view of the reactor sim, built once per frame.
// The UI reads from an OutputSnapshot instead of querying ReactorManager
// directly, so every widget in a frame sees the same picture and the sim
// stays headless-testable. Pairs with InputBus (the write side).

class ReactorManager;

struct OutputSnapshot {
	float reactorTemp    = 0.0f;
	float reactorPressure = 0.0f;
	float coolantTemp    = 0.0f;
	float turbineRPM     = 0.0f;
	float turbinePowerMW = 0.0f;
	float revenue        = 0.0f;
	bool  meltdown       = false;
	bool  kaboom         = false;

	static OutputSnapshot From(const ReactorManager& sim);
};

#endif //REACTORSIM_OUTPUTSNAPSHOT_H
