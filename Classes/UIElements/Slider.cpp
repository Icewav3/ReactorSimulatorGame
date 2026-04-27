#include "Classes/UIElements/Slider.h"
#include "Classes/SimIO/InputBus.h"
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
	  isHovered_(false) {
	AutoConfigureLayout();
	SetTheme(kVintageYellow); // Vintage nuclear control panel theme
}

void Slider::AutoConfigureLayout() {
	// Determine orientation based on aspect ratio
	isHorizontal_ = size_.x > size_.y;

	// Auto-scale dimensions based on size
	float min_dimension = std::min(size_.x, size_.y);

	// Auto-configure track and handle sizes
	if (isHorizontal_) {
		trackThickness_ = std::max(4.0f, min_dimension * 0.2f);
		handleSize_ = std::max(12.0f, min_dimension * 0.8f);
	} else {
		trackThickness_ = std::max(4.0f, min_dimension * 0.2f);
		handleSize_ = std::max(12.0f, min_dimension * 0.8f);
	}

	// Auto-calculate spacing
	labelSpacing_ = kDefaultPadding;
	valueSpacing_ = kDefaultPadding;

	// Configure track rectangle
	if (isHorizontal_) {
		float track_y = position_.y + (size_.y - trackThickness_) * 0.5f;
		trackRect_ = {position_.x, track_y, size_.x, trackThickness_};
	} else {
		float track_x = position_.x + (size_.x - trackThickness_) * 0.5f;
		trackRect_ = {track_x, position_.y, trackThickness_, size_.y};
	}

	// Auto-position label
	if (!label_.empty()) {
		int text_width = MeasureText(label_.c_str(), kDefaultFontSize);
		if (isHorizontal_) {
			labelPosition_ = {
				position_.x + (size_.x - text_width) * 0.5f,
				position_.y - kDefaultFontSize - labelSpacing_
			};
		} else {
			labelPosition_ = {
				position_.x + (size_.x - text_width) * 0.5f,
				position_.y - kDefaultFontSize - labelSpacing_
			};
		}
	}
}

void Slider::SetTheme(Color primary_color) {
	// Vintage nuclear control panel theme
	trackColor_ = kVintageBeige;
	handleColor_ = kVintageYellow;
	activeColor_ = kVintageOrange;
	textColor_ = kDefaultTextColor;
}

void Slider::Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) {
	(void)deltaTime;
	(void)snap;
	HandleInput();
	if (writer_) writer_(bus, currentValue_);
}

void Slider::HandleInput() {
	Vector2 mouse_pos = GetMousePosition();

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
	// Draw track
	DrawRectangleRounded(trackRect_, 0.5f, 8, trackColor_);
	DrawRectangleRoundedLines(trackRect_, 0.5f, 8, kDefaultBorderColor);

	// Draw handle with smooth color transitions
	Rectangle handle_bounds = GetHandleBounds();
	Color handle_draw_color = isDragging_
		                          ? activeColor_
		                          : (isHovered_ ? ColorBrightness(handleColor_, 0.1f) : handleColor_);

	DrawRectangleRounded(handle_bounds, 0.3f, 8, handle_draw_color);
	DrawRectangleRoundedLines(handle_bounds, 0.3f, 8, kDefaultBorderColor);

	// Draw label if present
	if (!label_.empty()) {
		DrawText(label_.c_str(),
		         (int) labelPosition_.x, (int) labelPosition_.y,
		         kDefaultFontSize, textColor_);
	}

	// Auto-draw current value
	char value_text[32];
	snprintf(value_text, sizeof(value_text), "%.1f", currentValue_);
	int value_width = MeasureText(value_text, kDefaultFontSize - 2);

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
	float range = maxValue_ - minValue_;
	float normalized_value = range > 0 ? (currentValue_ - minValue_) / range : 0;
	normalized_value = std::clamp(normalized_value, 0.0f, 1.0f);

	if (isHorizontal_) {
		float handle_x = position_.x + normalized_value * size_.x;
		return {handle_x, position_.y + size_.y * 0.5f};
	} else {
		float handle_y = position_.y + size_.y - (normalized_value * size_.y);
		return {position_.x + size_.x * 0.5f, handle_y};
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

Rectangle Slider::GetTrackBounds() const {
	return trackRect_;
}

float Slider::CalculateValueFromMouse(Vector2 mouse_pos) const {
	float normalized_pos;
    float range = maxValue_ - minValue_;

	if (isHorizontal_) {
		float relative_x = mouse_pos.x - position_.x;
		normalized_pos = relative_x / size_.x;
	} else {
		float relative_y = mouse_pos.y - position_.y;
		normalized_pos = 1.0f - (relative_y / size_.y); // Inverted for intuitive control
	}

	normalized_pos = std::clamp(normalized_pos, 0.0f, 1.0f);
	return minValue_ + normalized_pos * range;
}

void Slider::SetValue(float value) {
	currentValue_ = std::clamp(value, minValue_, maxValue_);
}

void Slider::SetRange(float min_val, float max_val) {
	minValue_ = min_val;
	maxValue_ = max_val;
	currentValue_ = std::clamp(currentValue_, minValue_, maxValue_);
}
