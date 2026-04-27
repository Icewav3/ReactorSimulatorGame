#ifndef REACTORSIM_THEME_H
#define REACTORSIM_THEME_H

#include "raylib.h"

// Central palette + chrome helpers for the vintage control-panel look.
// Anything that draws should pull colors and chrome from here so the look
// stays coherent and a future restyle is one file, not fifty.
//
// Per-widget visual choices that aren't palette-level (gauge tick spacing,
// label kerning, etc.) belong on the widget. This is the shared vocabulary.
//
// TODO text pass: font loading + per-widget typography decisions are
// deliberately deferred. Theme exposes a default font handle that resolves
// to GetFontDefault() until the real font is wired up; callers can already
// route through it so the swap is a one-line change later.

namespace Theme {

// --- Palette (vintage instrument panel) ---
// Names are role-based, not literal, so a future restyle doesn't ripple.
inline constexpr Color kPanelBeige   = {240, 230, 200, 255}; // panel face
inline constexpr Color kPanelBrown   = {139, 120,  93, 255}; // bezels, borders
inline constexpr Color kPanelOlive   = { 85, 107,  47, 255}; // safe / armed
inline constexpr Color kPanelRust    = {139,  69,  19, 255}; // alarm / danger
inline constexpr Color kPanelGold    = {218, 165,  32, 255}; // caution / lit
inline constexpr Color kPanelInk     = { 47,  47,  47, 255}; // text, needles
inline constexpr Color kPanelCream   = {250, 245, 235, 255}; // dial faces
inline constexpr Color kPanelOrange  = {205, 133,  63, 255}; // accent
inline constexpr Color kPanelShadow  = {  0,   0,   0,  60}; // soft drop shadow
inline constexpr Color kPanelHighlight = {255, 255, 255, 40}; // top-edge sheen
inline constexpr Color kScrewMetal   = {110, 100,  85, 255}; // screw heads
inline constexpr Color kRivetMetal   = {130, 120, 105, 255}; // rivet heads
inline constexpr Color kDymoTape     = { 30,  30,  30, 255}; // black label tape
inline constexpr Color kDymoText     = {235, 230, 215, 255}; // embossed letters

// --- Type ---
// TODO text pass: replace with a loaded font (e.g. condensed industrial sans).
const Font& DefaultFont();
inline constexpr int kFontSizeLabel = 12;
inline constexpr int kFontSizeValue = 18;
inline constexpr int kFontSizeDymo  = 14;

// --- Chrome primitives ---
// All take a Rectangle so callers can lay them out with rect math and
// not worry about origin/size conventions.

// Rounded recessed bezel around an instrument face.
void DrawBezel(Rectangle bounds, float thickness = 4.0f);

// Single screw head (outer ring + slot). Center + radius.
void DrawScrew(Vector2 center, float radius);

// Black embossed label-maker tape. Text is centered.
// TODO text pass: kerning + font swap.
void DrawDymoLabel(Rectangle bounds, const char* text);

// Full panel background: face color + bezel + corner screws.
void DrawPanelChrome(Rectangle bounds);

// Row of rivets along the top/bottom edge of a bounds rect.
void DrawRivets(Rectangle bounds, int count, bool topEdge = true);

} // namespace Theme

#endif //REACTORSIM_THEME_H
