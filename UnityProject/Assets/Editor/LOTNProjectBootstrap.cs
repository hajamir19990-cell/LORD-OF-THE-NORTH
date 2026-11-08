using System.IO;
using System.Linq;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;
using LOTN;

namespace LOTN.Editor
{
    public static class LOTNProjectBootstrap
    {
        private const string SceneFolder = "Assets/Scenes";
        private const string GeneratedFolder = "Assets/Generated/Terrain";
        private const string MeshFolder = "Assets/Art/SourceOBJ";

        public static void BuildPrototypeScenes()
        {
            EnsureFolder("Assets/Generated");
            EnsureFolder(GeneratedFolder);
            EnsureFolder(SceneFolder);

            string[] sceneNames = LOTNRegionData.SceneNames;
            var buildScenes = new EditorBuildSettingsScene[sceneNames.Length];
            for (int i = 0; i < sceneNames.Length; i++)
            {
                string path = SceneFolder + "/" + sceneNames[i] + ".unity";
                BuildScene(sceneNames[i], i);
                buildScenes[i] = new EditorBuildSettingsScene(path, true);
            }
            EditorBuildSettings.scenes = buildScenes;
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log("LOTN: created and registered " + sceneNames.Length + " landscape scenes.");
        }

        private static void BuildScene(string sceneName, int regionIndex)
        {
            Scene scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
            RenderSettings.ambientMode = UnityEngine.Rendering.AmbientMode.Flat;
            RenderSettings.ambientLight = new Color(0.34f, 0.39f, 0.46f);
            RenderSettings.fog = true;
            RenderSettings.fogColor = new Color(0.52f, 0.59f, 0.67f);
            RenderSettings.fogMode = FogMode.Exponential;
            RenderSettings.fogDensity = 0.0012f;

            CreateTerrain(sceneName, regionIndex);
            CreateLight();
            CreateEnvironment(regionIndex);
            CreateGameplay();
            EditorSceneManager.SaveScene(scene, SceneFolder + "/" + sceneName + ".unity");
        }

        private static void CreateTerrain(string sceneName, int seed)
        {
            TerrainData data = new TerrainData { heightmapResolution = 65, size = new Vector3(1000f, 95f, 1000f) };
            float[,] heights = new float[65, 65];
            for (int z = 0; z < 65; z++)
            for (int x = 0; x < 65; x++)
            {
                float nx = x / 64f, nz = z / 64f;
                float noise = Mathf.PerlinNoise(nx * 3.2f + seed * 0.71f, nz * 3.2f + seed * 0.39f);
                float ridge = Mathf.PerlinNoise(nx * 7.1f + 13f + seed, nz * 6.4f + 5f) * 0.018f;
                float centerFlatten = Mathf.Clamp01(Mathf.Min(Mathf.Abs(nx - 0.5f) * 4f, Mathf.Abs(nz - 0.42f) * 5f));
                heights[z, x] = Mathf.Lerp(0.018f, 0.07f + noise * 0.075f + ridge, centerFlatten);
            }
            data.SetHeights(0, 0, heights);
            string assetPath = GeneratedFolder + "/" + sceneName + "_Terrain.asset";
            AssetDatabase.DeleteAsset(assetPath);
            AssetDatabase.CreateAsset(data, assetPath);
            Terrain terrain = Terrain.CreateTerrainGameObject(data).GetComponent<Terrain>();
            terrain.transform.position = new Vector3(-500f, -4f, -500f);
            terrain.heightmapPixelError = 12;
            terrain.basemapDistance = 450f;
            terrain.drawInstanced = true;
            terrain.gameObject.name = "Northern Rolling Snowfield";
        }

        private static void CreateLight()
        {
            GameObject sun = new GameObject("Cold Northern Daylight");
            Light light = sun.AddComponent<Light>();
            light.type = LightType.Directional;
            light.color = new Color(0.76f, 0.83f, 0.94f);
            light.intensity = 1.15f;
            light.shadows = LightShadows.Soft;
            sun.transform.rotation = Quaternion.Euler(42f, -32f, 0f);
        }

