#include "Panel.h"
#include "Theme.h"
#include "../UIElements/Instrument.h"

Panel::Panel() = default;
Panel::Panel(std::string label) : label_(std::move(label)) {}
Panel::~Panel() = default;

Instrument* Panel::Add(std::unique_ptr<Instrument> instrument) {
	Instrument* raw = instrument.get();
	instruments_.push_back(std::move(instrument));
	return raw;
}

void Panel::SetBounds(Rectangle bounds) {
	bounds_ = bounds;
	Relayout();
}

void Panel::Update(float dt, const OutputSnapshot& snap, InputBus& bus) {
	for (auto& inst : instruments_) {
		inst->Update(dt, snap, bus);
	}
}

void Panel::Reset() {
	for (auto& inst : instruments_) {
		inst->Reset();
	}
}

void Panel::Draw() {
	// Chrome first so instruments paint on top of the bezel.
	Theme::DrawPanelChrome(bounds_);
	if (!label_.empty()) {
		// Auto-size the dymo label tape to the text width plus a margin so
		// long labels like "INSTRUMENTATION" aren't clipped by a fixed box.
		const float textWidth = MeasureTextEx(Theme::BoldFont(), label_.c_str(),
		                                          static_cast<float>(Theme::kFontSizeDymo), 0.5f).x;
		const float w = textWidth + 24.0f;
		const float h = 22.0f;
		Rectangle labelRect{
			bounds_.x + (bounds_.width - w) * 0.5f,
			bounds_.y + bounds_.height - h - 6.0f,
			w,
			h,
		};
		Theme::DrawDymoLabel(labelRect, label_.c_str());
	}
	for (auto& inst : instruments_) {
		inst->Draw();
	}
}
