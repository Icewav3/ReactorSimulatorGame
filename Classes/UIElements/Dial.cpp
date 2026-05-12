#include "Dial.h"
#include "../SimIO/OutputSnapshot.h"
#include "../UI/Theme.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <raymath.h>
#include <rlgl.h>

Dial::Dial(
	float xpos,
	float ypos,
	float length,
	float height,
	float min,
	float max,
	float value,
	const std::string& label)
	: Instrument(xpos, ypos, length, height),
	  minValue_(min),
	  maxValue_(max),
	  currentValue_(value),
	  label_(label) {
}

void Dial::Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) {
	(void)deltaTime;
	(void)bus;
	if (reader_) SetValue(reader_(snap));
}

void Dial::SetValue(float value) {
	currentValue_ = std::clamp(value, minValue_, maxValue_);
}

void Dial::SetLabel(const std::string& label) {
	label_ = label;
}

void Dial::SetZones(float warnValue, float dangerValue) {
	warningValue_ = warnValue;
	dangerValue_  = dangerValue;
}

void Dial::Draw() {
	// Reserve vertical space at the top of the bounding rect for the dymo label
	// strip, with a small fixed gap between the strip and the dial face. The
	// circle hugs the top of the remaining area so the label-to-face distance
	// stays constant regardless of how much slack the cell has.
	constexpr float kLabelReserve = 36.0f;
	constexpr float kLabelGap     = 6.0f;
	const float availH = std::max(size_.y - kLabelReserve - kLabelGap, 10.0f);
	const float radius = std::min(size_.x, availH) * 0.5f;
	const Vector2 center = {position_.x + size_.x * 0.5f,
	                        position_.y + kLabelReserve + kLabelGap + radius};

	// --- Bezel layers ---
	DrawCircleV(center, radius, Theme::kPanelBrown);
	DrawCircleV(center, radius * 0.95f, {20, 18, 15, 255});
	DrawCircleV(center, radius * 0.90f, Theme::kPanelCream);

	// --- Zone arcs (at 84% radius, ~6% thick) ---
	const float sweep   = dialEndAngle_ - dialStartAngle_; // 270°
	const float range   = maxValue_ - minValue_;
	const float arcOuter = radius * 0.87f;
	const float arcInner = radius * 0.80f;
	constexpr int kArcSeg = 40;

	auto valueToAngle = [&](float v) {
		return dialStartAngle_ + ((v - minValue_) / range) * sweep;
	};

	const float warnAngle = (warningValue_ >= 0.0f)
		? valueToAngle(warningValue_)
		: dialEndAngle_;
	const float dangAngle = (warningValue_ >= 0.0f && dangerValue_ >= 0.0f)
		? valueToAngle(dangerValue_)
		: dialEndAngle_;

	// Green zone: start → warning (or full range if no zones)
	DrawRing(center, arcInner, arcOuter,
	         dialStartAngle_, warnAngle, kArcSeg, Theme::kZoneGreen);

	if (warningValue_ >= 0.0f) {
		// Amber zone: warning → danger (or max)
		DrawRing(center, arcInner, arcOuter,
		         warnAngle, dangAngle, kArcSeg / 2, Theme::kZoneAmber);

		if (dangerValue_ >= 0.0f) {
			// Red zone: danger → max
			DrawRing(center, arcInner - 1.0f, arcOuter + 1.0f,
			         dangAngle, dialEndAngle_, kArcSeg / 4, Theme::kZoneRed);
		}
	}

	// --- Tick marks + numeric labels ---
	const Font& labelFont = Theme::OcrFont();
	const float labelFontSize = std::clamp(radius * 0.253f, 12.0f, 21.0f);
	constexpr int kMajorTicks = 10;

	for (int i = 0; i <= kMajorTicks; ++i) {
		const float t     = static_cast<float>(i) / kMajorTicks;
		const float angle = dialStartAngle_ + t * sweep;
		const float rad   = angle * (PI / 180.0f);
		const float cx    = cosf(rad);
		const float cy    = sinf(rad);

		// Major tick
		const Vector2 tickOuter = {center.x + cx * radius * 0.87f,
		                           center.y + cy * radius * 0.87f};
		const Vector2 tickInner = {center.x + cx * radius * 0.76f,
		                           center.y + cy * radius * 0.76f};
		DrawLineEx(tickOuter, tickInner, 2.0f, Theme::kPanelInk);

		// Numeric label
		const float labelVal = minValue_ + t * range;
		char buf[16];
		if (range >= 10000.0f)
			snprintf(buf, sizeof(buf), "%.0fk", labelVal / 1000.0f);
		else if (range >= 1000.0f && labelVal >= 1000.0f)
			snprintf(buf, sizeof(buf), "%.1fk", labelVal / 1000.0f);
		else
			snprintf(buf, sizeof(buf), "%.0f", labelVal);

		const float labelR = radius * 0.61f;
		const Vector2 rawPos = {center.x + cx * labelR, center.y + cy * labelR};
		const Vector2 textSz  = MeasureTextEx(labelFont, buf, labelFontSize, 0.0f);
		DrawTextEx(labelFont, buf,
		           {rawPos.x - textSz.x * 0.5f, rawPos.y - textSz.y * 0.5f},
		           labelFontSize, 0.0f, Theme::kPanelInk);
	}

	// Minor ticks (midpoint of each major interval)
	for (int i = 0; i < kMajorTicks; ++i) {
		const float t     = (static_cast<float>(i) + 0.5f) / kMajorTicks;
		const float angle = dialStartAngle_ + t * sweep;
		const float rad   = angle * (PI / 180.0f);
		const Vector2 mo  = {center.x + cosf(rad) * radius * 0.87f,
		                     center.y + sinf(rad) * radius * 0.87f};
		const Vector2 mi  = {center.x + cosf(rad) * radius * 0.81f,
		                     center.y + sinf(rad) * radius * 0.81f};
		DrawLineEx(mo, mi, 1.0f, {80, 70, 58, 180});
	}

	// --- Needle (filled triangle with counterweight tail) ---
	const float percentage  = range > 0.0f ? (currentValue_ - minValue_) / range : 0.0f;
	const float needleAngle = dialStartAngle_ + percentage * sweep;
	const float tipLen      = radius * 0.70f;
	const float tailLen     = radius * 0.18f;
	const float baseHalf    = radius * 0.035f;

	rlPushMatrix();
	rlTranslatef(center.x, center.y, 0.0f);
	rlRotatef(needleAngle, 0.0f, 0.0f, 1.0f);

	// DrawTriangle requires CCW winding in screen (Y-down) space.
	// Upper face: left → right → peak-up = CCW visually
	DrawTriangle({-tailLen, 0.0f}, {tipLen, 0.0f}, {0.0f, -baseHalf}, Theme::kPanelInk);
	// Lower face: left → peak-down → right = CCW visually
	DrawTriangle({-tailLen, 0.0f}, {0.0f, baseHalf}, {tipLen, 0.0f}, Theme::kPanelInk);

	rlPopMatrix();

	// --- Pivot ---
	DrawCircleV(center, radius * 0.065f, Theme::kPanelBrown);
	DrawCircleV(center, radius * 0.040f, Theme::kPanelInk);
	DrawCircleV(center, radius * 0.015f, Theme::kPanelCream); // highlight dot

	// --- Glass reflection (faint white ellipse, top-left quadrant) ---
	DrawCircleV({center.x - radius * 0.18f, center.y - radius * 0.22f},
	            radius * 0.48f, {255, 255, 255, 16});

	// --- Label dymo strip (centered above dial face) ---
	if (!label_.empty()) {
		const Font& dymoFont = Theme::BoldFont();
		const float kLabelSize = std::clamp(radius * 0.30f, 14.0f, 24.0f);
		const Vector2 lsz = MeasureTextEx(dymoFont, label_.c_str(), kLabelSize, 0.5f);
		const float lx = center.x - lsz.x * 0.5f;
		// Vertically center the text within the reserved top strip.
		const float ly = position_.y + (kLabelReserve - lsz.y) * 0.5f;
		const Rectangle strip{lx - 6.0f, ly - 3.0f, lsz.x + 12.0f, lsz.y + 6.0f};
		DrawRectangleRounded(strip, 0.3f, 4, Theme::kDymoTape);
		DrawTextEx(dymoFont, label_.c_str(), {lx, ly}, kLabelSize, 0.5f, Theme::kDymoText);
	}
}
