#include "Handwheel.h"
#include "../SimIO/InputBus.h"
#include "../UI/DebugOverlay.h"
#include "../UI/Theme.h"
#include "../UI/VirtualScreen.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <raymath.h>
#include <rlgl.h>

// Rim band — fraction of display radius that is "the rim" (and the only
// grabbable area). 1.0 = outer edge, kRimInner = inner edge of rim.
static constexpr float kRimInner    = 0.78f;
static constexpr float kRimMidEdge  = 0.92f;
static constexpr float kRimOuterDk  = 0.97f;
static constexpr float kRimGrabPad  = 0.05f;

Handwheel::Handwheel(float x, float y, float width, float height,
                     float initialValue, const std::string& label)
	: Instrument(x, y, width, height),
	  label_(label),
	  currentValue_(std::clamp(initialValue, 0.0f, 1.0f)),
	  initialValue_(std::clamp(initialValue, 0.0f, 1.0f)) {
}

void Handwheel::SetValue(float v) {
	currentValue_ = std::clamp(v, 0.0f, 1.0f);
}

void Handwheel::Reset() {
	currentValue_     = initialValue_;
	isDragging_       = false;
	grabAngleDeg_     = 0.0f;
	lastDetentIdx_    = -1;
	isSnapping_       = false;
	snapTarget_       = 0.0f;
	detentFlashTime_  = 0.0f;
}

float Handwheel::MouseAngleDeg() const {
	const Vector2 m = VirtualScreen::GetMouse();
	const Vector2 c = {position_.x + size_.x * 0.5f, position_.y + size_.y * 0.5f};
	return atan2f(m.y - c.y, m.x - c.x) * (180.0f / PI);
}

void Handwheel::HandleInput() {
	const float radius = std::min(size_.x, size_.y) * 0.5f * 0.78f;
	const Vector2 c    = {position_.x + size_.x * 0.5f, position_.y + size_.y * 0.5f};
	const Vector2 m    = VirtualScreen::GetMouse();
	const float dist   = Vector2Distance(m, c);

	const float rimMax = radius * (1.0f + kRimGrabPad);
	const float rimMin = radius * (kRimInner - kRimGrabPad);
	const bool overRim = dist >= rimMin && dist <= rimMax;

	if (overRim && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		isDragging_   = true;
		isSnapping_   = false;
		grabAngleDeg_ = MouseAngleDeg();
		lastDetentIdx_ = static_cast<int>(roundf(currentValue_ * kDetentCount));
	}
	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && isDragging_) {
		isDragging_ = false;
		const float step = 1.0f / static_cast<float>(kDetentCount);
		snapTarget_ = std::clamp(roundf(currentValue_ / step) * step, 0.0f, 1.0f);
		isSnapping_ = true;
	}

	if (isDragging_) {
		float deltaDeg = MouseAngleDeg() - grabAngleDeg_;
		while (deltaDeg >  180.0f) deltaDeg -= 360.0f;
		while (deltaDeg < -180.0f) deltaDeg += 360.0f;
		deltaDeg = std::clamp(deltaDeg, -kMaxDeltaPerFrameDeg, kMaxDeltaPerFrameDeg);
		grabAngleDeg_ += deltaDeg;

		const float valueDelta = (deltaDeg * kInputScale) / kFullSweepDeg;
		currentValue_ = std::clamp(currentValue_ + valueDelta, 0.0f, 1.0f);

		const int idx = static_cast<int>(roundf(currentValue_ * kDetentCount));
		if (idx != lastDetentIdx_) {
			lastDetentIdx_ = idx;
			detentFlashTime_ = 0.08f;
		}
	}
}

void Handwheel::Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) {
	(void)snap;
	HandleInput();

	if (isSnapping_ && !isDragging_) {
		const float diff = snapTarget_ - currentValue_;
		if (fabsf(diff) < kSnapEpsilon) {
			currentValue_ = snapTarget_;
			isSnapping_ = false;
		} else {
			const float t = std::clamp(deltaTime * kSnapRate, 0.0f, 1.0f);
			currentValue_ += diff * t;
		}
	}

	if (detentFlashTime_ > 0.0f)
		detentFlashTime_ = std::max(0.0f, detentFlashTime_ - deltaTime);

	if (writer_) writer_(bus, currentValue_);
}

