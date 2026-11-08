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
