#ifndef CPP_PROJECTS_UTIL_H
#define CPP_PROJECTS_UTIL_H
#include "raylib.h"
#include "UI/VirtualScreen.h"

// Percent-of-virtual-screen helpers. These resolve against the fixed virtual
// canvas so layouts stay stable across window resizes.
inline float PX(float percent) {
	return (percent / 100.0f) * VirtualScreen::Width();
}

inline float PY(float percent) {
	return (percent / 100.0f) * VirtualScreen::Height();
}
#endif //CPP_PROJECTS_UTIL_H
