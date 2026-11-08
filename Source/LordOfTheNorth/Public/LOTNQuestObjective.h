#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LOTNInteractable.h"
#include "LOTNQuestObjective.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLOTNQuestObjectiveActivated);

UCLASS()
class LORDOFTHENORTH_API ALOTNQuestObjective : public AActor, public ILOTNInteractable
{
    GENERATED_BODY()

public:
    ALOTNQuestObjective();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UBoxComponent> InteractionBounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest")
    FName QuestId = TEXT("Winterfell_Delivery");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest", meta=(ClampMin="1"))
    int32 ProgressAmount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Interaction")
    FText InteractionLabel = FText::FromString(TEXT("Inspect supply marker"));

    UPROPERTY(BlueprintAssignable, Category="LOTN|Quest")
    FLOTNQuestObjectiveActivated OnObjectiveActivated;

    virtual FText GetInteractionText_Implementation(AActor* InteractingActor) const override;
    virtual FText Interact_Implementation(AActor* InteractingActor) override;
};
