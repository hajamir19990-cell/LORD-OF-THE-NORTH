using UnityEngine;

namespace LOTN
{
    public sealed class LOTNGameSession : MonoBehaviour
    {
        public static LOTNGameSession Instance { get; private set; }
        public int health = 100;
        public int stamina = 100;
        public int gold = 25;
        public int renown = 0;
        public bool crateCollected;
        public bool questCompleted;

        private void Awake()
        {
            if (Instance != null && Instance != this) { Destroy(gameObject); return; }
            Instance = this;
            DontDestroyOnLoad(gameObject);
            Load();
        }

        public void SetPlayerState(int newHealth, int newStamina, int newGold, int newRenown)
        {
            health = Mathf.Clamp(newHealth, 0, 100);
            stamina = Mathf.Clamp(newStamina, 0, 100);
            gold = Mathf.Max(0, newGold);
            renown = Mathf.Max(0, newRenown);
            Save();
        }

        public void SetQuestState(bool collected, bool completed)
        {
            crateCollected = collected;
            questCompleted = completed;
            Save();
        }

        private void Load()
        {
            health = PlayerPrefs.GetInt("LOTN.Health", 100);
            stamina = PlayerPrefs.GetInt("LOTN.Stamina", 100);
            gold = PlayerPrefs.GetInt("LOTN.Gold", 25);
            renown = PlayerPrefs.GetInt("LOTN.Renown", 0);
            crateCollected = PlayerPrefs.GetInt("LOTN.Crate", 0) == 1;
            questCompleted = PlayerPrefs.GetInt("LOTN.QuestDone", 0) == 1;
        }

        public void Save()
        {
            PlayerPrefs.SetInt("LOTN.Health", health);
            PlayerPrefs.SetInt("LOTN.Stamina", stamina);
            PlayerPrefs.SetInt("LOTN.Gold", gold);
            PlayerPrefs.SetInt("LOTN.Renown", renown);
            PlayerPrefs.SetInt("LOTN.Crate", crateCollected ? 1 : 0);
            PlayerPrefs.SetInt("LOTN.QuestDone", questCompleted ? 1 : 0);
            PlayerPrefs.Save();
        }

        private void OnApplicationPause(bool paused) { if (paused) Save(); }
        private void OnApplicationQuit() { Save(); }
    }
}
