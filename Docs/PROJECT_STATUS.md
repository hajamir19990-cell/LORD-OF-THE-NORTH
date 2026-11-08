# Project status — Unreal mobile branch

Last updated: 2026-11-08

## Active branch
- Branch: rebuild/unreal-5-mobile
- Original Android project and earlier experiments are preserved on their existing branches.
- Development commits are isolated from main.

## Implemented in C++ source
- Portrait-oriented Android configuration and package identifier.
- Third-person character movement, camera, mobile joystick axes and stamina-based sprint.
- Player stats: health, stamina, gold and renown.
- Native portrait HUD fallback with contextual interaction prompt, dialogue, quest display, inventory count, touch interaction/sprint and save/load controls.
- Interactable interface, camera-view line trace, NPC dialogue and quest offer/turn-in.
- Quest objective progress and one-time rewards.
- Stackable inventory and apparel/trinket equipment slots.
- Save/load of transform, location ID, stats, inventory, equipment and quest state.
- Nine-region world-map data with renown unlock checks.
- Unreal automation tests for inventory, equipment, quest progress and map unlock rules.

## Build and test status
- GitHub Actions static source-structure validation exists.
- GitHub Actions passed 61 static structure checks with 0 failures on commit 3dcbdbc1. The following commit only tightened the asset register. These checks validate repository structure only; they are explicitly not an Unreal compile.
- Unreal automation tests have been added but have not been executed in Unreal Editor.
- No Unreal C++ compile has been verified.
- No Android APK has been produced.

## Missing editor assets
- No .umap level has been created yet.
- No binary .uasset/.ubulk environment, character, animation or UI assets have been imported.
- Candidate CC0 environment sources and mobile import checks are recorded in Docs/ASSET_REGISTER.md.
- First level assembly instructions are in Content/MAPS_README.md.

## What is required to finish the first APK
1. Open the project in a compatible Unreal Engine 5.x installation.
2. Compile the Editor target and resolve any Unreal Header Tool/compiler errors.
3. Create Content/Maps/VerticalPrototype.umap with PlayerStart and the first environment.
4. Import and profile licensed character/environment assets; assign the character mesh and animation blueprint.
5. Run the LOTN automation tests and the manual QA checklist.
6. Configure a Linux self-hosted GitHub Actions runner with Unreal Engine, Android SDK/NDK and Java, then dispatch Build Unreal Android package. The workflow compiles the Editor target and runs the LOTN automation tests before packaging.
7. Install and test the resulting APK on a physical Android device.

The repository deliberately blocks the manual APK workflow while no .umap exists. Do not mark the project complete until the compile, map, assets, automation tests, package and device QA gates all pass.
