#include "Theme.h"

namespace Theme {

const Font& DefaultFont() {
	static Font font = []() {
		Font f = LoadFontEx("Resources/fonts/TerminusTTF-4.49.3.ttf", 40, nullptr, 0);
		if (f.texture.id == 0) f = GetFontDefault();
		SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
		return f;
	}();
	return font;
}

const Font& BoldFont() {
	static Font font = []() {
		Font f = LoadFontEx("Resources/fonts/TerminusTTF-Bold-4.49.3.ttf", 40, nullptr, 0);
		if (f.texture.id == 0) f = GetFontDefault();
		SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
		return f;
	}();
	return font;
}

const Font& OcrFont() {
	static Font font = []() {
		Font f = LoadFontEx("Resources/fonts/OCR-A Regular.ttf", 40, nullptr, 0);
		if (f.texture.id == 0) f = GetFontDefault();
		SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
		return f;
	}();
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
	const float fontSize = static_cast<float>(kFontSizeDymo);
	const Font& font = BoldFont();
	Vector2 textSize = MeasureTextEx(font, text, fontSize, 0.5f);
	const float tx = bounds.x + (bounds.width  - textSize.x) * 0.5f;
	const float ty = bounds.y + (bounds.height - textSize.y) * 0.5f;
	DrawTextEx(font, text, {tx, ty}, fontSize, 0.5f, kDymoText);
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
