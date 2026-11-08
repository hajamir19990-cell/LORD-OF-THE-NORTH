#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "LOTNInventoryComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNWorldMapData.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNInventoryStackingTest,
    "LOTN.Inventory.StackingAndRemoval",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNInventoryStackingTest::RunTest(const FString& Parameters)
{
    ULOTNInventoryComponent* Inventory = NewObject<ULOTNInventoryComponent>();
    TestNotNull(TEXT("Inventory component created"), Inventory);
    if (!Inventory)
    {
        return false;
    }

    const int32 Remainder = Inventory->AddItem(
        TEXT("NorthToken"),
        FText::FromString(TEXT("Northern Token")),
        FText::FromString(TEXT("A small keepsake.")),
        ELOTNItemCategory::Material,
        150,
        99);

    TestEqual(TEXT("All valid quantity is accepted"), Remainder, 0);
    TestEqual(TEXT("Stacked count is preserved"), Inventory->GetItemCount(TEXT("NorthToken")), 150);
    TestEqual(TEXT("Two stacks are created"), Inventory->Items.Num(), 2);
    TestTrue(TEXT("Partial removal succeeds"), Inventory->RemoveItem(TEXT("NorthToken"), 40));
    TestEqual(TEXT("Partial removal updates count"), Inventory->GetItemCount(TEXT("NorthToken")), 110);
    TestFalse(TEXT("Insufficient removal is rejected"), Inventory->RemoveItem(TEXT("NorthToken"), 200));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNEquipmentSlotTest,
    "LOTN.Inventory.EquipmentSlots",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNEquipmentSlotTest::RunTest(const FString& Parameters)
{
    ULOTNInventoryComponent* Inventory = NewObject<ULOTNInventoryComponent>();
    if (!Inventory)
    {
        return false;
    }

    Inventory->AddItem(TEXT("WoolHood"), FText::FromString(TEXT("Wool Hood")), FText::GetEmpty(),
        ELOTNItemCategory::Apparel, 1, 1, ELOTNEquipmentSlot::Head);
    Inventory->AddItem(TEXT("FurHood"), FText::FromString(TEXT("Fur Hood")), FText::GetEmpty(),
        ELOTNItemCategory::Apparel, 1, 1, ELOTNEquipmentSlot::Head);

    TestTrue(TEXT("First apparel item equips"), Inventory->EquipItem(TEXT("WoolHood")));
    TestTrue(TEXT("Second item replaces the same equipment slot"), Inventory->EquipItem(TEXT("FurHood")));
    TestEqual(TEXT("Only one item remains equipped in the slot"), Inventory->EquippedItemIds.Num(), 1);
    TestTrue(TEXT("New slot item is equipped"), Inventory->EquippedItemIds.Contains(TEXT("FurHood")));
    TestFalse(TEXT("Materials cannot be equipped"), Inventory->EquipItem(TEXT("UnlistedMaterial")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNQuestProgressTest,
    "LOTN.Quests.ProgressAndTurnIn",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNQuestProgressTest::RunTest(const FString& Parameters)
{
    ULOTNQuestComponent* Quests = NewObject<ULOTNQuestComponent>();
    if (!Quests)
    {
        return false;
    }

    const FName QuestId(TEXT("Winterfell_Delivery"));
    TestTrue(TEXT("Quest starts"), Quests->StartQuest(
        QuestId,
        FText::FromString(TEXT("A Northern Errand")),
        FText::FromString(TEXT("Check the supply marker.")),
        25,
        5,
        FText::FromString(TEXT("Inspect the marker.")),
        1));
    TestFalse(TEXT("Duplicate quest start is rejected"), Quests->StartQuest(
        QuestId, FText::GetEmpty(), FText::GetEmpty(), 0, 0, FText::GetEmpty(), 1));
    TestTrue(TEXT("Objective advances"), Quests->AdvanceQuestProgress(QuestId));
    TestTrue(TEXT("Quest becomes ready to turn in"), Quests->IsQuestReadyToTurnIn(QuestId));
    TestTrue(TEXT("Quest turns in"), Quests->CompleteQuest(QuestId));
    TestTrue(TEXT("Quest is recorded as completed"), Quests->IsQuestCompleted(QuestId));
    TestFalse(TEXT("Completed quest cannot be turned in twice"), Quests->CompleteQuest(QuestId));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNWorldMapDataTest,
    "LOTN.WorldMap.UnlockRules",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNWorldMapDataTest::RunTest(const FString& Parameters)
{
    ULOTNWorldMapData* WorldMap = NewObject<ULOTNWorldMapData>();
    if (!WorldMap)
    {
        return false;
    }

    TestEqual(TEXT("Nine northern locations are seeded"), WorldMap->Locations.Num(), 9);
    TestTrue(TEXT("Winterfell is unlocked at the start"), WorldMap->IsLocationUnlocked(TEXT("Winterfell"), 0));
    TestFalse(TEXT("Distant region remains locked below its renown gate"), WorldMap->IsLocationUnlocked(TEXT("TheDreadfort"), 0));
    TestTrue(TEXT("Renown unlocks the Dreadfort"), WorldMap->IsLocationUnlocked(TEXT("TheDreadfort"), 30));
    TestFalse(TEXT("Location is not travel-ready until a real level is assigned"), WorldMap->IsLocationReadyToTravel(TEXT("Winterfell"), 100));
    return true;
}

#endif
