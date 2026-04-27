#include "Theme.h"

namespace Theme {

const Font& DefaultFont() {
	// TODO text pass: load a real font once and cache it here.
	static const Font font = GetFontDefault();
	return font;
}

void DrawBezel(Rectangle bounds, float thickness) {
	// Outer brown ring.
	DrawRectangleRoundedLinesEx(bounds, 0.08f, 6, thickness, kPanelBrown);
	// Inner dark line for depth.
	Rectangle inner = {
		bounds.x + thickness,
		bounds.y + thickness,
		bounds.width  - thickness * 2.0f,
		bounds.height - thickness * 2.0f,
	};
	DrawRectangleRoundedLinesEx(inner, 0.08f, 6, 1.0f, kPanelInk);
	// Top sheen.
	DrawLineEx(
		{bounds.x + thickness * 1.5f, bounds.y + thickness * 0.5f},
		{bounds.x + bounds.width - thickness * 1.5f, bounds.y + thickness * 0.5f},
		1.0f, kPanelHighlight);
}

void DrawScrew(Vector2 center, float radius) {
	DrawCircleV(center, radius, kScrewMetal);
	DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, kPanelInk);
	// Slot.
	DrawLineEx(
		{center.x - radius * 0.7f, center.y},
		{center.x + radius * 0.7f, center.y},
		1.0f, kPanelInk);
}

void DrawDymoLabel(Rectangle bounds, const char* text) {
	DrawRectangleRounded(bounds, 0.25f, 4, kDymoTape);
	// TODO text pass: real font + kerning.
	const int fontSize = kFontSizeDymo;
	const int textWidth = MeasureText(text, fontSize);
	const float tx = bounds.x + (bounds.width  - static_cast<float>(textWidth)) * 0.5f;
	const float ty = bounds.y + (bounds.height - static_cast<float>(fontSize))  * 0.5f;
	DrawText(text, static_cast<int>(tx), static_cast<int>(ty), fontSize, kDymoText);
}

void DrawPanelChrome(Rectangle bounds) {
	// Face.
	DrawRectangleRec(bounds, kPanelBeige);
	// Bezel.
	DrawBezel(bounds, 4.0f);
	// Corner screws.
	const float inset = 10.0f;
	const float r     = 4.0f;
	DrawScrew({bounds.x + inset,                       bounds.y + inset},                        r);
	DrawScrew({bounds.x + bounds.width  - inset,       bounds.y + inset},                        r);
	DrawScrew({bounds.x + inset,                       bounds.y + bounds.height - inset},        r);
	DrawScrew({bounds.x + bounds.width  - inset,       bounds.y + bounds.height - inset},        r);
}

void DrawRivets(Rectangle bounds, int count, bool topEdge) {
	if (count < 2) return;
	const float y = topEdge ? bounds.y + 6.0f : bounds.y + bounds.height - 6.0f;
	const float step = bounds.width / static_cast<float>(count - 1);
	for (int i = 0; i < count; ++i) {
		const float x = bounds.x + step * static_cast<float>(i);
		DrawCircleV({x, y}, 2.5f, kRivetMetal);
		DrawCircleLines(static_cast<int>(x), static_cast<int>(y), 2.5f, kPanelInk);
	}
}

} // namespace Theme
