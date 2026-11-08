using System.IO;
using System.Linq;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;
using UnityEngine.SceneManagement;
using LOTN;

namespace LOTN.Editor
{
    public static class LOTNProjectBootstrap
    {
        private const string SceneFolder = "Assets/Scenes";
        private const string GeneratedFolder = "Assets/Generated/Terrain";
        private const string MeshFolder = "Assets/Art/SourceOBJ";

        public static void PreExport()
        {
            ConfigurePlayerSettings();
            BuildPrototypeScenes();
        }

        public static void ConfigurePlayerSettings()
        {
            PlayerSettings.companyName = "LORD OF THE NORTH";
            PlayerSettings.productName = "LORD OF THE NORTH";
            PlayerSettings.bundleVersion = "0.1.0";
            PlayerSettings.SetApplicationIdentifier(BuildTargetGroup.Android, "com.lotn.northerngame");
            PlayerSettings.defaultInterfaceOrientation = UIOrientation.LandscapeLeft;
            PlayerSettings.colorSpace = ColorSpace.Linear;
            PlayerSettings.Android.targetArchitectures = AndroidArchitecture.ARM64;
            PlayerSettings.Android.minSdkVersion = AndroidSdkVersions.AndroidApiLevel26;
            PlayerSettings.Android.bundleVersionCode = 1;
            PlayerSettings.SetScriptingBackend(BuildTargetGroup.Android, ScriptingImplementation.IL2CPP);
            PlayerSettings.SetManagedStrippingLevel(BuildTargetGroup.Android, ManagedStrippingLevel.Medium);
        }

