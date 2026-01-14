# Todo

# Codebase Cleanliness

- Change Instrument class to instead be a UIElementBase class
    - Create a way to have a panel with rivets in each corner that is layered under whatever UI element inheritor is
      there

## Responsiveness

- Window Resizing

## Mechanics:

- reactor rod indicators to show if it's moving
- potentially 7 segment that shows the actual reactor rod position

# Polishing:

- Ui rework
    - Redo spacing and move around Dials
    - Try other color schemes
- Sound effects
- Blinking warning lights
- Allow dials to *break* if they go over the max before a loss condition

## Features:

- turbine controller
    - bridge the gap between pressure, heat and turbine RPM
    - lose if RPM exceeds max