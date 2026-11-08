using UnityEngine;

namespace LOTN
{
    public sealed class LOTNPlayerStats : MonoBehaviour
    {
        [Min(1)] public int maxHealth = 100;
        [Min(0)] public int health = 100;
        [Min(0)] public int stamina = 100;
        [Min(0)] public int gold = 25;
        [Min(0)] public int renown = 0;

        private void Awake()
        {
            LOTNGameSession session = LOTNGameSession.Instance;
            if (session == null) return;
            health = session.health;
            stamina = session.stamina;
            gold = session.gold;
            renown = session.renown;
        }

        public bool SpendStamina(int amount)
        {
            if (amount < 0 || stamina < amount) return false;
            stamina -= amount;
            Sync();
            return true;
        }

        public void RestoreStamina(int amount)
        {
            stamina = Mathf.Clamp(stamina + Mathf.Max(0, amount), 0, 100);
            Sync();
        }

        public void AddGold(int amount) { if (amount > 0) { gold += amount; Sync(); } }
        public void AddRenown(int amount) { if (amount > 0) { renown += amount; Sync(); } }
        public void Damage(int amount) { if (amount > 0) { health = Mathf.Max(0, health - amount); Sync(); } }

        private void Sync()
        {
            LOTNGameSession session = LOTNGameSession.Instance;
            if (session != null) session.SetPlayerState(health, stamina, gold, renown);
        }
    }
}
