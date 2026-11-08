# LORD OF THE NORTH — Unreal Engine Mobile

This is the isolated Unreal Engine 5 development branch. The original Android/OpenGL project and the earlier Godot experiment remain untouched on their own branches.

## Current foundation
- Portrait Android orientation.
- Mobile-oriented renderer defaults (mobile HDR off; no Lumen/Nanite dependency).
- C++ third-person pawn with spring-arm camera and movement bindings.
- Default Unreal mobile virtual joystick enabled.
- Enhanced Input plugin enabled for future touch UI and action mapping.

## First playable milestone
1. Open `LORD_OF_THE_NORTH.uproject` in an installed Unreal Engine 5.x version.
2. Generate IDE project files and compile the Editor target.
3. Create the first vertical greybox level in-editor, then replace all blockout geometry with licensed modular environment assets.
4. Add a custom portrait HUD, touch camera control, interaction, dialogue, and save data.
5. Package and test on a physical Android device before adding large environments.

## Visual direction
A grounded, atmospheric northern-fantasy world: snow-covered stone fortress, dense pine forest, timber villages, cold overcast lighting, readable silhouettes, detailed PBR surfaces, and restrained particles. Avoid primitive cube-only environments.

## Performance policy
Target scalable Android settings first. Use baked/static lighting where possible, LODs, instancing, texture streaming, and conservative material complexity. High-end desktop features are not assumed to work on mobile.

## Important
The GitHub source scaffold is not yet a compiled Unreal project or APK. Unreal Editor, Android SDK/NDK, and a real device build are required to validate and package it.
