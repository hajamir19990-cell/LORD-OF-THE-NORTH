#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LOTNPlayerController.generated.h"

class ULOTNHUDWidget;

UCLASS()
class LORDOFTHENORTH_API ALOTNPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALOTNPlayerController();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LOTN|HUD")
    TSubclassOf<ULOTNHUDWidget> HUDWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<ULOTNHUDWidget> HUDWidget;
};
