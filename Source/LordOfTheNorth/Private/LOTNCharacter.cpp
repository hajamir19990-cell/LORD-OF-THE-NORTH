#include "LOTNCharacter.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNInteractionComponent.h"
#include "LOTNInventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

ALOTNCharacter::ALOTNCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PlayerStats = CreateDefaultSubobject<ULOTNPlayerStatsComponent>(TEXT("PlayerStats"));
    QuestComponent = CreateDefaultSubobject<ULOTNQuestComponent>(TEXT("QuestComponent"));
    InteractionComponent = CreateDefaultSubobject<ULOTNInteractionComponent>(TEXT("InteractionComponent"));
    InventoryComponent = CreateDefaultSubobject<ULOTNInventoryComponent>(TEXT("InventoryComponent"));

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 520.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->BrakingDecelerationWalking = 1800.f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 360.f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 8.f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void ALOTNCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    StaminaUpdateAccumulator += DeltaSeconds;
    if (StaminaUpdateAccumulator < StaminaUpdateInterval)
    {
        return;
    }

    const float UpdateDelta = StaminaUpdateAccumulator;
    StaminaUpdateAccumulator = 0.f;

    if (PlayerStats && bSprintRequested)
    {
        const float StaminaCost = SprintStaminaDrainPerSecond * UpdateDelta;
        bIsSprinting = PlayerStats->Stamina >= StaminaCost && PlayerStats->ConsumeStamina(StaminaCost);
        if (!bIsSprinting)
        {
            bSprintRequested = false;
        }
    }
    else
    {
        bIsSprinting = false;
    }

    if (!bIsSprinting && PlayerStats && PlayerStats->Stamina < PlayerStats->MaxStamina)
    {
        PlayerStats->RestoreStamina(StaminaRegenPerSecond * UpdateDelta);
    }

    const float DesiredSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
    if (!FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, DesiredSpeed))
    {
        GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
    }
}

void ALOTNCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &ALOTNCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ALOTNCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &ALOTNCharacter::TurnCamera);
    PlayerInputComponent->BindAxis("LookUp", this, &ALOTNCharacter::LookCamera);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ALOTNCharacter::InteractPressed);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALOTNCharacter::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALOTNCharacter::StopSprint);
}

void ALOTNCharacter::InteractPressed()
{
    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

void ALOTNCharacter::StartSprint()
{
    bSprintRequested = true;
}

void ALOTNCharacter::StopSprint()
{
    bSprintRequested = false;
}

void ALOTNCharacter::MoveForward(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
    }
}

void ALOTNCharacter::MoveRight(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
    }
}

void ALOTNCharacter::TurnCamera(float Value)
{
    AddControllerYawInput(Value);
}

void ALOTNCharacter::LookCamera(float Value)
{
    AddControllerPitchInput(Value);
}
