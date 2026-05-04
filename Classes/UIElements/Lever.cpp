#include "Lever.h"
#include "../SimIO/InputBus.h"
#include "../UI/Theme.h"
#include "../UI/VirtualScreen.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

Lever::Lever(float x, float y, float width, float height,
             float initialValue, const std::string& label)
	: Instrument(x, y, width, height),
	  label_(label),
	  currentValue_(std::clamp(initialValue, 0.0f, 1.0f)) {
}

void Lever::SetValue(float v) {
	currentValue_ = std::clamp(v, 0.0f, 1.0f);
}

Rectangle Lever::SlotRect() const {
	const float slotW = 8.0f;
	return {position_.x + (size_.x - slotW) * 0.5f,
	        position_.y + 12.0f,
	        slotW,
	        size_.y - 48.0f};
}

float Lever::HandleCenterY() const {
	const Rectangle s = SlotRect();
	// value=1 → top of slot, value=0 → bottom of slot
	return s.y + (1.0f - currentValue_) * s.height;
}

Rectangle Lever::HandleRect() const {
	const float w = std::min(size_.x * 0.85f, 56.0f);
	const float h = 26.0f;
	const float cx = position_.x + size_.x * 0.5f;
	const float cy = HandleCenterY();
	return {cx - w * 0.5f, cy - h * 0.5f, w, h};
}

void Lever::HandleInput() {
	const Vector2 m = VirtualScreen::GetMouse();
	const Rectangle h = HandleRect();

	if (CheckCollisionPointRec(m, h) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		isDragging_ = true;
		grabMouseY_ = m.y;
		grabValue_  = currentValue_;
		pastLatch_  = false;
	}
	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		isDragging_ = false;
	}

	if (isDragging_) {
		float dy = m.y - grabMouseY_;
		if (!pastLatch_) {
			if (fabsf(dy) < kLatchPx) return; // still latched
			pastLatch_ = true;
			// Re-anchor past the latch dead-zone so motion is continuous.
			grabMouseY_ = m.y - (dy > 0.0f ? kLatchPx : -kLatchPx);
			dy = m.y - grabMouseY_;
		}
		const Rectangle s = SlotRect();
		const float valueDelta = -(dy * kInputScale) / s.height; // up = +
		currentValue_ = std::clamp(grabValue_ + valueDelta, 0.0f, 1.0f);
	}
}

void Lever::Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) {
	(void)deltaTime; (void)snap;
	HandleInput();
	if (writer_) writer_(bus, currentValue_);
}

void Lever::Draw() {
	const Rectangle slot = SlotRect();
	const float cx = position_.x + size_.x * 0.5f;

	// --- End labels (top: WITHDRAW, bottom: INSERT) ---
	{
		const Font& f = Theme::BoldFont();
		const float fs = 10.0f;
		const Vector2 sw = MeasureTextEx(f, "WITHDRAW", fs, 0.5f);
		const Vector2 si = MeasureTextEx(f, "INSERT",   fs, 0.5f);
		DrawTextEx(f, "WITHDRAW", {cx - sw.x * 0.5f, slot.y - fs - 2.0f},
		           fs, 0.5f, Theme::kPanelInk);
		DrawTextEx(f, "INSERT",   {cx - si.x * 0.5f, slot.y + slot.height + 2.0f},
		           fs, 0.5f, Theme::kPanelInk);
	}

	// --- Slot (recessed dark groove) ---
	DrawRectangleRec({slot.x - 1.0f, slot.y - 1.0f, slot.width + 2.0f, slot.height + 2.0f},
	                 Theme::kPanelBrown);
	DrawRectangleRec(slot, {25, 22, 18, 255});
	// inner highlight on left edge
	DrawRectangle(static_cast<int>(slot.x), static_cast<int>(slot.y),
	              1, static_cast<int>(slot.height), {0, 0, 0, 90});

	// --- Pivot bracket at base ---
	{
		const float bw = 36.0f, bh = 18.0f;
		const Rectangle base{cx - bw * 0.5f,
		                     slot.y + slot.height + 14.0f,
		                     bw, bh};
		DrawRectangleRec(base, Theme::kScrewMetal);
		DrawRectangleLinesEx(base, 1.0f, Theme::kPanelInk);
		Theme::DrawScrew({base.x + 4.0f,            base.y + 4.0f}, 2.0f);
		Theme::DrawScrew({base.x + base.width - 4.0f, base.y + 4.0f}, 2.0f);
	}

	// --- Handle ---
	const Rectangle h = HandleRect();
	const Color body  = isDragging_ ? Theme::kPanelOrange : Theme::kRivetMetal;
	const Color shadow{0, 0, 0, 80};

	// Drop shadow under handle
	DrawRectangleRounded({h.x + 2.0f, h.y + 3.0f, h.width, h.height}, 0.4f, 6, shadow);
	DrawRectangleRounded(h, 0.4f, 6, body);
	DrawRectangleRoundedLines(h, 0.4f, 6, Theme::kPanelInk);

	// Grip ridges (3 dark stripes across the handle)
	for (int i = 1; i <= 3; ++i) {
		const float rx = h.x + h.width * (i / 4.0f);
		DrawLineEx({rx, h.y + 4.0f}, {rx, h.y + h.height - 4.0f}, 1.5f,
		           {0, 0, 0, 110});
	}

	// Rust streak — small worn patch top-right of handle
	DrawCircleV({h.x + h.width - 6.0f, h.y + 5.0f}, 2.5f, {120, 60, 25, 140});

	// Center pin (visual pivot through handle to slot)
	DrawCircleV({cx, h.y + h.height * 0.5f}, 3.0f, Theme::kPanelInk);
	DrawCircleV({cx, h.y + h.height * 0.5f}, 1.5f, Theme::kPanelCream);

	// --- Value readout (right of handle) ---
	{
		const Font& f = Theme::OcrFont();
		char buf[24];
		snprintf(buf, sizeof(buf), "%3.0f%%", currentValue_ * 100.0f);
		const float fs = 12.0f;
		const Vector2 sz = MeasureTextEx(f, buf, fs, 0.5f);
		DrawTextEx(f, buf,
		           {h.x + h.width + 6.0f, h.y + h.height * 0.5f - sz.y * 0.5f},
		           fs, 0.5f, Theme::kPanelInk);
	}

	// --- Dymo label below pivot ---
	if (!label_.empty()) {
		const Font& f = Theme::BoldFont();
		const float fs = 12.0f;
		const Vector2 sz = MeasureTextEx(f, label_.c_str(), fs, 0.5f);
		const float lx = cx - sz.x * 0.5f;
		const float ly = position_.y + size_.y - 14.0f;
		const Rectangle strip{lx - 6.0f, ly - 2.0f, sz.x + 12.0f, sz.y + 4.0f};
		DrawRectangleRounded(strip, 0.3f, 4, Theme::kDymoTape);
		DrawTextEx(f, label_.c_str(), {lx, ly}, fs, 0.5f, Theme::kDymoText);
	}
}
