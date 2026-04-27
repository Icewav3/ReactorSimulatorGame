#ifndef REACTORSIM_CONSOLE_H
#define REACTORSIM_CONSOLE_H

#include "raylib.h"
#include <memory>
#include <vector>

class Panel;
struct OutputSnapshot;
class InputBus;

// Console = the entire control surface. Owns Panels, owns the viewport,
// drives Update + Draw. The "tree" in Architecture A is just this two-level
// composition (Console -> Panel -> Instrument); deeper nesting was rejected
// as over-engineering for ~50 widgets.
//
// Layout strategy: subclass and override Layout() to assign panel bounds
// using the Layout:: rect helpers. SetViewport() re-runs Layout() so a
// resize is one call.
class Console {
public:
	Console();
	virtual ~Console();

	// Adds a panel and returns a non-owning pointer for caller-side wiring.
	Panel* Add(std::unique_ptr<Panel> panel);

	void SetViewport(Rectangle viewport);
	Rectangle GetViewport() const { return viewport_; }

	void Update(float dt, const OutputSnapshot& snap, InputBus& bus);
	void Draw();

protected:
	// Override to assign bounds to each panel based on viewport_.
	// Default is a no-op (panels keep whatever bounds they were given).
	virtual void Layout() {}

	Rectangle viewport_{};
	std::vector<std::unique_ptr<Panel>> panels_;
};

#endif //REACTORSIM_CONSOLE_H
