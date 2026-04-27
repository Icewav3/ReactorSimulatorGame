#include "ReactorConsole.h"
#include "Panel.h"
#include "Layout.h"
#include "../UIElements/Dial.h"
#include "../UIElements/Slider.h"
#include "../SimIO/OutputSnapshot.h"
#include "../SimIO/InputBus.h"

ReactorConsole::ReactorConsole(Rectangle viewport) {
	SetViewport(viewport);

	// Two rows: instrumentation on top (60%), controls on bottom (40%).
	auto rows = Layout::SplitV(viewport, {3.0f, 2.0f}, 8.0f);

	// --- Top row: gauges ---
	auto gaugePanel = std::make_unique<Panel>("INSTRUMENTATION");
	gaugePanel->SetBounds(rows[0]);

	// Inset away from bezel + dymo label, then split into 4 columns.
	const Rectangle gaugeInner = Layout::Inset(rows[0], 24.0f, 24.0f, 24.0f, 40.0f);
	const auto gaugeCells = Layout::SplitH(gaugeInner, {1, 1, 1, 1}, 12.0f);

	auto tempDial = std::make_unique<Dial>(
		gaugeCells[0].x, gaugeCells[0].y, gaugeCells[0].width, gaugeCells[0].height,
		0.0f, 1000.0f, 0.0f, "TEMP C");
	tempDial->SetReader([](const OutputSnapshot& s) { return s.reactorTemp; });
	gaugePanel->Add(std::move(tempDial));

	auto rpmDial = std::make_unique<Dial>(
		gaugeCells[1].x, gaugeCells[1].y, gaugeCells[1].width, gaugeCells[1].height,
		0.0f, 3000.0f, 0.0f, "RPM");
	rpmDial->SetReader([](const OutputSnapshot& s) { return s.turbineRPM; });
	gaugePanel->Add(std::move(rpmDial));

	auto pressureDial = std::make_unique<Dial>(
		gaugeCells[2].x, gaugeCells[2].y, gaugeCells[2].width, gaugeCells[2].height,
		0.0f, 60.0f, 0.0f, "PRESSURE kPa");
	pressureDial->SetReader([](const OutputSnapshot& s) { return s.reactorPressure; });
	gaugePanel->Add(std::move(pressureDial));

	auto powerDial = std::make_unique<Dial>(
		gaugeCells[3].x, gaugeCells[3].y, gaugeCells[3].width, gaugeCells[3].height,
		0.0f, 20000.0f, 0.0f, "POWER MW");
	powerDial->SetReader([](const OutputSnapshot& s) { return s.turbinePowerMW; });
	gaugePanel->Add(std::move(powerDial));

	Add(std::move(gaugePanel));

	// --- Bottom row: controls ---
	auto controlPanel = std::make_unique<Panel>("CONTROLS");
	controlPanel->SetBounds(rows[1]);

	const Rectangle ctrlInner = Layout::Inset(rows[1], 24.0f, 24.0f, 24.0f, 40.0f);
	const auto ctrlCells = Layout::SplitH(ctrlInner, {1, 1}, 12.0f);

	// Vertical sliders are a fixed-thin track — center within the cell.
	const Rectangle coolantSlot = Layout::Center(ctrlCells[0], 20.0f, ctrlCells[0].height * 0.7f);
	const Rectangle rodSlot     = Layout::Center(ctrlCells[1], 20.0f, ctrlCells[1].height * 0.7f);

	auto coolantSlider = std::make_unique<Slider>(
		coolantSlot.x, coolantSlot.y, coolantSlot.width, coolantSlot.height,
		0.0f, 1.0f, 0.0f, "Coolant Valve");
	coolantSlider->SetWriter([](InputBus& bus, float v) { bus.SetCoolantValve(v); });
	controlPanel->Add(std::move(coolantSlider));

	auto rodSlider = std::make_unique<Slider>(
		rodSlot.x, rodSlot.y, rodSlot.width, rodSlot.height,
		0.0f, 1.0f, 1.0f, "Control Rods");
	rodSlider->SetWriter([](InputBus& bus, float v) { bus.SetControlRod(v); });
	controlPanel->Add(std::move(rodSlider));

	Add(std::move(controlPanel));
}
