#ifndef CPP_PROJECTS_DIAL_H
#define CPP_PROJECTS_DIAL_H

#include "Instrument.h"
#include <functional>
#include <optional>
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
	// Omit dangerValue (or pass nullopt) to skip the red zone.
	// Don't call SetZones at all to leave the full arc green.
	void SetZones(float warnValue, std::optional<float> dangerValue = std::nullopt);

private:

	float minValue_;
	float maxValue_;
	float currentValue_;
	float initialValue_;
	std::optional<float> warningValue_;
	std::optional<float> dangerValue_;
	std::string label_;
	Reader reader_;

	// Visual parameters defining the 270-degree arc of the dial.
	const float dialStartAngle_ = -225.0f; // Bottom-left
	const float dialEndAngle_ = 45.0f;   // Bottom-right
};

#endif //CPP_PROJECTS_DIAL_H
