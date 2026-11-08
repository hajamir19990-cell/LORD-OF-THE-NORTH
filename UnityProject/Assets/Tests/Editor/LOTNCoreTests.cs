using NUnit.Framework;
using UnityEngine;
using LOTN;

namespace LOTN.Tests
{
    public sealed class LOTNCoreTests
    {
        [Test]
        public void WorldMapDefinesNineDistinctRegions()
        {
            Assert.AreEqual(9, LOTNRegionData.Names.Length);
            Assert.AreEqual(9, LOTNRegionData.SceneNames.Length);
            Assert.AreEqual(9, LOTNRegionData.RenownRequired.Length);
            Assert.AreEqual(9, new System.Collections.Generic.HashSet<string>(LOTNRegionData.SceneNames).Count);
            Assert.AreEqual(0, LOTNRegionData.RenownRequired[0]);
        }

        [Test]
        public void PlayerStatsRejectNegativeRewardsAndClampStamina()
        {
            GameObject go = new GameObject("Stats Test");
            try
            {
                LOTNPlayerStats stats = go.AddComponent<LOTNPlayerStats>();
                stats.gold = 25;
                stats.renown = 5;
                stats.stamina = 95;

                stats.AddGold(-100);
                stats.AddRenown(-100);
                stats.RestoreStamina(1000);

                Assert.AreEqual(25, stats.gold);
                Assert.AreEqual(5, stats.renown);
                Assert.AreEqual(100, stats.stamina);
                Assert.IsFalse(stats.SpendStamina(-1));
            }
            finally { Object.DestroyImmediate(go); }
        }

        [Test]
        public void InventoryStacksItemsAndEquipsApparel()
        {
            PlayerPrefs.DeleteKey("LOTN.Inventory");
            GameObject go = new GameObject("Inventory Test");
            try
            {
                LOTNInventory inventory = go.AddComponent<LOTNInventory>();
                Assert.IsTrue(inventory.AddItem("test_token", "Test Token", 1));
                Assert.IsTrue(inventory.AddItem("test_token", "Test Token", 2));
                LOTNInventoryEntry token = null;
                foreach (LOTNInventoryEntry entry in inventory.Entries)
                    if (entry.id == "test_token") token = entry;
                Assert.IsNotNull(token);
                Assert.AreEqual(3, token.quantity);
                Assert.IsTrue(inventory.RemoveItem("test_token", 1));
                Assert.AreEqual(2, token.quantity);
                Assert.IsTrue(inventory.Equip("northern_cloak"));
                Assert.AreEqual("northern_cloak", inventory.EquippedCloak);
            }
            finally
            {
                Object.DestroyImmediate(go);
                PlayerPrefs.DeleteKey("LOTN.Inventory");
            }
        }

        [Test]
        public void QuestRequiresSupplyCrateBeforeCompletion()
        {
            GameObject sessionObject = new GameObject("Session Test");
            GameObject managerObject = new GameObject("Quest Test");
            try
            {
                LOTNGameSession session = sessionObject.AddComponent<LOTNGameSession>();
                session.SetQuestState(false, false);
                LOTNQuestManager manager = managerObject.AddComponent<LOTNQuestManager>();

                manager.Interact("QuestGiver");
                Assert.IsFalse(manager.questCompleted);

                manager.Interact("SupplyCrate");
                Assert.IsTrue(manager.crateCollected);
                Assert.IsFalse(manager.questCompleted);
            }
            finally
            {
                Object.DestroyImmediate(managerObject);
                Object.DestroyImmediate(sessionObject);
            }
        }
    }
}
