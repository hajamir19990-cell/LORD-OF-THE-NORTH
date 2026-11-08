using System;
using System.Collections.Generic;
using UnityEngine;

namespace LOTN
{
    [Serializable]
    public sealed class LOTNInventoryEntry
    {
        public string id;
        public string label;
        public int quantity;
        public string equipmentSlot;
    }

    [Serializable]
    internal sealed class LOTNInventorySave
    {
        public List<LOTNInventoryEntry> entries = new List<LOTNInventoryEntry>();
        public string cloak = "";
        public string trinket = "";
    }

    public sealed class LOTNInventory : MonoBehaviour
    {
        public static LOTNInventory Instance { get; private set; }
        public int maxSlots = 16;
        public IReadOnlyList<LOTNInventoryEntry> Entries => entries;
        public string EquippedCloak { get; private set; } = "";
        public string EquippedTrinket { get; private set; } = "";

        private readonly List<LOTNInventoryEntry> entries = new List<LOTNInventoryEntry>();
        private bool open;

        private void Awake()
        {
            Instance = this;
            Load();
            if (entries.Count == 0)
            {
                AddItem("northern_cloak", "Northern Wool Cloak", 1, "Cloak");
                AddItem("wolf_pendant", "Wolf Pendant", 1, "Trinket");
                AddItem("dried_rations", "Dried Rations", 3, "");
            }
        }

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.I)) open = !open;
        }

        public bool AddItem(string id, string label, int quantity, string equipmentSlot = "")
        {
            if (string.IsNullOrWhiteSpace(id) || quantity <= 0) return false;
            LOTNInventoryEntry existing = entries.Find(e => e.id == id);
            if (existing != null) existing.quantity += quantity;
            else
            {
                if (entries.Count >= maxSlots) return false;
                entries.Add(new LOTNInventoryEntry { id = id, label = label, quantity = quantity, equipmentSlot = equipmentSlot ?? "" });
            }
            Save();
            return true;
        }

        public bool RemoveItem(string id, int quantity)
        {
            LOTNInventoryEntry existing = entries.Find(e => e.id == id);
            if (existing == null || quantity <= 0 || existing.quantity < quantity) return false;
            existing.quantity -= quantity;
            if (existing.quantity == 0) entries.Remove(existing);
            Save();
            return true;
        }

        public bool Equip(string id)
        {
            LOTNInventoryEntry item = entries.Find(e => e.id == id);
            if (item == null || string.IsNullOrEmpty(item.equipmentSlot)) return false;
            if (item.equipmentSlot == "Cloak") EquippedCloak = item.id;
            else if (item.equipmentSlot == "Trinket") EquippedTrinket = item.id;
            else return false;
            Save();
            return true;
        }

        private void Save()
        {
            LOTNInventorySave data = new LOTNInventorySave { entries = entries, cloak = EquippedCloak, trinket = EquippedTrinket };
            PlayerPrefs.SetString("LOTN.Inventory", JsonUtility.ToJson(data));
            PlayerPrefs.Save();
        }

        private void Load()
        {
            string json = PlayerPrefs.GetString("LOTN.Inventory", "");
            if (string.IsNullOrEmpty(json)) return;
            try
            {
                LOTNInventorySave data = JsonUtility.FromJson<LOTNInventorySave>(json);
                if (data == null) return;
                entries.Clear();
                if (data.entries != null) entries.AddRange(data.entries);
                EquippedCloak = data.cloak ?? "";
                EquippedTrinket = data.trinket ?? "";
            }
            catch (Exception exception) { Debug.LogWarning("LOTN inventory save could not be read: " + exception.Message); }
        }

        private void OnGUI()
        {
            if (GUI.Button(new Rect(18, 116, 132, 34), open ? "CLOSE BAG" : "INVENTORY [I]")) open = !open;
            if (!open) return;
            float w = Mathf.Min(340, Screen.width - 36);
            Rect panel = new Rect(18, 158, w, Mathf.Min(330, Screen.height - 180));
            GUI.color = new Color(0.07f, 0.09f, 0.12f, 0.96f);
            GUI.Box(panel, GUIContent.none);
            GUI.color = new Color(0.94f, 0.89f, 0.77f);
            GUI.Label(new Rect(panel.x + 12, panel.y + 8, w - 24, 24), "INVENTORY  •  " + entries.Count + "/" + maxSlots);
            for (int i = 0; i < entries.Count; i++)
            {
                LOTNInventoryEntry item = entries[i];
                float y = panel.y + 38 + i * 34;
                GUI.Label(new Rect(panel.x + 12, y, w - 145, 28), item.label + " ×" + item.quantity);
                if (!string.IsNullOrEmpty(item.equipmentSlot) && GUI.Button(new Rect(panel.x + w - 124, y, 110, 26), "EQUIP"))
                    Equip(item.id);
            }
            GUI.Label(new Rect(panel.x + 12, panel.yMax - 34, w - 24, 26), "Cloak: " + EquippedCloak + "   Trinket: " + EquippedTrinket);
            GUI.color = Color.white;
        }

        private void OnDestroy() { if (Instance == this) Instance = null; }
    }
}
