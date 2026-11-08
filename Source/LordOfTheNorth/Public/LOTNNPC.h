#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LOTNInteractable.h"
#include "LOTNNPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLOTNDialogueRequested, const FText&, DialogueText);

UCLASS()
class LORDOFTHENORTH_API ALOTNNPC : public ACharacter, public ILOTNInteractable
{
    GENERATED_BODY()

public:
    ALOTNNPC();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|NPC")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|NPC", meta=(MultiLine="true"))
    FText DialogueText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest")
    bool bOffersQuest = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest", meta=(EditCondition="bOffersQuest"))
    FName QuestId = TEXT("Winterfell_Delivery");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest", meta=(EditCondition="bOffersQuest"))
    FText QuestTitle = FText::FromString(TEXT("A Northern Errand"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest", meta=(EditCondition="bOffersQuest", MultiLine="true"))
    FText QuestDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest", meta=(EditCondition="bOffersQuest", ClampMin="0"))
    int32 GoldReward = 25;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Quest", meta=(EditCondition="bOffersQuest", ClampMin="0"))
    int32 RenownReward = 5;

    UPROPERTY(BlueprintAssignable, Category="LOTN|Dialogue")
    FLOTNDialogueRequested OnDialogueRequested;

    virtual FText GetInteractionText_Implementation(AActor* InteractingActor) const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;
};
