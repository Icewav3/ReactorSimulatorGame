#ifndef REACTORSIM_LAYOUT_H
#define REACTORSIM_LAYOUT_H

#include "raylib.h"
#include <vector>

// Stateless rect-math helpers. No tree, no allocations beyond the result
// vector, no per-frame state. Callers slice a Rectangle into smaller
// Rectangles and feed those to widgets. This is the entire layout system —
// the "tree" is just C++ call structure (Console splits into Panels,
// Panels split into instrument rects).
//
// Convention: weights are unitless ratios. Pixel splits use the *Px variants.
// All functions are pure: same input -> same output, safe to call any frame.

namespace Layout {

// Split vertically (stack rows top-to-bottom) by weights.
// SplitV({0,0,100,300}, {1, 2}) -> [{0,0,100,100}, {0,100,100,200}]
std::vector<Rectangle> SplitV(Rectangle bounds, const std::vector<float>& weights, float gap = 0.0f);

// Split horizontally (stack columns left-to-right) by weights.
std::vector<Rectangle> SplitH(Rectangle bounds, const std::vector<float>& weights, float gap = 0.0f);

// Split vertically by absolute pixel sizes. Last entry of 0 means "fill remaining".
std::vector<Rectangle> SplitVPx(Rectangle bounds, const std::vector<float>& sizesPx, float gap = 0.0f);

// Split horizontally by absolute pixel sizes. Last entry of 0 means "fill remaining".
std::vector<Rectangle> SplitHPx(Rectangle bounds, const std::vector<float>& sizesPx, float gap = 0.0f);

// Shrink a rect inward. Uniform on all sides.
Rectangle Inset(Rectangle bounds, float padding);

// Shrink with separate horizontal/vertical insets.
Rectangle Inset(Rectangle bounds, float horizontal, float vertical);

// Shrink with per-side insets (left, top, right, bottom).
Rectangle Inset(Rectangle bounds, float left, float top, float right, float bottom);

// Center a (width x height) rect inside bounds.
Rectangle Center(Rectangle bounds, float width, float height);

// Largest centered square that fits inside bounds.
Rectangle CenterSquare(Rectangle bounds);

// Uniform grid of cells. Row-major: index = row * cols + col.
// Use At(grid, row, col) to fetch a single cell.
std::vector<Rectangle> Grid(Rectangle bounds, int rows, int cols, float gap = 0.0f);

// Convenience accessor for Grid results.
inline Rectangle At(const std::vector<Rectangle>& grid, int row, int col, int cols) {
	return grid[static_cast<size_t>(row * cols + col)];
}

} // namespace Layout

#endif //REACTORSIM_LAYOUT_H
