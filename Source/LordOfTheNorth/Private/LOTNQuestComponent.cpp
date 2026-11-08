#include "LOTNQuestComponent.h"
#include "LOTNPlayerStatsComponent.h"

ULOTNQuestComponent::ULOTNQuestComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool ULOTNQuestComponent::StartQuest(FName QuestId, FText Title, FText Description, int32 GoldReward, int32 RenownReward, FText ObjectiveText, int32 RequiredProgress)
{
    if (QuestId.IsNone() || IsQuestCompleted(QuestId))
    {
        return false;
    }

    for (const FLOTNQuestEntry& Entry : ActiveQuests)
    {
        if (Entry.QuestId == QuestId)
        {
            return false;
        }
    }

    FLOTNQuestEntry NewEntry;
    NewEntry.QuestId = QuestId;
    NewEntry.Title = Title;
    NewEntry.Description = Description;
    NewEntry.ObjectiveText = ObjectiveText;
    NewEntry.RequiredProgress = FMath::Max(1, RequiredProgress);
    NewEntry.GoldReward = FMath::Max(0, GoldReward);
    NewEntry.RenownReward = FMath::Max(0, RenownReward);
    ActiveQuests.Add(NewEntry);
    OnQuestStateChanged.Broadcast();
    return true;
}

bool ULOTNQuestComponent::AdvanceQuestProgress(FName QuestId, int32 Amount)
{
    if (Amount <= 0)
    {
        return false;
    }

    FLOTNQuestEntry* Entry = ActiveQuests.FindByPredicate(
        [QuestId](const FLOTNQuestEntry& Item) { return Item.QuestId == QuestId; });

    if (!Entry || Entry->CurrentProgress >= Entry->RequiredProgress)
    {
        return false;
    }

    const int64 NewProgress = static_cast<int64>(Entry->CurrentProgress) + Amount;
    Entry->CurrentProgress = static_cast<int32>(FMath::Min<int64>(Entry->RequiredProgress, NewProgress));
    OnQuestStateChanged.Broadcast();
    return true;
}

bool ULOTNQuestComponent::CompleteQuest(FName QuestId)
{
    const int32 Index = ActiveQuests.IndexOfByPredicate(
        [QuestId](const FLOTNQuestEntry& Entry) { return Entry.QuestId == QuestId; });

    if (Index == INDEX_NONE || !IsQuestReadyToTurnIn(QuestId))
    {
        return false;
    }

    const FLOTNQuestEntry CompletedEntry = ActiveQuests[Index];
    ActiveQuests.RemoveAt(Index);
    CompletedQuestIds.AddUnique(QuestId);

    if (AActor* OwnerActor = GetOwner())
    {
        if (ULOTNPlayerStatsComponent* Stats = OwnerActor->FindComponentByClass<ULOTNPlayerStatsComponent>())
        {
            Stats->AddGold(CompletedEntry.GoldReward);
            Stats->AddRenown(CompletedEntry.RenownReward);
        }
    }

    OnQuestStateChanged.Broadcast();
    return true;
}

bool ULOTNQuestComponent::IsQuestCompleted(FName QuestId) const
{
    return CompletedQuestIds.Contains(QuestId);
}

bool ULOTNQuestComponent::IsQuestReadyToTurnIn(FName QuestId) const
{
    const FLOTNQuestEntry* Entry = ActiveQuests.FindByPredicate(
        [QuestId](const FLOTNQuestEntry& Item) { return Item.QuestId == QuestId; });
    return Entry && Entry->CurrentProgress >= Entry->RequiredProgress;
}

void ULOTNQuestComponent::RestoreQuestState(const TArray<FName>& InCompletedQuestIds, const TArray<FLOTNQuestEntry>& InActiveQuests)
{
    CompletedQuestIds = InCompletedQuestIds;
    ActiveQuests = InActiveQuests;
    OnQuestStateChanged.Broadcast();
}
