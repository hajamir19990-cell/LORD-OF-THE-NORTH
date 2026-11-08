using UnityEngine;

namespace LOTN
{
    [System.Serializable]
    public sealed class LOTNPlayerStats : MonoBehaviour
    {
        [Min(1)] public int maxHealth = 100;
        [Min(0)] public int health = 100;
        [Min(0)] public int stamina = 100;
        [Min(0)] public int gold = 25;
        [Min(0)] public int renown = 0;

        public bool SpendStamina(int amount)
        {
            if (amount < 0 || stamina < amount) return false;
            stamina -= amount;
            return true;
        }

        public void RestoreStamina(int amount) => stamina = Mathf.Clamp(stamina + Mathf.Max(0, amount), 0, 100);
        public void AddGold(int amount) { if (amount > 0) gold += amount; }
        public void AddRenown(int amount) { if (amount > 0) renown += amount; }
        public void Damage(int amount) { if (amount > 0) health = Mathf.Max(0, health - amount); }
    }
}
