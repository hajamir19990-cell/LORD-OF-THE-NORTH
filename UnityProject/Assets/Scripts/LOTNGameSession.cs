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
            Application.targetFrameRate = 60;
            QualitySettings.vSyncCount = 0;
            Load();
        }

        public void SetPlayerState(int newHealth, int newStamina, int newGold, int newRenown)
        {
            int safeHealth = Mathf.Clamp(newHealth, 0, 100);
            int safeGold = Mathf.Max(0, newGold);
            int safeRenown = Mathf.Max(0, newRenown);
            bool persistentValueChanged = health != safeHealth || gold != safeGold || renown != safeRenown;
            health = safeHealth;
            stamina = Mathf.Clamp(newStamina, 0, 100);
            gold = safeGold;
            renown = safeRenown;
            if (persistentValueChanged) Save();
        }

        public void SetQuestState(bool collected, bool completed)
        {
            crateCollected = collected;
            questCompleted = completed;
            Save();
        }

        public void SavePlayerPosition(string sceneName, Vector3 position)
        {
            PlayerPrefs.SetString("LOTN.LastScene", sceneName);
            PlayerPrefs.SetFloat("LOTN.PosX", position.x);
            PlayerPrefs.SetFloat("LOTN.PosY", position.y);
            PlayerPrefs.SetFloat("LOTN.PosZ", position.z);
            PlayerPrefs.Save();
        }

        public bool TryGetPlayerPosition(string sceneName, out Vector3 position)
        {
            if (PlayerPrefs.GetString("LOTN.LastScene", "") == sceneName)
            {
                position = new Vector3(
                    PlayerPrefs.GetFloat("LOTN.PosX", 0f),
                    PlayerPrefs.GetFloat("LOTN.PosY", 4f),
                    PlayerPrefs.GetFloat("LOTN.PosZ", -345f));
                return true;
            }
            position = Vector3.zero;
            return false;
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
        private void OnDestroy() { if (Instance == this) Instance = null; }
    }
}
