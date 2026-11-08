# Project status — Unreal mobile branch

Last updated: 2026-11-08

## Active branch
- Branch: rebuild/unreal-5-mobile
- Original Android project and earlier experiments are preserved on their existing branches.
- Development commits are isolated from main.

## Implemented in C++ source
- Landscape-oriented Android configuration and package identifier.
- Third-person character movement, camera, mobile joystick axes and stamina-based sprint.
- Player stats: health, stamina, gold and renown.
- Native landscape HUD fallback with contextual interaction prompt, dialogue, quest display, interactive inventory overlay with apparel/trinket equip controls, touch interaction/sprint and save/load controls; save/load controls are positioned clear of the lower-left landscape virtual joystick; player controller now uses GameAndUI input mode so touch UI can receive input alongside gameplay.
- Interactable interface, camera-view line trace, NPC dialogue and quest offer/turn-in.
- Quest objective progress and one-time rewards.
- Stackable inventory and apparel/trinket equipment slots.
- Save/load of transform, location ID, stats, inventory, equipment and quest state.
- Nine-region world-map data with renown unlock checks.
- Unreal automation tests for inventory stacking/removal, equipment slots, invalid inventory inputs and per-call limits, quest progress/restore, map unlock rules, player health/stamina bounds, and rejection of negative gold/renown adjustments.

## Build and test status
- GitHub Actions static source-structure validation now watches project/config/C++ source, environment OBJ assets, assembly scripts, map guides, docs and workflow changes. The validator checks Python syntax and OBJ vertex/face index integrity; it is not an Unreal import or build.
- Mobile HUD input routing has been corrected to GameAndUI and added to the QA checklist/static validator. Currency and renown now reject negative adjustments, with regression coverage committed. All new automation tests still require execution inside Unreal Editor.
- GitHub Actions passed 66 static structure checks with 0 failures on commit 6a949632. These checks validate repository structure only; they are explicitly not an Unreal compile.
- Unreal automation tests have been added but have not been executed in Unreal Editor.
- No Unreal C++ compile has been verified.
- No Android APK has been produced.

## Missing editor assets
- No .umap level has been created yet. The Unreal Editor Python assembly script imports seven committed OBJ source meshes and lays out a landscape greybox with rolling terrain, curtain walls, watchtowers, adjoining road tiles, settlement buildings, pine groves, native C++ quest-giver/scout/objective actors, navigation bounds and temporary atmosphere; it must still be run and inspected in Unreal Editor.
- The seven starter OBJ source meshes (gatehouse, pine, longhouse, cobblestone road, curtain wall, watchtower and rolling terrain) are committed under Content/SourceMeshes; they are source interchange files, not integrated Unreal assets. Static validation now parses every OBJ and checks finite vertices and in-range face indices.
- No binary .uasset/.ubulk character, animation or final environment assets have been imported.
- Candidate CC0 environment sources and mobile import checks are recorded in Docs/ASSET_REGISTER.md.
- Landscape layout dimensions are in Content/MAPS_README.md; editor assembly steps are in Content/LANDSCAPE_ASSEMBLY.md and Scripts/create_winterfell_landscape.py.

## What is required to finish the first APK
1. Open the project in a compatible Unreal Engine 5.x installation.
2. Compile the Editor target and resolve any Unreal Header Tool/compiler errors.
3. Run Scripts/create_winterfell_landscape.py inside Unreal Editor to create /Game/Maps/WinterfellApproach, then inspect terrain collision/materials and add NPC/quest Blueprints; a sculpted Landscape can replace the terrain mesh if layered landscape materials are needed.
4. Import and profile licensed character/environment assets; assign the character mesh and animation blueprint.
5. Run the LOTN automation tests and the manual QA checklist.
6. Configure a Linux self-hosted GitHub Actions runner with Unreal Engine, Android SDK/NDK and Java, then dispatch Build Unreal Android package. The workflow compiles the Editor target and runs the LOTN automation tests before packaging.
7. Install and test the resulting APK on a physical Android device.

The repository deliberately blocks the manual APK workflow while no .umap exists. Do not mark the project complete until the compile, map, assets, automation tests, package and device QA gates all pass.
