#ifndef REACTORSIM_PANEL_H
#define REACTORSIM_PANEL_H

#include "raylib.h"
#include <memory>
#include <string>
#include <vector>

class Instrument;
struct OutputSnapshot;
class InputBus;

// Panel = a rectangular region of the console that owns a group of
// Instruments. Grouping is aesthetic-not-structural per Architecture A:
// a panel is "what would have a single bezel and dymo label in real life,"
// not a logical subsystem. Layout of children is the panel's responsibility
// (override Relayout to position via Layout:: helpers when bounds change).
class Panel {
public:
	Panel();
	explicit Panel(std::string label);
	virtual ~Panel();

	// Adds an instrument and returns a non-owning pointer for caller-side wiring.
	Instrument* Add(std::unique_ptr<Instrument> instrument);

	void SetBounds(Rectangle bounds);
	Rectangle GetBounds() const { return bounds_; }

	const std::string& GetLabel() const { return label_; }

	void Update(float dt, const OutputSnapshot& snap, InputBus& bus);
	void Draw();

protected:
	// Override to position child instruments when bounds_ changes.
	// Default is a no-op (children keep whatever bounds they were built with).
	virtual void Relayout() {}

	Rectangle   bounds_{};
	std::string label_;
	std::vector<std::unique_ptr<Instrument>> instruments_;
};

#endif //REACTORSIM_PANEL_H
