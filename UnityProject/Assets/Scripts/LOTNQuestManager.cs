using UnityEngine;

namespace LOTN
{
    public sealed class LOTNQuestManager : MonoBehaviour
    {
        public static LOTNQuestManager Instance { get; private set; }
        public string activeQuest = "Winterfell_Delivery";
        public string questTitle = "Winterfell Supply Run";
        public string objective = "Collect the supply crate";
        public bool crateCollected;
        public bool questCompleted;

        private void Awake() { Instance = this; }

        public void Interact(string interactionId)
        {
            if (interactionId == "QuestGiver" && !questCompleted)
            {
                objective = crateCollected ? "Return to the Northern Guard" : "Collect the supply crate";
                return;
            }
            if (interactionId == "SupplyCrate" && !crateCollected && !questCompleted)
            {
                crateCollected = true;
                objective = "Return to the Northern Guard";
                return;
            }
            if (interactionId == "QuestGiver" && crateCollected && !questCompleted)
            {
                questCompleted = true;
                objective = "Quest complete — the North remembers";
                LOTNPlayerStats stats = FindFirstObjectByType<LOTNPlayerStats>();
                if (stats != null) { stats.AddGold(30); stats.AddRenown(10); }
            }
        }

        private void OnGUI()
        {
            GUI.color = new Color(0.08f, 0.11f, 0.14f, 0.86f);
            GUI.Box(new Rect(18, 18, 310, 86), GUIContent.none);
            GUI.color = new Color(0.95f, 0.91f, 0.78f);
            GUI.Label(new Rect(32, 27, 285, 26), questTitle);
            GUI.Label(new Rect(32, 56, 285, 38), objective);
            GUI.color = Color.white;
        }
    }
}
