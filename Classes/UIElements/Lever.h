#ifndef REACTORSIM_LEVER_H
#define REACTORSIM_LEVER_H

#include "Instrument.h"
#include <functional>
#include <string>

class InputBus;
struct OutputSnapshot;

// Vertical throw-lever (control rod handle). Only the handle is grabbable;
// clicking the slot does nothing. While dragged, mouse Y delta is scaled
// (<1) so the lever lags the cursor — gives the player the feel of pushing
// physical mass.
class Lever : public Instrument {
public:
	Lever(float x, float y, float width, float height,
	      float initialValue, const std::string& label);

	void Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) override;
	void Draw() override;
	void Reset() override;

	using Writer = std::function<void(InputBus&, float)>;
	void SetWriter(Writer w) { writer_ = std::move(w); }

	float GetValue() const { return currentValue_; }
	void  SetValue(float v);

private:
	void HandleInput();
	Rectangle SlotRect() const;       // visible track
	Rectangle HandleRect() const;     // grabbable rect at current value
	float HandleCenterY() const;

	std::string label_;
	float currentValue_;             // 0..1; 1 = fully withdrawn (top)
	float initialValue_;

	// Drag state
	bool  isDragging_  = false;
	float grabMouseY_  = 0.0f;
	float grabValue_   = 0.0f;
	bool  pastLatch_   = false;       // first-frame friction released

	// Tuning
	static constexpr float kInputScale = 0.6f;   // weighted: lever<mouse
	static constexpr float kLatchPx    = 3.0f;   // first-move dead zone

	Writer writer_;
};

#endif //REACTORSIM_LEVER_H
