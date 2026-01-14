#include "Dial.h"
#include <algorithm>
#include <iostream>
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

void Dial::Update(float deltaTime) {
	// The Dial is a passive element; its state is updated externally via SetValue.
	// Update simply calls Draw to render it.
	Draw();
}

void Dial::Draw() {
	// Draw gauge outline with vintage colors
	float radius = size_.x > size_.y ? size_.y * 0.5f : size_.x * 0.5f;
	Vector2 center = {position_.x + radius, position_.y + radius};

	// Vintage control panel styling
	DrawCircle(center.x, center.y, radius, kVintageBrown);
	DrawCircle(center.x, center.y, radius * 0.95f, kVintageBeige);
	DrawCircle(center.x, center.y, radius * 0.9f, kVintageWhite);

	// Draw scale marks
	for (int i = 0; i <= 10; i++) {
		float angle = dialStartAngle_ + (i / 10.0f) * (dialEndAngle_ - dialStartAngle_);
		float markRadius = radius * 0.85f;
		float markEndRadius = radius * 0.8f;

		float radians = angle * PI / 180.0f;
		Vector2 markStart = {center.x + cos(radians) * markRadius, center.y + sin(radians) * markRadius};
		Vector2 markEnd = {center.x + cos(radians) * markEndRadius, center.y + sin(radians) * markEndRadius};

		DrawLineEx(markStart, markEnd, 2, kVintageBlack);
	}

	// Draw needle
	auto drawStart = Vector2(0, 0);
	float range = maxValue_ - minValue_;
	float percentage = range > 0 ? (currentValue_ - minValue_) / range : 0;
	float needleLength = radius * 0.75f;
	auto drawEnd = drawStart + Vector2(needleLength, 0.0f);

	// Rotate matrix for needle
	rlPushMatrix();
	rlTranslatef(center.x, center.y, 0.0f);
	float needleAngle = dialStartAngle_ + percentage * (dialEndAngle_ - dialStartAngle_);
	rlRotatef(needleAngle, 0, 0, 1);
	DrawLineEx(drawStart, drawEnd, 4, kVintageRed);
	rlPopMatrix();

	// Draw center dot
	DrawCircle(center.x, center.y, radius * 0.05f, kVintageBlack);

	// Draw label centered below the dial
	if (!label_.empty()) {
		int textWidth = MeasureText(label_.c_str(), kDefaultFontSize);
		int labelX = center.x - textWidth / 2;
		int labelY = position_.y + size_.y + kDefaultPadding;
		DrawText(label_.c_str(), labelX, labelY, kDefaultFontSize, kDefaultTextColor);
	}
}

void Dial::SetValue(float value) {
	currentValue_ = std::clamp(value, minValue_, maxValue_);
}

void Dial::SetLabel(const std::string& label) {
	this->label_ = label;
}
