# Unreal Editor, validation and Android packaging

## What CI can verify now

The GitHub Actions workflow **Validate Unreal source structure** checks:
- .uproject JSON and runtime module declaration
- Unreal module and target-rule files
- reflected header/generated-header structure
- matching C++ header/source pairs
- portrait Android and interaction input settings

This is a static repository check only. It does not compile Unreal C++, open the editor, cook assets, or create an APK.

## Required local toolchain

Install a matching Unreal Engine 5.x version from Epic Games Launcher (or a licensed source build), then install the Android SDK, NDK and Java versions supported by that exact Unreal release. In Unreal Editor:
1. Open LORD_OF_THE_NORTH.uproject.
2. Allow project files to regenerate if prompted.
3. Compile the LordOfTheNorthEditor target.
4. Fix all compiler and editor warnings/errors before proceeding.
5. Create and save the first map as Content/Maps/VerticalPrototype; add a PlayerStart and a walkable ground/landscape surface.
6. Set the map as the editor startup and game default map.
7. Add a Blueprint child of LOTNCharacter with the licensed player mesh, skeleton and animation blueprint.
8. Add a Blueprint child of LOTNGameMode, set its Default Pawn Class to the player Blueprint, and assign that GameMode to the map. Keep LOTNPlayerController as the Player Controller Class unless you create a controller child to customize the HUD.
9. Place an LOTNNPC Blueprint and an LOTNQuestObjective Blueprint in the map. Assign meshes, materials and collision; match the quest IDs.
10. Create a LOTNWorldMapData asset and review its nine location entries. Assign a real level name only after the corresponding map exists.
11. The C++ PlayerController creates the native portrait HUD automatically. Optionally create a Widget Blueprint child of LOTNHUDWidget to customize it; keep the named widget bindings documented in QUEST_SYSTEM.md.

## Android package

Use the Android platform setup wizard supplied by the installed Unreal version. Confirm SDK/NDK paths and accept required licenses. Then use Platforms > Android > Package Project in the editor for the first device build.

Before distributing an APK:
- Test on a physical Android device.
- Verify portrait orientation, touch camera drag, joystick movement and interaction.
- Check save/load persistence after closing and reopening the app.
- Profile frame time, memory, shader complexity and thermal behavior.
- Test a clean install and an upgrade install.

## GitHub-hosted Android build limitation

A normal GitHub-hosted Ubuntu runner does not include Unreal Engine or its Android toolchain. The repository now has a manual Unreal package workflow at .github/workflows/build-unreal-android.yml. It requires a self-hosted runner labelled self-hosted, unreal-engine and android, with UE_ROOT, ANDROID_HOME and JAVA_HOME configured. The workflow deliberately stops if no .umap level exists, so it cannot create a misleading APK from an empty project. Create Content/Maps/VerticalPrototype.umap and complete the art/import gate before dispatching it. Do not interpret the existing Gradle workflow as an Unreal build; it belongs to the legacy Android project.
