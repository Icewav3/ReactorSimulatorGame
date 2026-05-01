#ifndef REACTORSIM_VIRTUALSCREEN_H
#define REACTORSIM_VIRTUALSCREEN_H

#include "raylib.h"

// Fixed-resolution rendering with letterboxed scale-to-fit. All scene drawing
// targets a 1920x1080 RenderTexture; Present() blits it into the actual window
// preserving aspect ratio. Mouse input is transformed back into virtual space
// via GetMouse(). This sidesteps every raylib HIGHDPI bug (issues #1086,
// #1982, #4834, #5549) and is the same pattern as raylib's official
// core_window_letterbox example. Doubles as the portability layer for future
// Emscripten / mobile targets where the window size is the OS's problem.
namespace VirtualScreen {
    constexpr int kWidth = 1920;
    constexpr int kHeight = 1080;

    void Init();
    void Shutdown();

    // Bind the virtual framebuffer. Wrap all in-game drawing between Begin/End.
    void Begin();
    void End();

    // Blit the virtual framebuffer to the window with letterbox bars.
    void Present();

    // Mouse position in virtual (1920x1080) coordinates. Always read mouse
    // through this — never call GetMousePosition() directly in widgets.
    Vector2 GetMouse();

    constexpr int Width() { return kWidth; }
    constexpr int Height() { return kHeight; }
}

#endif // REACTORSIM_VIRTUALSCREEN_H
