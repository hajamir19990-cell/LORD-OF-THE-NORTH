using UnityEngine;
using UnityEngine.SceneManagement;

namespace LOTN
{
    public sealed class LOTNWorldMap : MonoBehaviour
    {
        private static readonly string[] Names = { "Winterfell", "Dreadfort", "White Harbor", "Bear Island", "Karhold", "Last Hearth", "Deepwood", "Greywater", "The Wall" };
        private static readonly string[] Scenes = { "WinterfellApproach", "LOTN_TheDreadfort", "LOTN_WhiteHarbor", "LOTN_BearIsland", "LOTN_Karhold", "LOTN_LastHearth", "LOTN_DeepwoodMotte", "LOTN_GreywaterWatch", "LOTN_TheWall" };
        private static readonly int[] Unlocks = { 0, 30, 15, 45, 25, 20, 35, 60, 50 };
        private static readonly Vector2[] MapPositions =
        {
            new Vector2(0.50f, 0.50f), new Vector2(0.82f, 0.62f), new Vector2(0.72f, 0.18f),
            new Vector2(0.12f, 0.20f), new Vector2(0.91f, 0.38f), new Vector2(0.72f, 0.82f),
            new Vector2(0.22f, 0.48f), new Vector2(0.32f, 0.84f), new Vector2(0.50f, 0.08f)
        };
        private static readonly int[,] Routes =
        {
            { 0,1 }, { 0,2 }, { 0,4 }, { 0,5 }, { 0,6 }, { 0,7 }, { 0,8 },
            { 1,2 }, { 1,4 }, { 2,3 }, { 3,6 }, { 4,5 }, { 4,8 }, { 5,8 }, { 6,7 }
        };
        private bool open;

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.M)) open = !open;
            if (Input.touchCount == 3 && Input.GetTouch(0).phase == TouchPhase.Began) open = !open;
        }

        private void OnGUI()
        {
            LOTNPlayerStats stats = FindFirstObjectByType<LOTNPlayerStats>();
            int renown = stats != null ? stats.renown : 0;

            GUI.color = new Color(0.10f, 0.13f, 0.16f, 0.92f);
            GUI.Box(new Rect(Screen.width - 150, 18, 132, 44), GUIContent.none);
            GUI.color = new Color(0.96f, 0.91f, 0.79f);
            if (GUI.Button(new Rect(Screen.width - 144, 24, 120, 32), open ? "CLOSE MAP" : "WORLD MAP  [M]")) open = !open;

            if (stats != null)
            {
                GUI.color = new Color(0.08f, 0.11f, 0.14f, 0.90f);
                GUI.Box(new Rect(Screen.width - 166, 70, 148, 66), GUIContent.none);
                GUI.color = new Color(0.94f, 0.89f, 0.77f);
                GUI.Label(new Rect(Screen.width - 156, 76, 132, 54),
                    "HP " + stats.health + "   STA " + stats.stamina +
                    "\nGOLD " + stats.gold + "   RENOWN " + stats.renown);
            }
            if (!open) { GUI.color = Color.white; return; }

            float w = Mathf.Min(520, Screen.width - 24);
            float h = Mathf.Min(560, Screen.height - 30);
            Rect panel = new Rect((Screen.width - w) / 2, (Screen.height - h) / 2, w, h);
            GUI.color = new Color(0.12f, 0.11f, 0.10f, 0.97f);
            GUI.Box(panel, GUIContent.none);
            GUI.color = new Color(0.88f, 0.79f, 0.62f);
            GUI.Label(new Rect(panel.x + 18, panel.y + 12, w - 36, 28), "THE NORTH  •  REGIONAL ROUTES  •  RENOWN " + renown);

            Rect mapArea = new Rect(panel.x + 14, panel.y + 48, w - 28, h - 112);
            DrawRoutes(mapArea);

            for (int i = 0; i < Names.Length; i++)
            {
                bool unlocked = renown >= Unlocks[i];
                Vector2 point = new Vector2(
                    mapArea.x + MapPositions[i].x * mapArea.width,
                    mapArea.y + (1f - MapPositions[i].y) * mapArea.height);
                Rect node = new Rect(point.x - 43f, point.y - 19f, 86f, 38f);
                GUI.enabled = unlocked;
                GUI.color = unlocked ? new Color(0.91f, 0.84f, 0.68f) : new Color(0.48f, 0.50f, 0.52f);
                string label = Names[i] + (unlocked ? "" : "\nLOCK " + Unlocks[i]);
                if (GUI.Button(node, label))
                {
                    open = false;
                    SceneManager.LoadScene(Scenes[i]);
                }
            }

            GUI.enabled = true;
            GUI.color = new Color(0.82f, 0.76f, 0.64f);
            GUI.Label(new Rect(panel.x + 18, panel.yMax - 54, w - 36, 38),
                "Select an unlocked region to travel. Routes show known connections.");
            GUI.color = Color.white;
        }

        private void DrawRoutes(Rect area)
        {
            for (int i = 0; i < Routes.GetLength(0); i++)
            {
                Vector2 a = MapPositions[Routes[i, 0]];
                Vector2 b = MapPositions[Routes[i, 1]];
                Vector2 start = new Vector2(area.x + a.x * area.width, area.y + (1f - a.y) * area.height);
                Vector2 end = new Vector2(area.x + b.x * area.width, area.y + (1f - b.y) * area.height);
                DrawLine(start, end, new Color(0.65f, 0.54f, 0.35f, 0.65f), 2.2f);
            }
        }

        private static void DrawLine(Vector2 start, Vector2 end, Color color, float thickness)
        {
            Vector2 delta = end - start;
            float angle = Mathf.Atan2(delta.y, delta.x) * Mathf.Rad2Deg;
            Matrix4x4 previous = GUI.matrix;
            GUIUtility.RotateAroundPivot(angle, start);
            GUI.color = color;
            GUI.DrawTexture(new Rect(start.x, start.y - thickness * 0.5f, delta.magnitude, thickness), Texture2D.whiteTexture);
            GUI.matrix = previous;
        }
    }
}