        [MenuItem("Tools/LORD OF THE NORTH/Generate all nine regional scenes")]
        public static void BuildPrototypeScenes()
        {
            EnsureFolder("Assets/Generated");
            EnsureFolder("Assets/Settings");
            EnsureURPAssets();
            EnsureFolder(GeneratedFolder);
            EnsureFolder(SceneFolder);
            AssetDatabase.Refresh(ImportAssetOptions.ForceSynchronousImport);
            ConvertImportedMaterialsToURP();

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

        private static void EnsureURPAssets()
        {
            const string rendererPath = "Assets/Settings/LOTN_UniversalRenderer.asset";
            const string pipelinePath = "Assets/Settings/LOTN_URP_Mobile.asset";

            UniversalRendererData rendererData = AssetDatabase.LoadAssetAtPath<UniversalRendererData>(rendererPath);
            if (rendererData == null)
            {
                rendererData = ScriptableObject.CreateInstance<UniversalRendererData>();
                rendererData.name = "LOTN Universal Renderer";
                rendererData.renderingMode = RenderingMode.Forward;
                AssetDatabase.CreateAsset(rendererData, rendererPath);
            }

            UniversalRenderPipelineAsset pipeline = AssetDatabase.LoadAssetAtPath<UniversalRenderPipelineAsset>(pipelinePath);
            if (pipeline == null)
            {
                pipeline = UniversalRenderPipelineAsset.Create(rendererData);
                pipeline.name = "LOTN URP Mobile";
                AssetDatabase.CreateAsset(pipeline, pipelinePath);
            }

            pipeline.renderScale = 0.9f;
            pipeline.supportsHDR = false;
            pipeline.supportsMainLightShadows = true;
            pipeline.supportsSoftShadows = false;
            pipeline.shadowDistance = 65f;
            pipeline.msaaSampleCount = 2;
            pipeline.useSRPBatcher = true;
            pipeline.EnsureGlobalSettings();
            GraphicsSettings.defaultRenderPipeline = pipeline;
            QualitySettings.renderPipeline = pipeline;
            EditorUtility.SetDirty(rendererData);
            EditorUtility.SetDirty(pipeline);
            AssetDatabase.SaveAssets();
        }

        private static void ConvertImportedMaterialsToURP()
        {
            Shader urpLit = Shader.Find("Universal Render Pipeline/Lit");
            if (urpLit == null) throw new System.Exception("URP Lit shader is unavailable; verify the URP package import.");
            string[] materialGuids = AssetDatabase.FindAssets("t:Material", new[] { MeshFolder });
            foreach (string guid in materialGuids)
            {
                string path = AssetDatabase.GUIDToAssetPath(guid);
                Material material = AssetDatabase.LoadAssetAtPath<Material>(path);
                if (material == null || material.shader == urpLit) continue;
                Color color = material.HasProperty("_Color") ? material.GetColor("_Color") : Color.white;
                Texture texture = material.HasProperty("_MainTex") ? material.GetTexture("_MainTex") : null;
                material.shader = urpLit;
                if (material.HasProperty("_BaseColor")) material.SetColor("_BaseColor", color);
                if (material.HasProperty("_BaseMap")) material.SetTexture("_BaseMap", texture);
                EditorUtility.SetDirty(material);
            }
            AssetDatabase.SaveAssets();
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
            data.terrainLayers = new[] { CreateSnowLayer() };
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

        private static TerrainLayer CreateSnowLayer()
        {
            const string texturePath = "Assets/Generated/Terrain/Northern_Snow_Albedo.png";
            const string layerPath = "Assets/Generated/Terrain/Northern_Snow.terrainlayer";
            Texture2D texture = AssetDatabase.LoadAssetAtPath<Texture2D>(texturePath);
            if (texture == null)
            {
                Texture2D generated = new Texture2D(128, 128, TextureFormat.RGB24, false, true);
                Color[] pixels = new Color[128 * 128];
                for (int y = 0; y < 128; y++)
                for (int x = 0; x < 128; x++)
                {
                    float grain = Mathf.PerlinNoise(x * 0.19f, y * 0.19f) * 0.07f;
                    float fleck = ((x * 17 + y * 31) % 13) / 13f * 0.025f;
                    float value = Mathf.Clamp(0.82f + grain + fleck, 0.78f, 0.98f);
                    pixels[y * 128 + x] = new Color(value * 0.96f, value * 0.985f, value, 1f);
                }
                generated.SetPixels(pixels);
                generated.Apply();
                string absoluteTexturePath = Path.Combine(Application.dataPath, "Generated/Terrain/Northern_Snow_Albedo.png");
                File.WriteAllBytes(absoluteTexturePath, generated.EncodeToPNG());
                Object.DestroyImmediate(generated);
                AssetDatabase.ImportAsset(texturePath, ImportAssetOptions.ForceUpdate);
                texture = AssetDatabase.LoadAssetAtPath<Texture2D>(texturePath);
            }

            TerrainLayer layer = AssetDatabase.LoadAssetAtPath<TerrainLayer>(layerPath);
            if (layer == null)
            {
                layer = new TerrainLayer();
                layer.diffuseTexture = texture;
                layer.tileSize = new Vector2(24f, 24f);
                AssetDatabase.CreateAsset(layer, layerPath);
            }
            return layer;
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
            bool fortress = region == 0 || region == 1 || region == 4 || region == 8;
            if (fortress)
                AddModel("Winterfell_Gatehouse.obj", new Vector3(0, 0, region == 8 ? 315 : 265), region == 8 ? 78f : 95f,
                    region == 0 ? "Winterfell Gatehouse" : "Northern Fortress Gatehouse");

            if (region == 8)
            {
                AddModel("Northern_Curtain_Wall.obj", new Vector3(-300, 0, 310), 210f, "The Wall — Western Reach");
                AddModel("Northern_Curtain_Wall.obj", new Vector3(0, 0, 330), 230f, "The Wall — Central Reach");
                AddModel("Northern_Curtain_Wall.obj", new Vector3(300, 0, 310), 210f, "The Wall — Eastern Reach");
            }
            else if (fortress)
            {
                AddModel("Northern_Curtain_Wall.obj", new Vector3(-105, 0, 245), 105f, "Western Granite Curtain Wall");
                AddModel("Northern_Curtain_Wall.obj", new Vector3(105, 0, 245), 105f, "Eastern Granite Curtain Wall");
            }

            AddModel("Northern_Watchtower.obj", new Vector3(-235, 0, 225), region == 3 || region == 6 ? 44f : 36f, "Western Watchtower");
            AddModel("Northern_Watchtower.obj", new Vector3(235, 0, 225), region == 3 || region == 6 ? 44f : 36f, "Eastern Watchtower");
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
            if (region == 2) AddWaterBody("White Harbor Inlet", new Vector3(0f, -1.9f, 405f), new Vector3(8f, 1f, 2.8f));
            if (region == 7)
            {
                AddWaterBody("Greywater Watch — East Channel", new Vector3(245f, -1.8f, 155f), new Vector3(2.8f, 1f, 8f));
                AddWaterBody("Greywater Watch — West Channel", new Vector3(-235f, -1.8f, 115f), new Vector3(2.4f, 1f, 7f));
            }

            int treeCount = region == 3 || region == 6 ? 36 : (region == 7 || region == 2 ? 14 : 24);
            for (int i = 0; i < treeCount; i++)
            {
                float side = (i % 2 == 0) ? -1f : 1f;
                float z = -160f + (i / 2) * 30f;
                float x = side * (300f + (i % 3) * 24f);
                AddModel("Northern_Pine_Tree.obj", new Vector3(x, 0, z), 22f + (i % 4) * 2.5f, "Snow-Capped Northern Pine");
            }

            if (region != 0) return;

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

        private static void AddWaterBody(string name, Vector3 position, Vector3 scale)
        {
            GameObject water = GameObject.CreatePrimitive(PrimitiveType.Plane);
            water.name = name;
            water.transform.position = position;
            water.transform.localScale = scale;
            Collider collider = water.GetComponent<Collider>();
            if (collider != null) Object.DestroyImmediate(collider);
            water.GetComponent<Renderer>().sharedMaterial = MakeMaterial(new Color(0.12f, 0.23f, 0.31f));
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
            GameObject session = new GameObject("LOTN Persistent Session");
            session.AddComponent<LOTNGameSession>();

            GameObject systems = new GameObject("LOTN Game Systems");
            systems.AddComponent<LOTNTouchInput>();
            systems.AddComponent<LOTNQuestManager>();
            systems.AddComponent<LOTNInventory>();
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
