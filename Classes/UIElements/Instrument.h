#ifndef CPP_PROJECTS_INSTRUMENT_H
#define CPP_PROJECTS_INSTRUMENT_H

#include "raylib.h"
#include <string>

struct OutputSnapshot;
class InputBus;

/**
 * @class Instrument
 * @brief An abstract base class for all UI elements in the simulation.
 * It defines a common interface for updating and drawing UI components,
 * and provides a shared set of vintage color and style constants.
 */
class Instrument {
public:
	virtual ~Instrument() = default;

	Instrument(float x, float y, float width, float height)
		: position_{x, y},
		  size_{width, height},
		  bounds_{x, y, width, height} {
	}

	// Reads from snap (frame-stable view of the sim) and writes intents into
	// bus (drained at frame boundary). Widgets must not touch ReactorManager
	// directly. Update is read/write only — drawing happens in Draw().
	virtual void Update(float deltaTime, const OutputSnapshot& snap, InputBus& bus) = 0;

	// Renders the widget. Called by Panel::Draw after panel chrome so the
	// widget paints on top of the bezel and face.
	virtual void Draw() = 0;

	// --- Getters ---
	Vector2 GetPosition() const { return position_; }
	Vector2 GetSize() const { return size_; }
	Rectangle GetBounds() const { return bounds_; }

	// --- Setters ---
	void SetPosition(float x, float y) {
		position_ = {x, y};
		bounds_.x = x;
		bounds_.y = y;
	}

protected:
	Vector2 position_;
	Vector2 size_;
	Rectangle bounds_;

	// Widget-metric defaults. Colors live in Theme — pull from there.
	static constexpr int   kDefaultFontSize = 12;
	static constexpr float kDefaultPadding  = 5.0f;
};

#endif //CPP_PROJECTS_INSTRUMENT_H
