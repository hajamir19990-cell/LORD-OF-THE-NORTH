#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LOTNCharacter.generated.h"

class ULOTNPlayerStatsComponent;
class ULOTNQuestComponent;
class ULOTNInteractionComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LORDOFTHENORTH_API ALOTNCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ALOTNCharacter();

    UFUNCTION(BlueprintCallable, Category="LOTN|Interaction")
    void InteractPressed();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|World")
    FName CurrentLocationId = TEXT("Winterfell");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Stats")
    TObjectPtr<ULOTNPlayerStatsComponent> PlayerStats;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Quest")
    TObjectPtr<ULOTNQuestComponent> QuestComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Interaction")
    TObjectPtr<ULOTNInteractionComponent> InteractionComponent;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void TurnCamera(float Value);
    void LookCamera(float Value);

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<UCameraComponent> FollowCamera;
};
