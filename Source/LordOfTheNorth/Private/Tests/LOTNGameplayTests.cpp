#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "LOTNInventoryComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNWorldMapData.h"
#include "LOTNPlayerStatsComponent.h"

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
    Inventory->AddItem(TEXT("Stone"), FText::FromString(TEXT("Stone")), FText::GetEmpty(),
        ELOTNItemCategory::Material, 1, 10);
    TestFalse(TEXT("Materials cannot be equipped"), Inventory->EquipItem(TEXT("Stone")));
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


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNInventoryRestoreTest,
    "LOTN.Inventory.RestoreValidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNInventoryRestoreTest::RunTest(const FString& Parameters)
{
    ULOTNInventoryComponent* Inventory = NewObject<ULOTNInventoryComponent>();
    if (!Inventory)
    {
        return false;
    }

    FLOTNInventoryItem HoodA;
    HoodA.ItemId = TEXT("HoodA");
    HoodA.Category = ELOTNItemCategory::Apparel;
    HoodA.EquipmentSlot = ELOTNEquipmentSlot::Head;
    HoodA.Quantity = 8;
    HoodA.MaxStack = 1;

    FLOTNInventoryItem HoodB = HoodA;
    HoodB.ItemId = TEXT("HoodB");
    HoodB.Quantity = 1;

    FLOTNInventoryItem InvalidItem;
    InvalidItem.ItemId = NAME_None;
    InvalidItem.Quantity = 3;

    TArray<FLOTNInventoryItem> SavedItems = { HoodA, HoodB, InvalidItem };
    TArray<FName> SavedEquipment = { TEXT("HoodA"), TEXT("HoodB"), TEXT("MissingItem") };
    Inventory->RestoreInventory(SavedItems, SavedEquipment);

    TestEqual(TEXT("Invalid item IDs are discarded"), Inventory->Items.Num(), 2);
    TestEqual(TEXT("Saved stack size is clamped"), Inventory->Items[0].Quantity, 1);
    TestEqual(TEXT("Only one item per equipment slot is restored"), Inventory->EquippedItemIds.Num(), 1);
    TestTrue(TEXT("First valid equipment entry is retained"), Inventory->EquippedItemIds.Contains(TEXT("HoodA")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNQuestRestoreTest,
    "LOTN.Quests.RestoreValidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNQuestRestoreTest::RunTest(const FString& Parameters)
{
    ULOTNQuestComponent* Quests = NewObject<ULOTNQuestComponent>();
    if (!Quests)
    {
        return false;
    }

    FLOTNQuestEntry CompletedQuest;
    CompletedQuest.QuestId = TEXT("AlreadyDone");

    FLOTNQuestEntry ActiveQuest;
    ActiveQuest.QuestId = TEXT("StillActive");
    ActiveQuest.RequiredProgress = 2;
    ActiveQuest.CurrentProgress = 9;
    ActiveQuest.GoldReward = -10;
    ActiveQuest.RenownReward = -2;

    FLOTNQuestEntry DuplicateQuest = ActiveQuest;
    FLOTNQuestEntry EmptyQuest;

    Quests->RestoreQuestState(
        { TEXT("AlreadyDone"), TEXT("AlreadyDone"), NAME_None },
        { CompletedQuest, ActiveQuest, DuplicateQuest, EmptyQuest });

    TestEqual(TEXT("Completed IDs are unique"), Quests->CompletedQuestIds.Num(), 1);
    TestEqual(TEXT("Invalid and duplicate active quests are discarded"), Quests->ActiveQuests.Num(), 1);
    TestEqual(TEXT("Progress is clamped to requirement"), Quests->ActiveQuests[0].CurrentProgress, 2);
    TestEqual(TEXT("Negative rewards are clamped"), Quests->ActiveQuests[0].GoldReward, 0);
    TestEqual(TEXT("Negative renown is clamped"), Quests->ActiveQuests[0].RenownReward, 0);
    return true;
}



IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNInventoryInputBoundsTest,
    "LOTN.Inventory.InputBounds",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNInventoryInputBoundsTest::RunTest(const FString& Parameters)
{
    ULOTNInventoryComponent* Inventory = NewObject<ULOTNInventoryComponent>();
    if (!Inventory)
    {
        return false;
    }

    TestEqual(TEXT("Empty item ID is rejected"), Inventory->AddItem(
        NAME_None, FText::GetEmpty(), FText::GetEmpty(), ELOTNItemCategory::Material, 5), 5);
    TestEqual(TEXT("Zero quantity is a no-op"), Inventory->AddItem(
        TEXT("ZeroItem"), FText::GetEmpty(), FText::GetEmpty(), ELOTNItemCategory::Material, 0), 0);
    TestEqual(TEXT("Oversized request reports unaccepted remainder"), Inventory->AddItem(
        TEXT("BoundedItem"), FText::FromString(TEXT("Bounded Item")), FText::GetEmpty(),
        ELOTNItemCategory::Material, 10001, 250), 1);
    TestEqual(TEXT("Inventory accepts no more than the per-call safety cap"),
        Inventory->GetItemCount(TEXT("BoundedItem")), 10000);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FLOTNPlayerStatsBoundsTest,
    "LOTN.PlayerStats.BoundsAndRecovery",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLOTNPlayerStatsBoundsTest::RunTest(const FString& Parameters)
{
    ULOTNPlayerStatsComponent* Stats = NewObject<ULOTNPlayerStatsComponent>();
    if (!Stats)
    {
        return false;
    }

    Stats->ApplyDamage(-10.f);
    TestEqual(TEXT("Negative damage is ignored"), Stats->Health, 100.f);
    Stats->ApplyDamage(150.f);
    TestEqual(TEXT("Damage cannot reduce health below zero"), Stats->Health, 0.f);
    Stats->RestoreHealth(150.f);
    TestEqual(TEXT("Healing cannot exceed maximum health"), Stats->Health, Stats->MaxHealth);

    TestFalse(TEXT("Insufficient stamina is rejected"), Stats->ConsumeStamina(101.f));
    TestEqual(TEXT("Rejected stamina cost does not mutate stamina"), Stats->Stamina, 100.f);
    TestTrue(TEXT("Valid stamina cost succeeds"), Stats->ConsumeStamina(40.f));
    TestEqual(TEXT("Stamina cost is applied"), Stats->Stamina, 60.f);
    Stats->RestoreStamina(100.f);
    TestEqual(TEXT("Stamina recovery is capped"), Stats->Stamina, Stats->MaxStamina);
    return true;
}


#endif
