#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LOTNInteractable.generated.h"

UINTERFACE(BlueprintType)
class LORDOFTHENORTH_API ULOTNInteractable : public UInterface
{
    GENERATED_BODY()
};

class LORDOFTHENORTH_API ILOTNInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="LOTN|Interaction")
    FText GetInteractionText(AActor* InteractingActor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="LOTN|Interaction")
    void Interact(AActor* InteractingActor);
};
