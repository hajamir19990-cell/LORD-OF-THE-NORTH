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
        private string dialogueLine = "";
        private float dialogueUntil;

        private void Awake()
        {
            Instance = this;
            LOTNGameSession session = LOTNGameSession.Instance;
            if (session != null)
            {
                crateCollected = session.crateCollected;
                questCompleted = session.questCompleted;
                RefreshObjective();
            }
        }

        public void Interact(string interactionId)
        {
            LOTNGameSession session = LOTNGameSession.Instance;
            if (interactionId == "SupplyCrate" && !crateCollected && !questCompleted)
            {
                crateCollected = true;
                if (LOTNInventory.Instance != null)
                    LOTNInventory.Instance.AddItem("winterfell_supplies", "Winterfell Supply Pack", 1);
                if (session != null) session.SetQuestState(crateCollected, questCompleted);
                ShowDialogue("You collected the Winterfell supply pack.");
                RefreshObjective();
                return;
            }

            if (interactionId == "QuestGiver" && crateCollected && !questCompleted)
            {
                questCompleted = true;
                if (LOTNInventory.Instance != null)
                    LOTNInventory.Instance.RemoveItem("winterfell_supplies", 1);
                if (session != null) session.SetQuestState(crateCollected, questCompleted);
                LOTNPlayerStats stats = FindFirstObjectByType<LOTNPlayerStats>();
                if (stats != null) { stats.AddGold(30); stats.AddRenown(10); }
                ShowDialogue("The Guard: Winterfell thanks you. You earned gold and renown.");
                RefreshObjective();
                return;
            }

            if (interactionId == "QuestGiver" && !questCompleted)
            {
                objective = "The Guard needs supplies from the nearby crate";
                ShowDialogue(crateCollected
                    ? "The Guard: Bring the supply pack back to me."
                    : "The Guard: The winter stores are low. Please collect the supply pack nearby.");
            }
        }

        private void OnDestroy() { if (Instance == this) Instance = null; }

        private void ShowDialogue(string line)
        {
            dialogueLine = line;
            dialogueUntil = Time.time + 4.5f;
        }

        private void RefreshObjective()
        {
            objective = questCompleted ? "Quest complete — the North remembers" :
                (crateCollected ? "Return to the Northern Guard" : "Collect the supply crate");
        }

        private void OnGUI()
        {
            GUI.color = new Color(0.08f, 0.11f, 0.14f, 0.86f);
            GUI.Box(new Rect(18, 18, 310, 86), GUIContent.none);
            GUI.color = new Color(0.95f, 0.91f, 0.78f);
            GUI.Label(new Rect(32, 27, 285, 26), questTitle);
            GUI.Label(new Rect(32, 56, 285, 38), objective);
            if (!string.IsNullOrEmpty(dialogueLine) && Time.time < dialogueUntil)
            {
                float width = Mathf.Min(440f, Screen.width - 40f);
                Rect dialogue = new Rect((Screen.width - width) * 0.5f, 116f, width, 66f);
                GUI.color = new Color(0.08f, 0.10f, 0.13f, 0.94f);
                GUI.Box(dialogue, GUIContent.none);
                GUI.color = new Color(0.96f, 0.91f, 0.79f);
                GUI.Label(new Rect(dialogue.x + 14f, dialogue.y + 10f, width - 28f, 48f), dialogueLine);
            }
            GUI.color = Color.white;
        }
    }
}
