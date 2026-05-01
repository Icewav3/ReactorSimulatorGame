#include "VirtualScreen.h"
#include <algorithm>

namespace {
    RenderTexture2D target_{};
    bool initialized_ = false;

    struct Layout {
        float scale;
        float offsetX;
        float offsetY;
    };

    Layout ComputeLayout() {
        const float sw = static_cast<float>(GetScreenWidth());
        const float sh = static_cast<float>(GetScreenHeight());
        const float scale = std::min(sw / VirtualScreen::kWidth,
                                     sh / VirtualScreen::kHeight);
        return {
            scale,
            (sw - VirtualScreen::kWidth * scale) * 0.5f,
            (sh - VirtualScreen::kHeight * scale) * 0.5f,
        };
    }
}

void VirtualScreen::Init() {
    target_ = LoadRenderTexture(kWidth, kHeight);
    // Bilinear softens the up/down-scale; UI lines stay readable at any window
    // size. Switch to TEXTURE_FILTER_POINT if we ever want pixel-perfect look.
    SetTextureFilter(target_.texture, TEXTURE_FILTER_BILINEAR);
    initialized_ = true;
}

void VirtualScreen::Shutdown() {
    if (initialized_) {
        UnloadRenderTexture(target_);
        initialized_ = false;
    }
}

void VirtualScreen::Begin() {
    BeginTextureMode(target_);
}

void VirtualScreen::End() {
    EndTextureMode();
}

void VirtualScreen::Present() {
    // Letterbox bars.
    ClearBackground(BLACK);
    const Layout l = ComputeLayout();
    // Source height is negated because RenderTexture is stored bottom-up in
    // OpenGL; the negative flips it back during the blit.
    const Rectangle src{0.0f, 0.0f,
                        static_cast<float>(kWidth),
                        -static_cast<float>(kHeight)};
    const Rectangle dst{l.offsetX, l.offsetY,
                        kWidth * l.scale, kHeight * l.scale};
    DrawTexturePro(target_.texture, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);
}

Vector2 VirtualScreen::GetMouse() {
    const Layout l = ComputeLayout();
    const Vector2 m = GetMousePosition();
    return {
        (m.x - l.offsetX) / l.scale,
        (m.y - l.offsetY) / l.scale,
    };
}