void Handwheel::Draw() {
	const float radius = std::min(size_.x, size_.y) * 0.5f * 0.78f;
	const Vector2 c    = {position_.x + size_.x * 0.5f, position_.y + size_.y * 0.5f};

	// --- Position arrow + OPEN/CLOSED label above wheel ---
	{
		const Font& f = Theme::BoldFont();
		const char* tag = (currentValue_ > 0.5f) ? "OPEN" : "CLOSED";
		const Vector2 sz = MeasureTextEx(f, tag, 14.0f, 0.5f);
		const float ax = c.x;
		const float ay = c.y - radius - 22.0f;
		DrawTextEx(f, tag, {ax - sz.x * 0.5f, ay - sz.y - 4.0f}, 14.0f, 0.5f, Theme::kPanelInk);
		DrawTriangle({ax,        ay + 8.0f},
		             {ax + 5.0f, ay},
		             {ax - 5.0f, ay},
		             Theme::kPanelInk);
	}

	// --- Panel-fixed tick ring (orientation reference; doesn't rotate) ---
	{
		for (int i = 0; i < kDetentCount; ++i) {
			const float a = (i * 360.0f / kDetentCount) * (PI / 180.0f);
			const float ux = cosf(a), uy = sinf(a);
			const bool  major = (i % 4) == 0;
			const float rIn   = radius + 3.0f;
			const float rOut  = radius + (major ? 10.0f : 5.0f);
			const Color tc    = major ? Theme::kPanelInk : Color{60, 55, 50, 200};
			DrawLineEx({c.x + ux * rIn,  c.y + uy * rIn},
			           {c.x + ux * rOut, c.y + uy * rOut},
			           major ? 2.0f : 1.2f, tc);
		}
	}

	// One full turn over 0..1, no rest tilt → notch realigns with OPEN at 100%.
	const float angleDeg = currentValue_ * kFullSweepDeg;

	rlPushMatrix();
	rlTranslatef(c.x, c.y, 0.0f);
	rlRotatef(angleDeg, 0.0f, 0.0f, 1.0f);

	// --- Rim (rounded tube cross-section: dark→mid→lit→mid→dark) ---
	const Color rimDark  = {95, 24, 17, 255};
	const Color rimDark2 = {120, 32, 22, 255};
	const Color rimMid   = isDragging_ ? Color{220, 90, 55, 255}
	                                    : Color{170, 48, 33, 255};
	const Color rimLit   = isDragging_ ? Color{245, 130, 80, 255}
	                                    : Color{205, 78, 52, 255};
	const Color hollow   = Theme::kPanelBeige;

	// Outer edge → crown → inner edge, then hollow.
	DrawCircleV({0, 0}, radius,                rimDark);   // 1.00 outer shadow
	DrawCircleV({0, 0}, radius * kRimOuterDk,  rimMid);    // 0.97 outer slope
	DrawCircleV({0, 0}, radius * kRimMidEdge,  rimLit);    // 0.92 crown (brightest)
	DrawCircleV({0, 0}, radius * 0.87f,        rimMid);    // 0.87 inner slope
	DrawCircleV({0, 0}, radius * 0.83f,        rimDark2);  // 0.83 inner shadow lip
	DrawCircleV({0, 0}, radius * kRimInner,    hollow);    // 0.78 hollow

	// Ambient occlusion at the rim/hollow junction.
	const Color bevelShadow = {0, 0, 0, 55};
	DrawRing({0, 0}, radius * kRimInner - 2.5f, radius * kRimInner,
	         0.0f, 360.0f, 64, bevelShadow);

	// --- 4 spokes + 4 X-brace = 8 ---
	const float spokeHalf  = 4.5f;
	const float spokeOuter = radius * (kRimInner + 0.01f);
	const float spokeInner = 6.0f;
	const Color spokeColor  = rimMid;
	const Color spokeShadow = {0, 0, 0, 90};
	const float shadowOff   = 2.0f;

	auto drawSpokeAt = [&](float deg, Vector2 offset, Color col) {
		const float r  = deg * (PI / 180.0f);
		const float ux = cosf(r), uy = sinf(r);
		const float nx = -uy,     ny = ux;
		const Vector2 inA  = {ux * spokeInner - nx * spokeHalf + offset.x, uy * spokeInner - ny * spokeHalf + offset.y};
		const Vector2 inB  = {ux * spokeInner + nx * spokeHalf + offset.x, uy * spokeInner + ny * spokeHalf + offset.y};
		const Vector2 outA = {ux * spokeOuter - nx * spokeHalf + offset.x, uy * spokeOuter - ny * spokeHalf + offset.y};
		const Vector2 outB = {ux * spokeOuter + nx * spokeHalf + offset.x, uy * spokeOuter + ny * spokeHalf + offset.y};
		DrawTriangle(inA, inB,  outB, col);
		DrawTriangle(inA, outB, outA, col);
	};
	// Shadow pass first (offset down/right in unrotated local space — the
	// rotation matrix carries it with the wheel, which is fine; the cast is
	// "down-right relative to the spoke," reads as ambient occlusion).
	for (int i = 0; i < 8; ++i) drawSpokeAt(i * 45.0f, {shadowOff, shadowOff}, spokeShadow);
	for (int i = 0; i < 8; ++i) drawSpokeAt(i * 45.0f, {0, 0},                 spokeColor);

	// --- Hub (bakelite + metal cap) ---
	DrawCircleV({0, 0}, 13.0f, Theme::kPanelInk);
	DrawCircleV({0, 0}, 10.0f, {30, 28, 26, 255});
	DrawCircleV({0, 0},  4.5f, Theme::kRivetMetal);

	// Reference index notch on rim at local 12 o'clock (rotates with wheel)
	{
		const float r  = -90.0f * (PI / 180.0f);
		const float ux = cosf(r), uy = sinf(r);
		const float nx = -uy,     ny = ux;
		const float rOuter = radius * 0.99f;
		const float rInner = radius * kRimInner;
		const float w = 3.5f;
		const Vector2 inA  = {ux * rInner - nx * w, uy * rInner - ny * w};
		const Vector2 inB  = {ux * rInner + nx * w, uy * rInner + ny * w};
		const Vector2 outA = {ux * rOuter - nx * w, uy * rOuter - ny * w};
		const Vector2 outB = {ux * rOuter + nx * w, uy * rOuter + ny * w};
		DrawTriangle(inA, inB,  outB, Theme::kPanelCream);
		DrawTriangle(inA, outB, outA, Theme::kPanelCream);
	}

	rlPopMatrix();

	// --- Panel-fixed rim highlight (single light source upper-left) ---
	// Mirrors the dial's faint white reflection. 135°→225° in raylib's
	// Y-down/CW-positive angle space is the upper-left arc of the rim.
	DrawRing(c, radius * kRimMidEdge, radius * 0.995f,
	         200.0f, 340.0f, 48, {255, 240, 220, 28});

	// --- Grab glow ---
	if (isDragging_) {
		DrawRing(c, radius + 11.0f, radius + 14.0f, 0.0f, 360.0f, 48,
		         {255, 170, 90, 110});
	}

	// --- Detent click flash ---
	if (detentFlashTime_ > 0.0f) {
		const float a = std::clamp(detentFlashTime_ / 0.08f, 0.0f, 1.0f);
		DrawRing(c, radius + 1.0f, radius + 4.0f, 0.0f, 360.0f, 48,
		         {255, 220, 120, static_cast<unsigned char>(200 * a)});
	}

	// --- Position readout ---
	{
		const Font& f = Theme::OcrFont();
		char buf[32];
		snprintf(buf, sizeof(buf), "POS %3.0f%%", currentValue_ * 100.0f);
		const Vector2 sz = MeasureTextEx(f, buf, 15.0f, 0.5f);
		DrawTextEx(f, buf,
		           {c.x - sz.x * 0.5f, c.y + radius + 16.0f},
		           15.0f, 0.5f, Theme::kPanelInk);
	}

	// --- Dymo label ---
	if (!label_.empty()) {
		const Font& f = Theme::BoldFont();
		const float fs = 16.0f;
		const Vector2 sz = MeasureTextEx(f, label_.c_str(), fs, 0.5f);
		const float lx = c.x - sz.x * 0.5f;
		const float ly = c.y + radius + 36.0f;
		const Rectangle strip{lx - 6.0f, ly - 2.0f, sz.x + 12.0f, sz.y + 4.0f};
		DrawRectangleRounded(strip, 0.3f, 4, Theme::kDymoTape);
		DrawTextEx(f, label_.c_str(), {lx, ly}, fs, 0.5f, Theme::kDymoText);
	}

	// Must match HandleInput's rimMin/rimMax — keep in sync.
	DebugOverlay::DrawHitboxRing(c,
	                             radius * (kRimInner - kRimGrabPad),
	                             radius * (1.0f + kRimGrabPad));
}
