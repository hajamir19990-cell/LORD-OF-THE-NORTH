#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LOTNInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLOTNInteractionPromptChanged, const FText&, PromptText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLOTNInteractionMessage, const FText&, MessageText);

UCLASS(ClassGroup=(LOTN), meta=(BlueprintSpawnableComponent))
class LORDOFTHENORTH_API ULOTNInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULOTNInteractionComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|Interaction", meta=(ClampMin="100.0", ClampMax="1000.0"))
    float InteractionDistance = 350.f;

    UPROPERTY(BlueprintAssignable, Category="LOTN|Interaction")
    FLOTNInteractionPromptChanged OnPromptChanged;

    UPROPERTY(BlueprintAssignable, Category="LOTN|Interaction")
    FLOTNInteractionMessage OnInteractionMessage;

    UFUNCTION(BlueprintCallable, Category="LOTN|Interaction")
    bool TryInteract();

    UFUNCTION(BlueprintPure, Category="LOTN|Interaction")
    FText GetCurrentPrompt() const;

protected:
    AActor* FindInteractable() const;
};
