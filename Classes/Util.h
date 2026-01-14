#ifndef CPP_PROJECTS_UTIL_H
#define CPP_PROJECTS_UTIL_H
#include "raylib.h"

inline float PX(float percent) {
	return (percent / 100.0f) * GetScreenWidth();
}

inline float PY(float percent) {
	return (percent / 100.0f) * GetScreenHeight();
}
#endif //CPP_PROJECTS_UTIL_H
