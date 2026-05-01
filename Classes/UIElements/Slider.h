#ifndef SLIDER_H
#define SLIDER_H

#include "Instrument.h"
#include <algorithm>
#include <functional>
#include <string>

class Slider : public Instrument {
public:
	// Simplified constructor - auto-configures everything based on dimensions
	Slider(float x, float y, float width, float height,
	       float min_value = 0.0f, float max_value = 100.0f,
	       float initial_value = 0.0f, const std::string &label = "");

	void Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) override;
	void Draw() override;

	using Writer = std::function<void(InputBus&, float)>;
	void SetWriter(Writer writer) { writer_ = std::move(writer); }

	float GetValue() const { return currentValue_; }
	void SetValue(float value);
	void SetRange(float min_val, float max_val);
	void SetLabel(const std::string &label) { label_ = label; }

private:
	void HandleInput();
	void AutoConfigureLayout();
	Vector2 GetHandlePosition() const;
	Rectangle GetHandleBounds() const;

	float CalculateValueFromMouse(Vector2 mouse_pos) const;

	// Core values
	float minValue_;
	float maxValue_;
	float currentValue_;
	std::string label_;

	// Auto-configured layout
	bool isHorizontal_;
	float trackThickness_;
	float handleSize_;
	Rectangle trackRect_;
	Vector2 labelPosition_;

	// Colors (auto-themed)
	Color trackColor_;
	Color handleColor_;
	Color activeColor_;
	Color textColor_;

	// Input state
	bool isDragging_;
	bool isHovered_;

	// Auto-calculated spacing
	float labelSpacing_;
	float valueSpacing_;

	Writer writer_;
};

#endif // SLIDER_H
