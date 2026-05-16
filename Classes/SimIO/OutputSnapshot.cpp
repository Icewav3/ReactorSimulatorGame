#include "OutputSnapshot.h"
#include "../Reactor/ReactorManager.h"

OutputSnapshot OutputSnapshot::From(const ReactorManager& sim) {
	return OutputSnapshot{
		.reactorTemp     = sim.GetReactorTemp(),
		.reactorPressure = sim.GetReactorPressure(),
		.coolantTemp     = sim.GetCoolantTemp(),
		.turbineRPM      = sim.GetTurbineRPM(),
		.turbinePowerMW  = sim.GetTurbinePowerOut(),
		.revenue         = sim.GetRevenue(),
		.meltdown        = sim.IsMeltdown(),
		.kaboom          = sim.IsKaboom(),
	};
}
