#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LOTNQuestComponent.generated.h"

USTRUCT(BlueprintType)
struct FLOTNQuestEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
    FName QuestId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
    FText Title;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
    FText Description;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
    int32 GoldReward = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
    int32 RenownReward = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLOTNQuestStateChanged);

UCLASS(ClassGroup=(LOTN), meta=(BlueprintSpawnableComponent))
class LORDOFTHENORTH_API ULOTNQuestComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULOTNQuestComponent();

    UPROPERTY(BlueprintAssignable, Category="LOTN|Quest")
    FLOTNQuestStateChanged OnQuestStateChanged;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Quest")
    TArray<FLOTNQuestEntry> ActiveQuests;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Quest")
    TArray<FName> CompletedQuestIds;

    UFUNCTION(BlueprintCallable, Category="LOTN|Quest")
    bool StartQuest(FName QuestId, FText Title, FText Description, int32 GoldReward, int32 RenownReward);

    UFUNCTION(BlueprintCallable, Category="LOTN|Quest")
    bool CompleteQuest(FName QuestId);

    UFUNCTION(BlueprintPure, Category="LOTN|Quest")
    bool IsQuestCompleted(FName QuestId) const;

    UFUNCTION(BlueprintCallable, Category="LOTN|Quest")
    void RestoreQuestState(const TArray<FName>& InCompletedQuestIds, const TArray<FName>& InActiveQuestIds);

    UFUNCTION(BlueprintPure, Category="LOTN|Quest")
    TArray<FName> GetActiveQuestIds() const;
};
