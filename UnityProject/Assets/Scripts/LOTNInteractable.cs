using UnityEngine;

namespace LOTN
{
    public sealed class LOTNInteractable : MonoBehaviour
    {
        public string interactionId = "QuestGiver";
        public string prompt = "Interact";
        public float range = 3.2f;
        private Transform player;

        private void Update()
        {
            if (player == null)
            {
                GameObject p = GameObject.FindGameObjectWithTag("Player");
                if (p != null) player = p.transform;
            }
            if (player == null || Vector3.Distance(player.position, transform.position) > range) return;
            bool pressed = LOTNTouchInput.InteractPressed || Input.GetKeyDown(KeyCode.E);
            if (pressed && LOTNQuestManager.Instance != null)
                LOTNQuestManager.Instance.Interact(interactionId);
        }

        private void OnGUI()
        {
            if (player == null || Vector3.Distance(player.position, transform.position) > range) return;
            GUI.color = new Color(0.98f, 0.91f, 0.72f);
            GUI.Label(new Rect(Screen.width * 0.5f - 100, Screen.height * 0.72f, 220, 34), "USE  •  " + prompt);
            GUI.color = Color.white;
        }
    }
}
