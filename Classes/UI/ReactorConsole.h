#ifndef REACTORSIM_REACTORCONSOLE_H
#define REACTORSIM_REACTORCONSOLE_H

#include "Console.h"

// Concrete control surface for the reactor sim. The constructor wires
// dial readers and slider writers against the SimIO contract and lays
// the panels out via the Layout:: helpers. This is the foundation
// placeholder — instrument set + arrangement will be expanded once the
// foundation lands. Resize is not yet plumbed: viewport is captured at
// construction time. (Layout() override is the future hook for that.)
class ReactorConsole : public Console {
public:
	explicit ReactorConsole(Rectangle viewport);
};

#endif //REACTORSIM_REACTORCONSOLE_H
