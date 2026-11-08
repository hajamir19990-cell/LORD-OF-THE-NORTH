# LORD OF THE NORTH — Unity 6.3 LTS

This is the active Unity migration branch. The Unity project is in this folder so the legacy Unreal and native Android work remains preserved in the repository.

## Engine and target
- Unity Editor: 6000.3.27f1 (Unity 6.3 LTS)
- Render pipeline: URP 17.3
- Target: Android, ARM64, landscape
- Gameplay code: C#

## Current prototype foundation
- Fifteen original OBJ environment source meshes migrated from the Unreal branch.
- Editor automation to generate nine landscape scenes.
- Third-person CharacterController movement, keyboard/editor fallback and touch input.
- Follow camera, player health/stamina/gold/renown HUD.
- Stackable inventory, cloak/trinket equipment, quest item collection and local persistence.
- Generated snow TerrainLayer and mobile URP asset with forward rendering settings.
- Quest giver and supply-crate interaction with persistent quest progress.
- Nine-region world map with renown-gated travel.
- Player and quest values persisted with PlayerPrefs.
- Android build method and GitHub Actions workflow.

## Generate scenes in Unity Editor
Open this folder as a Unity project, allow package import to finish, then use the Tools → LORD OF THE NORTH → Generate all nine regional scenes menu. The generated scenes and terrain assets are saved under Assets/Scenes and Assets/Generated/Terrain.

The automated cloud Android build invokes the same generator before building.

## Build
The Android cloud workflow is manually dispatched from GitHub Actions. It requires a valid Unity license configured as repository Actions secrets. The workflow runs LOTN.Editor.LOTNBuildPipeline.BuildAndroid and uploads LORD-OF-THE-NORTH.apk.

Static checks can run without a Unity license:
```bash
python3 Scripts/validate_unity_project.py
```

## Honest status boundary
Repository validation does not prove Unity C# compilation, scene import, Android packaging, or device QA. Those are only complete after the Unity Editor build succeeds and the APK is installed and tested on a physical Android device.
