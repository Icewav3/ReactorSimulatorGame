#ifndef REACTORSIM_HANDWHEEL_H
#define REACTORSIM_HANDWHEEL_H

#include "Instrument.h"
#include <functional>
#include <string>

class InputBus;
struct OutputSnapshot;

// Rotary isolation-valve handwheel. 4 spokes + X-brace, red rim.
// Interaction is angular-delta from grab point (no click-jump), scaled
// for resistance and quantized to discrete detents on release.
class Handwheel : public Instrument {
public:
	Handwheel(float x, float y, float width, float height,
	          float initialValue, const std::string& label);

	void Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) override;
	void Draw() override;

	using Writer = std::function<void(InputBus&, float)>;
	void SetWriter(Writer w) { writer_ = std::move(w); }

	float GetValue() const { return currentValue_; }
	void  SetValue(float v);

private:
	float MouseAngleDeg() const;
	void  HandleInput();

	std::string label_;
	float currentValue_;       // 0..1

	// Drag state
	bool  isDragging_    = false;
	float grabAngleDeg_  = 0.0f;
	int   lastDetentIdx_ = -1;

	// Snap-on-release animation
	bool  isSnapping_ = false;
	float snapTarget_ = 0.0f;

	// Visual click feedback
	float detentFlashTime_ = 0.0f;

	// Tuning
	static constexpr int   kDetentCount         = 16;
	static constexpr float kFullSweepDeg        = 360.0f;
	static constexpr float kInputScale          = 0.5f;
	static constexpr float kMaxDeltaPerFrameDeg = 16.0f;
	static constexpr float kSnapRate            = 22.0f;
	static constexpr float kSnapEpsilon         = 0.0008f;

	Writer writer_;
};

#endif //REACTORSIM_HANDWHEEL_H
