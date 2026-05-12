#pragma once
#include "raylib.h"

// F3 toggles hitbox outlines + mouse/coord HUD. Gated on REACTOR_DEBUG
// (set for Debug configs in CMakeLists.txt). Release builds get inline
// no-op stubs so call sites compile away entirely.
namespace DebugOverlay {
#ifdef REACTOR_DEBUG
    void Update();
    void DrawHUD();
    void DrawHitbox(Rectangle r);
    void DrawHitboxRing(Vector2 center, float rInner, float rOuter);
    bool IsEnabled();
#else
    inline void Update() {}
    inline void DrawHUD() {}
    inline void DrawHitbox(Rectangle) {}
    inline void DrawHitboxRing(Vector2, float, float) {}
    inline bool IsEnabled() { return false; }
#endif
}
