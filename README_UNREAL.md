# LORD OF THE NORTH — Unreal Engine 5 Mobile

This is the isolated Unreal Engine development branch. The original Android/OpenGL project and the earlier Godot experiment remain untouched on their own branches.

## Project foundation

- Portrait Android configuration and mobile-oriented renderer defaults.
- Third-person C++ character with spring-arm camera and keyboard/gamepad/virtual joystick movement axes.
- Stamina-limited sprint with hold-to-run control and regeneration.
- Player stats component: health, stamina, gold and renown.
- Stackable inventory and apparel/trinket equipment state.
- Interaction interface and camera-view trace component with contextual prompts.
- NPC dialogue event and configurable quest offer/turn-in flow.
- Quest objective actors with progress tracking and reward handling.
- Save Game object and Game Instance save subsystem for player transform, stats, inventory, equipment and active/completed quest records.
- World map data asset seeded with the nine northern locations and renown unlock checks.
- Native portrait HUD fallback with health/stamina bars, progression labels, inventory count, quest objective/progress, interaction, hold-to-sprint and save/load controls.
- Player controller that creates the native HUD by default; a Widget Blueprint can override it for visual customization.
- GitHub Actions static source-structure validation.

## First playable milestone

The first slice is the road outside Winterfell:
1. Create the portrait-friendly map in Unreal Editor.
2. Create a Blueprint child of LOTNCharacter and assign a properly licensed character mesh and animation blueprint.
3. Open the map with the default C++ GameMode and verify the native HUD; optionally create a Widget Blueprint child of LOTNHUDWidget for a custom visual design.
4. Place an NPC Blueprint based on LOTNNPC and an objective Blueprint based on LOTNQuestObjective; match their quest IDs.
5. Import and profile the selected environment assets using the asset register.
6. Compile, test the movement, sprint, interaction/quest/inventory/save loop, and package to Android only after the required toolchain is installed.

## Visual direction

A grounded, atmospheric northern-fantasy world: weathered stone fortress, timber settlement, dense pine forest, snow-covered paths, cold overcast lighting and detailed PBR surfaces. No cube-only final environment.

## Mobile performance policy

Target scalable Android settings first. Do not depend on Lumen or Nanite. Prefer baked/static lighting, LODs, instancing, texture streaming, simple collision and conservative material complexity. Profile on a physical device.

## Build truth

The GitHub branch contains Unreal C++ source and configuration, not yet a compiled Unreal project or APK. GitHub Actions currently runs static source-structure checks only. A real Unreal compile/cook/package requires Unreal Engine and its matching Android toolchain.
