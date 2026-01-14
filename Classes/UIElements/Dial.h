#ifndef CPP_PROJECTS_DIAL_H
#define CPP_PROJECTS_DIAL_H

#include "Instrument.h"
#include <string>
#include "raylib.h"

/**
 * @class Dial
 * @brief A UI element representing a circular gauge or dial.
 * It visualizes a value within a specified range using a rotating needle.
 */
class Dial : public Instrument {
public:
	Dial(
		float xpos,
		float ypos,
		float length,
		float height,
		float min,
		float max,
		float value,
		const std::string& label = "");

	void Update(float deltaTime) override;

	void SetValue(float value);
	void SetLabel(const std::string& label);

private:
	void Draw() override;

	float minValue_;
	float maxValue_;
	float currentValue_;
	std::string label_;

	// Visual parameters defining the 270-degree arc of the dial.
	const float dialStartAngle_ = -225.0f; // Bottom-left
	const float dialEndAngle_ = 45.0f;   // Bottom-right
};

#endif //CPP_PROJECTS_DIAL_H
