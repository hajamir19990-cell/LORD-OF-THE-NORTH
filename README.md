# LORD OF THE NORTH — Unreal Engine Mobile Project

A portrait-oriented mobile third-person fantasy RPG project inspired by the North of Westeros.

## Current project state

- Unreal Engine project descriptor: `LORD_OF_THE_NORTH.uproject`
- Runtime module: `LordOfTheNorth`
- Android orientation is configured as **Portrait** in `Config/DefaultEngine.ini`.
- Source 3D meshes are stored under `Content/SourceMeshes/` as OBJ files.
- The Unreal Android workflow is designed to validate source assets, compile the Editor target, assemble and validate nine maps, run gameplay automation tests, and package an Android build.
- Generated Unreal assets and maps must be verified from a successful Unreal Editor run. A successful static validation workflow alone does not prove that the Unreal project compiles or that an APK exists.

## Android build requirements

The packaging workflow uses a self-hosted Linux GitHub Actions runner with these labels:

```text
self-hosted, linux, unreal-engine, android
```

The runner must have a compatible Unreal Engine installation and Android toolchain configured, including:

- `UE_ROOT` pointing to the Unreal Engine installation
- Android SDK (`ANDROID_HOME`)
- Java JDK (`JAVA_HOME`)
- Android NDK (`NDKROOT` or `ANDROID_NDK_ROOT`)

See [Build Unreal Android workflow](.github/workflows/build-unreal-android.yml).

## Build verification

A build is considered complete only after the Unreal Editor target compiles, all required maps are generated and validated, gameplay automation tests pass, and the Android package is uploaded as a workflow artifact.

Static repository checks are useful, but they are not a substitute for an Unreal Engine build or an installable Android package.
