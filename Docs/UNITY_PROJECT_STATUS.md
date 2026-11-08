# Unity migration status

Last reviewed: 2026-11-08

## Active target
- Branch: migration/unity-urp
- Engine: Unity 6.3 LTS, editor version 6000.3.27f1
- Renderer: URP 17.3, mobile forward configuration
- Platform: Android ARM64, landscape, IL2CPP

## Repository work completed
- Fifteen source OBJ meshes and the shared MTL library were copied from the Unreal branch.
- OBJ import configuration sets centimeter-to-meter scale, bakes axis conversion and generates secondary lightmap UVs.
- Editor bootstrap generates nine distinct named landscape scenes, snow terrain layers, URP assets, collision for major structures, region-specific forest/water dressing and Winterfell quest objects.
- C# prototype includes third-person movement, touch look/movement/sprint/interact, camera follow, player stats, persistent region checkpoints, inventory stacks, cloak/trinket equipment, quest dialogue/rewards and a clickable nine-region route map.
- Android build settings and a custom build pipeline are present.
- Unity EditMode tests cover region data, player stats, inventory, quest progression and per-region position saving.
- GitHub Actions static validator checks project files and OBJ vertex/face/UV/normal/MTL integrity.

## Verified
- GitHub hosted static validation passed on commit f473b48a.
- The validator reported 15 OBJ meshes, 8,100 vertices and 6,724 faces.
- Static validation is repository-level validation only; it is not a Unity compile.

## Not yet verified
- Unity Editor has not yet imported the assets in a real Editor session.
- No generated Unity scene file has been confirmed as saved by Unity.
- C# compilation and EditMode tests have not run inside Unity.
- No Android APK/AAB has been produced.
- No physical Android device QA has been performed.

## Required external gate
A Unity cloud Editor is required. The repository includes a Unity Build Automation pre-export hook at LOTN.Editor.LOTNProjectBootstrap.PreExport and a GameCI Android workflow. The repository owner must connect Unity Build Automation to this GitHub repository or configure a valid Unity license in GitHub Actions secrets. Do not share license files, serials, passwords, or signing keys in chat.
