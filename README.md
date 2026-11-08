# LORD OF THE NORTH — Rebuild

A clean rebuild of the northern open-world fantasy RPG, now using **Godot 4.7 stable** and GDScript instead of the previous hand-written Android/OpenGL renderer.

## Preserved game concept

- Nine northern destinations: Winterfell, the Dreadfort, White Harbor, Bear Island, Karhold, Last Hearth, Deepwood Motte, Greywater Watch, and the Wall.
- Third-person exploration, camera-relative movement, mobile touch controls, and a readable landscape HUD.
- Main quest, named locations, NPC interaction and dialogue, gold, renown, stamina, and local save/load.
- Main menu: New Game, Continue, Settings, How to Play, Developers, and Exit.
- Developers: **AmirReza — Game Director & Creator**; **Raha — Creative & Technical Assistant**.

## New technical direction

- Godot 4.7 stable, GDScript, Mobile renderer.
- Procedural 3D environment for the first playable milestone; no fragile runtime mesh-buffer code.
- Original vector direwolf app icon.
- Android ARM64 debug APK exported by GitHub Actions.
- Project validation and APK artifact checks in CI.

## Current milestone

This branch is the new foundation, not a finished open-world game. It contains a procedural northern landscape, settlement blockouts, a third-person player, basic mobile controls, a first NPC interaction, HUD, menu, and local save data. Realistic character/environment models, authored textures, audio, deeper quest/inventory/equipment systems, weather and optimization are staged for later milestones and must be tested before being marked complete.

## Build

Open this folder in Godot 4.7 stable. For Android, install matching export templates, OpenJDK 17, Android SDK Platform 35 and Build Tools 35.0.0, then export the Android preset. GitHub Actions performs the same export and uploads the debug APK.

## Performance target

Mobile-first landscape mode. The first target is stable operation on mid-range Android hardware; visual quality and scene density will be tuned against real-device measurements, not guessed.
