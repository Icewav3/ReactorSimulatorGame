#include "Console.h"
#include "Panel.h"

Console::Console() = default;
Console::~Console() = default;

Panel* Console::Add(std::unique_ptr<Panel> panel) {
	Panel* raw = panel.get();
	panels_.push_back(std::move(panel));
	return raw;
}

void Console::SetViewport(Rectangle viewport) {
	viewport_ = viewport;
	Layout();
}

void Console::Update(float dt, const OutputSnapshot& snap, InputBus& bus) {
	for (auto& panel : panels_) {
		panel->Update(dt, snap, bus);
	}
}

void Console::Draw() {
	for (auto& panel : panels_) {
		panel->Draw();
	}
}
