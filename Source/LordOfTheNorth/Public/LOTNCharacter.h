#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LOTNCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LORDOFTHENORTH_API ALOTNCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ALOTNCharacter();

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
