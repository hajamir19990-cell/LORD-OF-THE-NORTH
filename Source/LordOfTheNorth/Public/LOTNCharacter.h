#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LOTNCharacter.generated.h"

class ULOTNPlayerStatsComponent;
class ULOTNQuestComponent;
class ULOTNInteractionComponent;
class ULOTNInventoryComponent;
class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;

UCLASS()
class LORDOFTHENORTH_API ALOTNCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ALOTNCharacter();

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="LOTN|Interaction")
    void InteractPressed();

    UFUNCTION(BlueprintCallable, Category="LOTN|Movement")
    void StartSprint();

    UFUNCTION(BlueprintCallable, Category="LOTN|Movement")
    void StopSprint();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOTN|World")
    FName CurrentLocationId = TEXT("Winterfell");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Stats")
    TObjectPtr<ULOTNPlayerStatsComponent> PlayerStats;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Quest")
    TObjectPtr<ULOTNQuestComponent> QuestComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Interaction")
    TObjectPtr<ULOTNInteractionComponent> InteractionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Inventory")
    TObjectPtr<ULOTNInventoryComponent> InventoryComponent;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void TurnCamera(float Value);
    void LookCamera(float Value);

    // Temporary low-poly humanoid assembled from engine primitives. Replace
    // with a licensed skeletal character and animation blueprint during art pass.
    UPROPERTY(VisibleAnywhere, Category="LOTN|Placeholder")
    TObjectPtr<UStaticMeshComponent> PlaceholderTorso;

    UPROPERTY(VisibleAnywhere, Category="LOTN|Placeholder")
    TObjectPtr<UStaticMeshComponent> PlaceholderHead;

    UPROPERTY(VisibleAnywhere, Category="LOTN|Placeholder")
    TObjectPtr<UStaticMeshComponent> PlaceholderLeftArm;

    UPROPERTY(VisibleAnywhere, Category="LOTN|Placeholder")
    TObjectPtr<UStaticMeshComponent> PlaceholderRightArm;

    UPROPERTY(VisibleAnywhere, Category="LOTN|Placeholder")
    TObjectPtr<UStaticMeshComponent> PlaceholderLeftLeg;

    UPROPERTY(VisibleAnywhere, Category="LOTN|Placeholder")
    TObjectPtr<UStaticMeshComponent> PlaceholderRightLeg;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditDefaultsOnly, Category="LOTN|Movement")
    float WalkSpeed = 360.f;

    UPROPERTY(EditDefaultsOnly, Category="LOTN|Movement")
    float SprintSpeed = 520.f;

    UPROPERTY(EditDefaultsOnly, Category="LOTN|Movement")
    float SprintStaminaDrainPerSecond = 22.f;

    UPROPERTY(EditDefaultsOnly, Category="LOTN|Movement")
    float StaminaRegenPerSecond = 14.f;

    UPROPERTY(EditDefaultsOnly, Category="LOTN|Movement")
    float StaminaUpdateInterval = 0.2f;

    bool bSprintRequested = false;
    bool bIsSprinting = false;
    float StaminaUpdateAccumulator = 0.f;
};
