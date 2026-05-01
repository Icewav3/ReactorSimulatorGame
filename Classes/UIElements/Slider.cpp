#include "Classes/UIElements/Slider.h"
#include "Classes/SimIO/InputBus.h"
#include "Classes/UI/Theme.h"
#include "Classes/UI/VirtualScreen.h"
#include <cmath>

Slider::Slider(float x, float y, float width, float height,
                   float min_value, float max_value, float initial_value,
                   const std::string &label)
	: Instrument(x, y, width, height),
	  minValue_(min_value),
	  maxValue_(max_value),
	  currentValue_(std::clamp(initial_value, min_value, max_value)),
	  label_(label),
	  isDragging_(false),
	  isHovered_(false),
	  trackColor_(Theme::kPanelBeige),
	  handleColor_(Theme::kPanelGold),
	  activeColor_(Theme::kPanelOrange),
	  textColor_(Theme::kPanelInk) {
	AutoConfigureLayout();
}

void Slider::AutoConfigureLayout() {
	isHorizontal_ = size_.x > size_.y;
	const float minDim = std::min(size_.x, size_.y);

	trackThickness_ = std::max(4.0f, minDim * 0.2f);
	handleSize_     = std::max(12.0f, minDim * 0.8f);

	labelSpacing_ = kDefaultPadding;
	valueSpacing_ = kDefaultPadding;

	if (isHorizontal_) {
		const float track_y = position_.y + (size_.y - trackThickness_) * 0.5f;
		trackRect_ = {position_.x, track_y, size_.x, trackThickness_};
	} else {
		const float track_x = position_.x + (size_.x - trackThickness_) * 0.5f;
		trackRect_ = {track_x, position_.y, trackThickness_, size_.y};
	}

	if (!label_.empty()) {
		const int text_width = MeasureText(label_.c_str(), kDefaultFontSize);
		labelPosition_ = {
			position_.x + (size_.x - text_width) * 0.5f,
			position_.y - kDefaultFontSize - labelSpacing_
		};
	}
}

void Slider::Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) {
	(void)deltaTime;
	(void)snap;
	HandleInput();
	if (writer_) writer_(bus, currentValue_);
}

void Slider::HandleInput() {
	// Mouse must come from the virtual framebuffer transform — the raylib
	// GetMousePosition is in window pixels, which won't match our drawn
	// coordinates once the window is resized.
	Vector2 mouse_pos = VirtualScreen::GetMouse();

	// Hover/press hit area is the full slider bounds — clicking anywhere
	// on the track grabs the handle and jumps it to that position. The
	// handle-only hitbox was 16x16 which is too small to hit reliably.
	isHovered_ = CheckCollisionPointRec(mouse_pos, bounds_);

	if (isHovered_ && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		isDragging_ = true;
		SetValue(CalculateValueFromMouse(mouse_pos));
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		isDragging_ = false;
	}

	if (isDragging_) {
		SetValue(CalculateValueFromMouse(mouse_pos));
	}
}

void Slider::Draw() {
	DrawRectangleRounded(trackRect_, 0.5f, 8, trackColor_);
	DrawRectangleRoundedLines(trackRect_, 0.5f, 8, Theme::kPanelBrown);

	Rectangle handle_bounds = GetHandleBounds();
	Color handle_draw_color = isDragging_
		                          ? activeColor_
		                          : (isHovered_ ? ColorBrightness(handleColor_, 0.1f) : handleColor_);

	DrawRectangleRounded(handle_bounds, 0.3f, 8, handle_draw_color);
	DrawRectangleRoundedLines(handle_bounds, 0.3f, 8, Theme::kPanelBrown);

	if (!label_.empty()) {
		DrawText(label_.c_str(),
		         (int) labelPosition_.x, (int) labelPosition_.y,
		         kDefaultFontSize, textColor_);
	}

	char value_text[32];
	snprintf(value_text, sizeof(value_text), "%.1f", currentValue_);
	const int value_width = MeasureText(value_text, kDefaultFontSize - 2);

	Vector2 handle_pos = GetHandlePosition();
	Vector2 value_pos;

	if (isHorizontal_) {
		value_pos = {
			handle_pos.x - value_width * 0.5f,
			position_.y + size_.y + valueSpacing_
		};
	} else {
		value_pos = {
			position_.x + size_.x + valueSpacing_,
			handle_pos.y - (kDefaultFontSize - 2) * 0.5f
		};
	}

	DrawText(value_text, (int) value_pos.x, (int) value_pos.y,
	         kDefaultFontSize - 2, textColor_);
}

Vector2 Slider::GetHandlePosition() const {
	const float range = maxValue_ - minValue_;
	float normalized_value = range > 0 ? (currentValue_ - minValue_) / range : 0;
	normalized_value = std::clamp(normalized_value, 0.0f, 1.0f);

	if (isHorizontal_) {
		return {position_.x + normalized_value * size_.x, position_.y + size_.y * 0.5f};
	} else {
		return {position_.x + size_.x * 0.5f, position_.y + size_.y - (normalized_value * size_.y)};
	}
}

Rectangle Slider::GetHandleBounds() const {
	Vector2 handle_pos = GetHandlePosition();
	return {
		handle_pos.x - handleSize_ * 0.5f,
		handle_pos.y - handleSize_ * 0.5f,
		handleSize_,
		handleSize_
	};
}

float Slider::CalculateValueFromMouse(Vector2 mouse_pos) const {
	const float range = maxValue_ - minValue_;
	float normalized_pos;

	if (isHorizontal_) {
		normalized_pos = (mouse_pos.x - position_.x) / size_.x;
	} else {
		normalized_pos = 1.0f - (mouse_pos.y - position_.y) / size_.y;
	}

	return minValue_ + std::clamp(normalized_pos, 0.0f, 1.0f) * range;
}

void Slider::SetValue(float value) {
	currentValue_ = std::clamp(value, minValue_, maxValue_);
}

void Slider::SetRange(float min_val, float max_val) {
	minValue_ = min_val;
	maxValue_ = max_val;
	currentValue_ = std::clamp(currentValue_, minValue_, maxValue_);
}
