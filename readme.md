The core fantasy is the illusion of control over something terrifyingly powerful that you can never directly see. You are the person in the room full of dials — and the reactor is a monster behind a wall.  

Style:
Heavy analog focus, yellowed beige sun bleached control panel. 1960s era with creative liberty. A mix between a large variety of different control mechanics. Each unique instrument that is used for interaction should have a unique method of interaction. Ensure things are not boring by creatively using the tools at our disposal. E.g. simulate weight of a lever by binding the mouse to it when clicked and then scale the users mouse input to a bit smaller, this allows for weight. Ensure reactive and enviromental warnings. The more visually going on the better. But things should also be labeled and logical. This UI should look like it was built by engineers who didn't care how it looked — they cared that it worked. Things should be a bit worn, and show signs of heavy use.

Interaction Feel:
Switches should have two states with a committed, binary feel — no hover dithering, no soft animations. You flip something, it flips. The decisiveness matters.
Rotary controls for things like valve positions should resist the mouse slightly — they shouldn't jump to wherever you click, they should turn, with a visible mechanical detent every few degrees. Turning something too fast should feel slightly wrong.
Confirmation dialogs for dangerous actions should not be modern modal popups. They should be implemented as physically lifting a safety cover and pressing a button underneath — a sequential unlock gesture that takes a deliberate moment. Friction is the key design.

The Control Room as a Composition:
The overall layout should evoke a physical panel — not a software dashboard. Sections should literally look like they were bolted together, with visible seams and screws, slightly different background tones between panels as if they were manufactured separately. Nothing is perfectly aligned. There's a slight grid irregularity that suggests this was built, not generated.
The player should feel small in front of it.


Shader & Post-Processing — Quick Brief for Design Team
The Core Mechanism
Raylib lets you render the entire scene to a texture first, then run GLSL fragment shaders over it before it hits the screen. Effects can be stacked in passes and every parameter can be driven by a float from game code — meaning visual degradation can be tied directly to system state without any backend coupling.
What's Available Out of the Box
Raylib ships ready-to-use shader files for bloom, blur, scanlines, fisheye, and edge detection. No writing shaders from scratch required to get started.
The Recommended Selective Approach
Rather than a full-screen CRT overlay, consider applying shaders selectively — individual screen/display elements (readout panels, monitoring screens within the physical panel) get the CRT treatment, while the physical hardware around them does not. This sells the illusion that you're looking at a screen embedded in a real object, which is far more grounded than everything being a TV.
Dynamic Stress Wiring
Any shader uniform can be updated every frame from a single value. One systemStress float can simultaneously drive bloom intensity, scanline visibility, chromatic aberration, and noise — the display elements degrade visually as the reactor degrades, with no hardcoding to specific backend events.
The Golden Rule
All of these effects should be nearly invisible at rest. Their job is not to look cool — it's to make the player feel something is wrong before they can articulate why.