#ifndef CPP_PROJECTS_DIAL_H
#define CPP_PROJECTS_DIAL_H

#include "Instrument.h"
#include <functional>
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

	void Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) override;
	void Draw() override;
	void Reset() override;

	using Reader = std::function<float(const OutputSnapshot&)>;
	void SetReader(Reader reader) { reader_ = std::move(reader); }

	void SetValue(float value);
	void SetLabel(const std::string& label);

	// warnValue / dangerValue are absolute values in [min, max].
	// Pass -1 to disable a zone. Defaults: both disabled.
	void SetZones(float warnValue, float dangerValue = -1.0f);

private:

	float minValue_;
	float maxValue_;
	float currentValue_;
	float initialValue_;
	float warningValue_ = -1.0f;
	float dangerValue_  = -1.0f;
	std::string label_;
	Reader reader_;

	// Visual parameters defining the 270-degree arc of the dial.
	const float dialStartAngle_ = -225.0f; // Bottom-left
	const float dialEndAngle_ = 45.0f;   // Bottom-right
};

#endif //CPP_PROJECTS_DIAL_H
