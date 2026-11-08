#include "LOTNQuestComponent.h"
#include "LOTNPlayerStatsComponent.h"

ULOTNQuestComponent::ULOTNQuestComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool ULOTNQuestComponent::StartQuest(FName QuestId, FText Title, FText Description, int32 GoldReward, int32 RenownReward)
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
    NewEntry.GoldReward = FMath::Max(0, GoldReward);
    NewEntry.RenownReward = FMath::Max(0, RenownReward);
    ActiveQuests.Add(NewEntry);
    OnQuestStateChanged.Broadcast();
    return true;
}

bool ULOTNQuestComponent::CompleteQuest(FName QuestId)
{
    const int32 Index = ActiveQuests.IndexOfByPredicate(
        [QuestId](const FLOTNQuestEntry& Entry) { return Entry.QuestId == QuestId; });

    if (Index == INDEX_NONE)
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

void ULOTNQuestComponent::RestoreQuestState(const TArray<FName>& InCompletedQuestIds, const TArray<FName>& InActiveQuestIds)
{
    CompletedQuestIds = InCompletedQuestIds;
    ActiveQuests.RemoveAll([this](const FLOTNQuestEntry& Entry)
    {
        return !InActiveQuestIds.Contains(Entry.QuestId);
    });
    OnQuestStateChanged.Broadcast();
}

TArray<FName> ULOTNQuestComponent::GetActiveQuestIds() const
{
    TArray<FName> Result;
    Result.Reserve(ActiveQuests.Num());
    for (const FLOTNQuestEntry& Entry : ActiveQuests)
    {
        Result.Add(Entry.QuestId);
    }
    return Result;
}
