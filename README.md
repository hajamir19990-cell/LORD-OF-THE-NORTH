# LORD OF THE NORTH — Native Android Edition

A native Android game foundation using Java and OpenGL ES 2.0. The runtime does not use a WebView or require an internet connection.

## Current implementation
- Third-person northern terrain exploration
- Nine named locations: Winterfell, the Dreadfort, White Harbor, Bear Island, Karhold, Last Hearth, Deepwood Motte, Greywater Watch and the Wall
- Named northern NPCs, basic dialogue and proximity interaction
- Basic quest text and renown progression
- Landscape presentation with touch joystick and touch-look controls
- Day/night ambient color cycle
- Local terrain, stone, bark, rock and forest textures
- Local save/load for player position, gold and renown

## Build toolchain
The checked-in project currently uses:
- Android Gradle Plugin: 8.7.3
- Gradle wrapper: 8.9
- JDK: 17
- Android compile SDK / target SDK: 35
- Minimum Android SDK: 26

Use a compatible Android Studio installation, allow Gradle sync, then run `./gradlew :app:assembleDebug`.

## APK output
Debug APK: `app/build/outputs/apk/debug/app-debug.apk`
Release APK: `app/build/outputs/apk/release/app-release.apk` (requires release signing configuration before distribution).
