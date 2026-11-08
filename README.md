# LORD OF THE NORTH

**Active development branch:** migration/unity-urp

The current development target is a landscape 3D mobile RPG built with **Unity 6.3 LTS + URP**, with Android ARM64 as the first release platform.

## Project

- Unity project: [UnityProject/](UnityProject/)
- Migration implementation: [Docs/UNITY_MIGRATION.md](Docs/UNITY_MIGRATION.md)
- Cloud build setup: [Docs/CLOUD_BUILD_SETUP.md](Docs/CLOUD_BUILD_SETUP.md)
- Static validation: `python3 Scripts/validate_unity_project.py`

## Current implementation

- Fifteen source OBJ environment meshes migrated into the Unity project.
- Nine regional scene definitions and an Editor bootstrap that generates and saves the scenes.
- Mobile URP pipeline, procedural snow terrain layer and terrain-sampled environment placement.
- Third-person movement, touch controls, camera follow and player stats.
- Inventory, cloak/trinket equipment, quest interaction, dialogue, save data and regional travel.
- GitHub Actions static validation and a prepared Unity Android cloud-build workflow.

## Verification status

GitHub static validation has passed, including checks of all 15 OBJ meshes (vertices, faces, UV/normal references and MTL files). This does **not** prove Unity Editor compilation, actual scene generation, Android packaging or device QA.

A Unity cloud Editor run is still required to generate the nine Unity scenes, compile C#, run EditMode tests, produce the APK and verify it on a physical Android device.

## Preserved legacy work

Earlier native Android and Unreal experiments remain in their existing branches. This Unity migration is isolated and does not delete or overwrite them.
