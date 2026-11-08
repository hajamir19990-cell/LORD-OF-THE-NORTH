# Cloud build setup — Unity Android

The Unity project is prepared for cloud building, but this repository cannot authorize a Unity account or create a Unity Cloud organization on the user's behalf.

## Option A — Unity Build Automation (recommended cloud service)
Unity Build Automation builds projects in Unity's cloud and supports Android. Official setup documentation:
- https://docs.unity.com/en-us/build-automation
- https://docs.unity.com/en-us/build-automation/basic-build-configuration/overview

Steps performed by the repository owner:
1. Sign in to the Unity Dashboard using the Unity account that owns the project.
2. Open DevOps → Build Automation and connect the GitHub repository.
3. Grant Unity's GitHub App access only to LORD-OF-THE-NORTH.
4. Select branch migration/unity-urp.
5. Set Project subfolder path to UnityProject.
6. Select Unity 6000.3.27f1 (or a compatible Unity 6.3 LTS patch) and Android as the target.
7. Configure Android output as APK for device testing. Keep signing credentials private; a debug APK is sufficient for initial QA.
8. Start a build and inspect the cloud build log and downloadable artifact.

Unity's documentation explains that the project subfolder must contain Assets, Packages and ProjectSettings, and that the build target and Unity version are selected in the cloud configuration.

## Option B — GitHub Actions with GameCI
The repository includes .github/workflows/unity-android.yml. It runs only when manually dispatched and uses GameCI's Unity Builder.

A valid Unity license must be configured in repository Settings → Secrets and variables → Actions. Prefer the UNITY_LICENSE secret when the account's current Unity licensing flow supports it. Do not commit license files, passwords, serials, keystores or signing passwords into Git.

Official GameCI documentation:
- https://game.ci/docs/github/getting-started/
- https://game.ci/docs/github/activation/
- https://game.ci/docs/github/builder/

Unity's current licensing rules and account eligibility take precedence. If a Personal license cannot be activated by the documented GameCI route, use Unity Build Automation rather than attempting to bypass licensing.

## Costs and account safety
Check the current Unity plan, cloud build minutes, storage, runner limits and any payment requirements before enabling a paid service. Do not add payment methods or purchase cloud capacity without the account owner's/parent or guardian's approval. Never share Unity or GitHub passwords in chat.

## What is automated by this repository
The cloud build method generates nine scenes, sets Android landscape orientation and ARM64 IL2CPP, builds the APK, and uploads it as a GitHub Actions artifact. The first successful cloud build is still required to confirm Unity import, C# compilation, scene generation and Android packaging.
