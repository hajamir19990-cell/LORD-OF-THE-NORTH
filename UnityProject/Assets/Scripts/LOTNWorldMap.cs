using UnityEngine;
using UnityEngine.SceneManagement;

namespace LOTN
{
    public sealed class LOTNWorldMap : MonoBehaviour
    {
        private static readonly string[] Names = { "Winterfell", "The Dreadfort", "White Harbor", "Bear Island", "Karhold", "Last Hearth", "Deepwood Motte", "Greywater Watch", "The Wall" };
        private static readonly string[] Scenes = { "WinterfellApproach", "LOTN_TheDreadfort", "LOTN_WhiteHarbor", "LOTN_BearIsland", "LOTN_Karhold", "LOTN_LastHearth", "LOTN_DeepwoodMotte", "LOTN_GreywaterWatch", "LOTN_TheWall" };
        private static readonly int[] Unlocks = { 0, 30, 15, 45, 25, 20, 35, 60, 50 };
        private bool open;

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.M)) open = !open;
            if (Input.touchCount == 3 && Input.GetTouch(0).phase == TouchPhase.Began) open = !open;
        }

        private void OnGUI()
        {
            int renown = 0;
            LOTNPlayerStats stats = FindFirstObjectByType<LOTNPlayerStats>();
            if (stats != null) renown = stats.renown;
            GUI.color = new Color(0.10f, 0.13f, 0.16f, 0.92f);
            GUI.Box(new Rect(Screen.width - 150, 18, 132, 44), GUIContent.none);
            GUI.color = new Color(0.96f, 0.91f, 0.79f);
            if (GUI.Button(new Rect(Screen.width - 144, 24, 120, 32), open ? "CLOSE MAP" : "WORLD MAP  [M]")) open = !open;
            LOTNPlayerStats hudStats = FindFirstObjectByType<LOTNPlayerStats>();
            if (hudStats != null)
            {
                GUI.color = new Color(0.08f, 0.11f, 0.14f, 0.90f);
                GUI.Box(new Rect(Screen.width - 166, 70, 148, 66), GUIContent.none);
                GUI.color = new Color(0.94f, 0.89f, 0.77f);
                GUI.Label(new Rect(Screen.width - 156, 76, 132, 54),
                    "HP " + hudStats.health + "   STA " + hudStats.stamina +
                    "\\nGOLD " + hudStats.gold + "   RENOWN " + hudStats.renown);
            }
            if (!open) { GUI.color = Color.white; return; }

            float w = Mathf.Min(430, Screen.width - 30);
            float h = Mathf.Min(480, Screen.height - 50);
            Rect panel = new Rect((Screen.width - w) / 2, (Screen.height - h) / 2, w, h);
            GUI.color = new Color(0.07f, 0.09f, 0.12f, 0.97f);
            GUI.Box(panel, GUIContent.none);
            GUI.color = new Color(0.94f, 0.89f, 0.77f);
            GUI.Label(new Rect(panel.x + 18, panel.y + 14, w - 36, 28), "THE NORTH  •  RENOWN " + renown);
            for (int i = 0; i < Names.Length; i++)
            {
                bool unlocked = renown >= Unlocks[i];
                GUI.enabled = unlocked;
                GUI.color = unlocked ? new Color(0.9f, 0.88f, 0.79f) : new Color(0.55f, 0.57f, 0.6f);
                string label = Names[i] + (unlocked ? "  —  TRAVEL" : "  —  LOCKED (" + Unlocks[i] + ")");
                if (GUI.Button(new Rect(panel.x + 18, panel.y + 50 + i * 42, w - 36, 34), label))
                {
                    open = false;
                    SceneManager.LoadScene(Scenes[i]);
                }
            }
            GUI.enabled = true;
            GUI.color = Color.white;
        }
    }
}
