#include "Layout.h"
#include <numeric>

namespace Layout {

namespace {

std::vector<Rectangle> SplitAxis(Rectangle bounds, const std::vector<float>& weights, float gap, bool vertical) {
	std::vector<Rectangle> out;
	if (weights.empty()) return out;

	const float total = std::accumulate(weights.begin(), weights.end(), 0.0f);
	if (total <= 0.0f) return out;

	const int   n        = static_cast<int>(weights.size());
	const float gapTotal = gap * static_cast<float>(n - 1);
	const float extent   = (vertical ? bounds.height : bounds.width) - gapTotal;
	const float origin   = vertical ? bounds.y : bounds.x;

	out.reserve(weights.size());
	float cursor = origin;
	for (int i = 0; i < n; ++i) {
		const float size = extent * (weights[static_cast<size_t>(i)] / total);
		Rectangle r = vertical
			? Rectangle{bounds.x, cursor, bounds.width, size}
			: Rectangle{cursor, bounds.y, size, bounds.height};
		out.push_back(r);
		cursor += size + gap;
	}
	return out;
}

std::vector<Rectangle> SplitAxisPx(Rectangle bounds, const std::vector<float>& sizesPx, float gap, bool vertical) {
	std::vector<Rectangle> out;
	if (sizesPx.empty()) return out;

	const int   n        = static_cast<int>(sizesPx.size());
	const float gapTotal = gap * static_cast<float>(n - 1);
	const float extent   = (vertical ? bounds.height : bounds.width) - gapTotal;
	const float origin   = vertical ? bounds.y : bounds.x;

	// Sum fixed sizes; a trailing 0 means "fill remaining".
	float fixed = 0.0f;
	for (int i = 0; i < n; ++i) fixed += sizesPx[static_cast<size_t>(i)];
	const float fillSize = extent - fixed; // only used when last entry is 0

	out.reserve(sizesPx.size());
	float cursor = origin;
	for (int i = 0; i < n; ++i) {
		float size = sizesPx[static_cast<size_t>(i)];
		if (size == 0.0f && i == n - 1) size = fillSize > 0.0f ? fillSize : 0.0f;

		Rectangle r = vertical
			? Rectangle{bounds.x, cursor, bounds.width, size}
			: Rectangle{cursor, bounds.y, size, bounds.height};
		out.push_back(r);
		cursor += size + gap;
	}
	return out;
}

} // namespace

std::vector<Rectangle> SplitV(Rectangle bounds, const std::vector<float>& weights, float gap) {
	return SplitAxis(bounds, weights, gap, true);
}

std::vector<Rectangle> SplitH(Rectangle bounds, const std::vector<float>& weights, float gap) {
	return SplitAxis(bounds, weights, gap, false);
}

std::vector<Rectangle> SplitVPx(Rectangle bounds, const std::vector<float>& sizesPx, float gap) {
	return SplitAxisPx(bounds, sizesPx, gap, true);
}

std::vector<Rectangle> SplitHPx(Rectangle bounds, const std::vector<float>& sizesPx, float gap) {
	return SplitAxisPx(bounds, sizesPx, gap, false);
}

Rectangle Inset(Rectangle bounds, float padding) {
	return Inset(bounds, padding, padding, padding, padding);
}

Rectangle Inset(Rectangle bounds, float horizontal, float vertical) {
	return Inset(bounds, horizontal, vertical, horizontal, vertical);
}

Rectangle Inset(Rectangle bounds, float left, float top, float right, float bottom) {
	Rectangle r{
		bounds.x + left,
		bounds.y + top,
		bounds.width  - left - right,
		bounds.height - top  - bottom,
	};
	if (r.width  < 0.0f) r.width  = 0.0f;
	if (r.height < 0.0f) r.height = 0.0f;
	return r;
}

Rectangle Center(Rectangle bounds, float width, float height) {
	return Rectangle{
		bounds.x + (bounds.width  - width)  * 0.5f,
		bounds.y + (bounds.height - height) * 0.5f,
		width,
		height,
	};
}

Rectangle CenterSquare(Rectangle bounds) {
	const float side = bounds.width < bounds.height ? bounds.width : bounds.height;
	return Center(bounds, side, side);
}

std::vector<Rectangle> Grid(Rectangle bounds, int rows, int cols, float gap) {
	std::vector<Rectangle> out;
	if (rows <= 0 || cols <= 0) return out;

	const float gapW = gap * static_cast<float>(cols - 1);
	const float gapH = gap * static_cast<float>(rows - 1);
	const float cellW = (bounds.width  - gapW) / static_cast<float>(cols);
	const float cellH = (bounds.height - gapH) / static_cast<float>(rows);

	out.reserve(static_cast<size_t>(rows * cols));
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			out.push_back(Rectangle{
				bounds.x + static_cast<float>(c) * (cellW + gap),
				bounds.y + static_cast<float>(r) * (cellH + gap),
				cellW,
				cellH,
			});
		}
	}
	return out;
}

} // namespace Layout
