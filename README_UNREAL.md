# LORD OF THE NORTH — Unreal Engine 5 Mobile

This is the isolated Unreal Engine development branch. The original Android/OpenGL project and the earlier Godot experiment remain untouched on their own branches.

## Project foundation

- Portrait Android configuration and mobile-oriented renderer defaults.
- Third-person C++ character with spring-arm camera and keyboard/gamepad/virtual joystick movement axes.
- Player stats component: health, stamina, gold and renown.
- Interaction interface and visibility-channel trace component.
- NPC dialogue event and configurable quest offer/turn-in flow.
- Quest objective actors with progress tracking and reward handling.
- Save Game object and Game Instance save subsystem for player transform, stats and active/completed quest records.
- World map data asset seeded with the nine northern locations.
- UMG HUD base class with health/stamina bars, progression labels, quest objective/progress, interaction and save/load buttons.
- Player controller that can instantiate a HUD Widget Blueprint assigned in the editor.
- GitHub Actions static source-structure validation.

## First playable milestone

The first slice is the road outside Winterfell:
1. Create the portrait-friendly map in Unreal Editor.
2. Create a Blueprint child of LOTNCharacter and assign a properly licensed character mesh and animation blueprint.
3. Create a HUD Widget Blueprint based on LOTNHUDWidget and assign it to a Player Controller Blueprint child of LOTNPlayerController.
4. Place an NPC Blueprint based on LOTNNPC and an objective Blueprint based on LOTNQuestObjective; match their quest IDs and bind dialogue events to the HUD.
5. Import and profile the selected environment assets using the asset register.
6. Compile, test the interaction/quest/save loop, and package to Android only after the required toolchain is installed.

## Visual direction

A grounded, atmospheric northern-fantasy world: weathered stone fortress, timber settlement, dense pine forest, snow-covered paths, cold overcast lighting and detailed PBR surfaces. No cube-only final environment.

## Mobile performance policy

Target scalable Android settings first. Do not depend on Lumen or Nanite. Prefer baked/static lighting, LODs, instancing, texture streaming, simple collision and conservative material complexity. Profile on a physical device.

## Build truth

The GitHub branch contains Unreal C++ source and configuration, not yet a compiled Unreal project or APK. GitHub Actions currently runs static source-structure checks only. A real Unreal compile/cook/package requires Unreal Engine and its matching Android toolchain.
