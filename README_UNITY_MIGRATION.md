# LORD OF THE NORTH — Unity migration branch

Active migration work is on migration/unity-urp.

- Unity project: UnityProject/
- Original OBJ environment source: UnityProject/Assets/Art/SourceOBJ/
- Scene generator: UnityProject/Assets/Editor/LOTNProjectBootstrap.cs
- Android build pipeline: UnityProject/Assets/Editor/LOTNBuildPipeline.cs
- Static validation: python3 Scripts/validate_unity_project.py
- Android cloud workflow: .github/workflows/unity-android.yml
- Unity Build Automation pre-export hook: LOTN.Editor.LOTNProjectBootstrap.PreExport

Read UnityProject/README.md, Docs/UNITY_MIGRATION.md and Docs/CLOUD_BUILD_SETUP.md before enabling a cloud build.

This branch preserves the Unreal and earlier Android branches. It is not yet a verified playable APK until a licensed Unity cloud build succeeds and device QA is completed.