        private static void CreateEnvironment(int region)
        {
            bool winterfell = region == 0;
            AddModel("Winterfell_Gatehouse.obj", new Vector3(0, 0, 265), 95f, winterfell ? "Winterfell Gatehouse" : "Regional Keep Landmark");
            AddModel("Northern_Curtain_Wall.obj", new Vector3(-105, 0, 245), 105f, "Western Granite Curtain Wall");
            AddModel("Northern_Curtain_Wall.obj", new Vector3(105, 0, 245), 105f, "Eastern Granite Curtain Wall");
            AddModel("Northern_Watchtower.obj", new Vector3(-235, 0, 225), 36f, "Western Watchtower");
            AddModel("Northern_Watchtower.obj", new Vector3(235, 0, 225), 36f, "Eastern Watchtower");
            AddModel("Northern_Cobblestone_Road.obj", new Vector3(0, 0, -55), 28f, "Snowbound Approach Road");
            AddModel("Northern_Longhouse.obj", new Vector3(-170, 0, -95), 34f, "Western Longhouse");
            AddModel("Northern_Longhouse.obj", new Vector3(170, 0, -105), 34f, "Eastern Longhouse");
            AddModel("Northern_Palisade_Fence.obj", new Vector3(-125, 0, -230), 36f, "Western Timber Palisade");
            AddModel("Northern_Palisade_Fence.obj", new Vector3(125, 0, -230), 36f, "Eastern Timber Palisade");
            AddModel("Northern_Wolf_Banner.obj", new Vector3(-48, 0, 205), 16f, "Northern Wolf Standard");
            AddModel("Northern_Wolf_Banner.obj", new Vector3(48, 0, 205), 16f, "Northern Wolf Standard");
            AddModel("Northern_Stone_Firepit.obj", new Vector3(185, 0, -170), 12f, "Settlement Firepit");
            AddModel("Northern_Handcart.obj", new Vector3(205, 0, -115), 18f, "Supply Handcart");
            AddModel("Northern_Woodpile.obj", new Vector3(-195, 0, -145), 14f, "Stacked Firewood");
            AddModel("Northern_Snowdrift.obj", new Vector3(-30, 0, -15), 25f, "Roadside Snowdrift");
            AddModel("Northern_Rock_Cluster.obj", new Vector3(-340, 0, 40), 24f, "Snow-Capped Rocks");
            AddModel("Northern_Rock_Cluster.obj", new Vector3(340, 0, 65), 24f, "Snow-Capped Rocks");
            for (int i = 0; i < 24; i++)
            {
                float side = (i % 2 == 0) ? -1f : 1f;
                float z = -120f + (i / 2) * 38f;
                float x = side * (300f + (i % 3) * 24f);
                AddModel("Northern_Pine_Tree.obj", new Vector3(x, 0, z), 22f + (i % 4) * 2.5f, "Snow-Capped Northern Pine");
            }

            GameObject npc = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            npc.name = "Northern Guard — Quest Giver";
            npc.transform.position = new Vector3(-2f, 2f, -275f);
            npc.transform.localScale = new Vector3(0.8f, 1.1f, 0.8f);
            npc.GetComponent<Renderer>().sharedMaterial = MakeMaterial(new Color(0.22f, 0.29f, 0.38f));
            LOTNInteractable giver = npc.AddComponent<LOTNInteractable>();
            giver.interactionId = "QuestGiver";
            giver.prompt = "Speak to the Northern Guard";

            GameObject crate = AddModel("Northern_Supply_Crate.obj", new Vector3(15f, 0f, -90f), 13f, "Winterfell Supply Crate");
            if (crate != null)
            {
                BoxCollider box = crate.AddComponent<BoxCollider>();
                box.isTrigger = true;
                LOTNInteractable objective = crate.AddComponent<LOTNInteractable>();
                objective.interactionId = "SupplyCrate";
                objective.prompt = "Collect the supply crate";
            }
        }

        private static GameObject AddModel(string file, Vector3 position, float targetSize, string objectName)
        {
            string path = MeshFolder + "/" + file;
            GameObject source = AssetDatabase.LoadAssetAtPath<GameObject>(path);
            if (source == null) { Debug.LogWarning("LOTN missing source model: " + path); return null; }
            GameObject instance = (GameObject)PrefabUtility.InstantiatePrefab(source);
            instance.name = objectName;
            instance.transform.position = position;
            instance.transform.rotation = Quaternion.identity;
            instance.transform.localScale = Vector3.one;
            Renderer[] renderers = instance.GetComponentsInChildren<Renderer>();
            if (renderers.Length > 0)
            {
                Bounds bounds = renderers[0].bounds;
                for (int i = 1; i < renderers.Length; i++) bounds.Encapsulate(renderers[i].bounds);
                float largest = Mathf.Max(bounds.size.x, bounds.size.y, bounds.size.z);
                if (largest > 0.001f) instance.transform.localScale = Vector3.one * (targetSize / largest);
            }
            return instance;
        }

        private static void CreateGameplay()
        {
            GameObject systems = new GameObject("LOTN Game Systems");
            systems.AddComponent<LOTNTouchInput>();
            systems.AddComponent<LOTNQuestManager>();
            systems.AddComponent<LOTNWorldMap>();

            GameObject player = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            player.name = "LOTN Player — Temporary Greybox";
            player.tag = "Player";
            player.transform.position = new Vector3(0f, 4f, -345f);
            player.transform.localScale = new Vector3(0.8f, 1f, 0.8f);
            Object.DestroyImmediate(player.GetComponent<CapsuleCollider>());
            player.AddComponent<CharacterController>();
            player.AddComponent<LOTNPlayerStats>();
            player.AddComponent<LOTNPlayerController>();
            player.GetComponent<Renderer>().sharedMaterial = MakeMaterial(new Color(0.18f, 0.23f, 0.29f));

            GameObject camera = new GameObject("Main Camera");
            camera.tag = "MainCamera";
            Camera cam = camera.AddComponent<Camera>();
            cam.fieldOfView = 62f;
            cam.nearClipPlane = 0.08f;
            camera.AddComponent<AudioListener>();
            LOTNCameraFollow follow = camera.AddComponent<LOTNCameraFollow>();
            follow.target = player.transform;
        }

        private static Material MakeMaterial(Color color)
        {
            Shader shader = Shader.Find("Universal Render Pipeline/Lit");
            if (shader == null) shader = Shader.Find("Standard");
            Material material = new Material(shader);
            material.color = color;
            return material;
        }

        private static void EnsureFolder(string path)
        {
            if (AssetDatabase.IsValidFolder(path)) return;
            string parent = Path.GetDirectoryName(path).Replace("\\", "/");
            string folder = Path.GetFileName(path);
            if (!AssetDatabase.IsValidFolder(parent)) EnsureFolder(parent);
            AssetDatabase.CreateFolder(parent, folder);
        }
    }
}
