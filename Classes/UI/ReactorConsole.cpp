#include "ReactorConsole.h"
#include "Panel.h"
#include "Layout.h"
#include "../UIElements/Dial.h"
#include "../UIElements/Handwheel.h"
#include "../UIElements/Lever.h"
#include "../SimIO/OutputSnapshot.h"
#include "../SimIO/InputBus.h"

ReactorConsole::ReactorConsole(Rectangle viewport) {
	SetViewport(viewport);

	// Two rows: instrumentation (top 60%), controls (bottom 40%).
	auto rows = Layout::SplitV(viewport, {3.0f, 2.0f}, 8.0f);

	// --- Top row: three semantic panels side-by-side ---
	auto panelCells = Layout::SplitH(rows[0], {1.0f, 1.0f, 1.0f}, 8.0f);

	// Helper: inset for dial area inside a panel (leaves room for panel chrome + dymo)
	auto dialArea = [](Rectangle cell) {
		return Layout::Inset(cell, 28.0f, 28.0f, 28.0f, 44.0f);
	};

	// ── Panel A: PRIMARY COOLANT ──────────────────────────────────
	{
		auto panel = std::make_unique<Panel>("PRIMARY COOLANT");
		panel->SetBounds(panelCells[0]);

		const Rectangle inner  = dialArea(panelCells[0]);
		const auto      cells  = Layout::SplitH(inner, {1.0f, 1.0f}, 10.0f);

		auto coolantDial = std::make_unique<Dial>(
			cells[0].x, cells[0].y, cells[0].width, cells[0].height,
			0.0f, 1000.0f, 0.0f, "COOLANT °C");
		coolantDial->SetReader([](const OutputSnapshot& s) { return s.coolantTemp; });
		coolantDial->SetZones(700.0f, 900.0f);
		panel->Add(std::move(coolantDial));

		auto pressDial = std::make_unique<Dial>(
			cells[1].x, cells[1].y, cells[1].width, cells[1].height,
			0.0f, 60.0f, 0.0f, "PRESS kPa");
		pressDial->SetReader([](const OutputSnapshot& s) { return s.reactorPressure; });
		pressDial->SetZones(40.0f, 54.0f);
		panel->Add(std::move(pressDial));

		Add(std::move(panel));
	}

	// ── Panel B: REACTOR CORE ─────────────────────────────────────
	{
		auto panel = std::make_unique<Panel>("REACTOR CORE");
		panel->SetBounds(panelCells[1]);

		const Rectangle inner = dialArea(panelCells[1]);
		const auto      cells = Layout::SplitH(inner, {1.0f, 1.0f}, 10.0f);

		auto tempDial = std::make_unique<Dial>(
			cells[0].x, cells[0].y, cells[0].width, cells[0].height,
			0.0f, 1000.0f, 0.0f, "TEMP °C");
		tempDial->SetReader([](const OutputSnapshot& s) { return s.reactorTemp; });
		tempDial->SetZones(700.0f, 900.0f);
		panel->Add(std::move(tempDial));

		auto powerDial = std::make_unique<Dial>(
			cells[1].x, cells[1].y, cells[1].width, cells[1].height,
			0.0f, 20000.0f, 0.0f, "POWER MW");
		powerDial->SetReader([](const OutputSnapshot& s) { return s.turbinePowerMW; });
		powerDial->SetZones(16000.0f, 19000.0f);
		panel->Add(std::move(powerDial));

		Add(std::move(panel));
	}

	// ── Panel C: TURBINE / GRID ───────────────────────────────────
	{
		auto panel = std::make_unique<Panel>("TURBINE / GRID");
		panel->SetBounds(panelCells[2]);

		const Rectangle inner = dialArea(panelCells[2]);
		// Single dial centered; future instruments will fill the right half.
		const auto cells = Layout::SplitH(inner, {1.0f, 1.0f}, 10.0f);

		auto rpmDial = std::make_unique<Dial>(
			cells[0].x, cells[0].y, cells[0].width, cells[0].height,
			0.0f, 3000.0f, 0.0f, "RPM");
		rpmDial->SetReader([](const OutputSnapshot& s) { return s.turbineRPM; });
		rpmDial->SetZones(2400.0f, 2800.0f);
		panel->Add(std::move(rpmDial));

		Add(std::move(panel));
	}

	// --- Bottom row: controls ----------------------------------------
	{
		auto controlPanel = std::make_unique<Panel>("CONTROLS");
		controlPanel->SetBounds(rows[1]);

		const Rectangle ctrlInner = Layout::Inset(rows[1], 24.0f, 24.0f, 24.0f, 40.0f);
		const auto      ctrlCells = Layout::SplitH(ctrlInner, {1.0f, 1.0f}, 12.0f);

		// Handwheel wants square-ish bounds; lever wants tall + narrow.
		const float wheelDim   = std::min(ctrlCells[0].width * 0.85f, ctrlCells[0].height * 0.95f);
		const Rectangle wheelSlot = Layout::Center(ctrlCells[0], wheelDim, wheelDim);
		const Rectangle leverSlot = Layout::Center(ctrlCells[1],
		                                           std::min(ctrlCells[1].width * 0.55f, 110.0f),
		                                           ctrlCells[1].height * 0.95f);

		auto coolantWheel = std::make_unique<Handwheel>(
			wheelSlot.x, wheelSlot.y, wheelSlot.width, wheelSlot.height,
			0.0f, "COOLANT VALVE");
		coolantWheel->SetWriter([](InputBus& bus, float v) { bus.SetCoolantValve(v); });
		controlPanel->Add(std::move(coolantWheel));

		auto rodLever = std::make_unique<Lever>(
			leverSlot.x, leverSlot.y, leverSlot.width, leverSlot.height,
			1.0f, "CONTROL RODS");
		rodLever->SetWriter([](InputBus& bus, float v) { bus.SetControlRod(v); });
		controlPanel->Add(std::move(rodLever));

		Add(std::move(controlPanel));
	}
}
