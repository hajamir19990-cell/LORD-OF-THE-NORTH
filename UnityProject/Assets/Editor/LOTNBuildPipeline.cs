using System.IO;
using System.Linq;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEngine;

namespace LOTN.Editor
{
    public static class LOTNBuildPipeline
    {
        public static void BuildAndroid()
        {
            try
            {
                LOTNProjectBootstrap.BuildPrototypeScenes();
                PlayerSettings.companyName = "LORD OF THE NORTH";
                PlayerSettings.productName = "LORD OF THE NORTH";
                PlayerSettings.SetApplicationIdentifier(BuildTargetGroup.Android, "com.lotn.northerngame");
                PlayerSettings.defaultInterfaceOrientation = UIOrientation.LandscapeLeft;
                PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64;
                PlayerSettings.Android.minSdkVersion = AndroidSdkVersions.AndroidApiLevel26;
                PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);
                EditorUserBuildSettings.buildAppBundle = false;

                string[] scenes = EditorBuildSettings.scenes
                    .Where(s => s.enabled)
                    .Select(s => s.path)
                    .ToArray();
                if (scenes.Length != 9)
                    throw new System.Exception("Expected nine enabled regional scenes; found " + scenes.Length);

                string output = Path.GetFullPath("Builds/Android/LORD-OF-THE-NORTH.apk");
                Directory.CreateDirectory(Path.GetDirectoryName(output));
                BuildPlayerOptions options = new BuildPlayerOptions
                {
                    scenes = scenes,
                    locationPathName = output,
                    target = BuildTarget.Android,
                    options = BuildOptions.None
                };
                BuildReport report = BuildPipeline.BuildPlayer(options);
                Debug.Log("LOTN Android build result: " + report.summary.result +
                          " | size: " + report.summary.totalSize + " bytes | output: " + output);
                EditorApplication.Exit(report.summary.result == BuildResult.Succeeded ? 0 : 1);
            }
            catch (System.Exception exception)
            {
                Debug.LogException(exception);
                EditorApplication.Exit(1);
            }
        }
    }
}
