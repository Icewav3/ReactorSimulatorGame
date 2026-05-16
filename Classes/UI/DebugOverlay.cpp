#include "DebugOverlay.h"

#ifdef REACTOR_DEBUG

#include "VirtualScreen.h"
#include "Theme.h"
#include <cstdio>

namespace {
    bool enabled_ = false;
    constexpr Color kHitboxColor{255, 0, 0, 255};
    constexpr Color kCrosshairColor{0, 220, 220, 255};
}

void DebugOverlay::Update() {
    if (IsKeyPressed(KEY_F3)) enabled_ = !enabled_;
}

bool DebugOverlay::IsEnabled() { return enabled_; }

void DebugOverlay::DrawHitbox(Rectangle r) {
    if (!enabled_) return;
    DrawRectangleLinesEx(r, 1.5f, kHitboxColor);
}

void DebugOverlay::DrawHitboxRing(Vector2 center, float rInner, float rOuter) {
    if (!enabled_) return;
    DrawCircleLinesV(center, rInner, kHitboxColor);
    DrawCircleLinesV(center, rOuter, kHitboxColor);
}

void DebugOverlay::DrawHUD() {
    if (!enabled_) return;

    const Vector2 vm     = VirtualScreen::GetMouse();
    const Vector2 raw    = GetMousePosition();
    const Vector2 dpi    = GetWindowScaleDPI();
    const float   sw     = static_cast<float>(GetScreenWidth());
    const float   sh     = static_cast<float>(GetScreenHeight());
    const auto    layout = VirtualScreen::GetLayout();
    const float   scale  = layout.scale;
    const float   offX   = layout.offsetX;
    const float   offY   = layout.offsetY;

    DrawLineEx({vm.x - 10.0f, vm.y}, {vm.x + 10.0f, vm.y}, 1.5f, kCrosshairColor);
    DrawLineEx({vm.x, vm.y - 10.0f}, {vm.x, vm.y + 10.0f}, 1.5f, kCrosshairColor);
    DrawCircleV(vm, 2.0f, kCrosshairColor);

    char buf[512];
    snprintf(buf, sizeof(buf),
             "raw    %.1f, %.1f\n"
             "dpi    %.3f, %.3f\n"
             "screen %.0f x %.0f\n"
             "scale  %.4f\n"
             "offset %.1f, %.1f\n"
             "vmouse %.1f, %.1f",
             raw.x, raw.y, dpi.x, dpi.y, sw, sh, scale, offX, offY, vm.x, vm.y);

    const Font& f = Theme::OcrFont();
    constexpr float fs = 14.0f;
    const Vector2 pos{20.0f, 120.0f};
    const Vector2 sz = MeasureTextEx(f, buf, fs, 0.5f);
    DrawRectangle(static_cast<int>(pos.x - 6.0f),
                  static_cast<int>(pos.y - 4.0f),
                  static_cast<int>(sz.x + 12.0f),
                  static_cast<int>(sz.y + 8.0f),
                  {0, 0, 0, 180});
    DrawTextEx(f, buf, pos, fs, 0.5f, {255, 255, 255, 255});
}

#endif // REACTOR_DEBUG
