#include "LOTNCharacter.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNInteractionComponent.h"
#include "LOTNInventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Math/RotationMatrix.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

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

    // A visible greybox body keeps the player readable before final character
    // art is imported. Every primitive is non-colliding; the Character capsule
    // remains the only movement collision shape.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleAsset(
        TEXT("/Engine/BasicShapes/Capsule.Capsule"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

    auto MakePlaceholderPart = [this](FName Name, UStaticMesh* Mesh,
                                      const FVector& Location, const FVector& Scale,
                                      const FRotator& Rotation)
    {
        UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
        Part->SetupAttachment(RootComponent);
        Part->SetRelativeLocation(Location);
        Part->SetRelativeScale3D(Scale);
        Part->SetRelativeRotation(Rotation);
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Part->SetCastShadow(true);
        if (Mesh)
        {
            Part->SetStaticMesh(Mesh);
        }
        return Part;
    };

    UStaticMesh* CapsuleMesh = CapsuleAsset.Succeeded() ? CapsuleAsset.Object : nullptr;
    UStaticMesh* SphereMesh = SphereAsset.Succeeded() ? SphereAsset.Object : nullptr;
    UStaticMesh* CylinderMesh = CylinderAsset.Succeeded() ? CylinderAsset.Object : nullptr;

    PlaceholderTorso = MakePlaceholderPart(TEXT("PlaceholderTorso"), CapsuleMesh,
        FVector(0.f, 0.f, -4.f), FVector(0.23f, 0.23f, 0.38f), FRotator::ZeroRotator);
    PlaceholderHead = MakePlaceholderPart(TEXT("PlaceholderHead"), SphereMesh,
        FVector(0.f, 0.f, 52.f), FVector(0.18f, 0.18f, 0.18f), FRotator::ZeroRotator);
    PlaceholderLeftArm = MakePlaceholderPart(TEXT("PlaceholderLeftArm"), CylinderMesh,
        FVector(0.f, -25.f, -5.f), FVector(0.085f, 0.085f, 0.28f), FRotator(0.f, 0.f, 8.f));
    PlaceholderRightArm = MakePlaceholderPart(TEXT("PlaceholderRightArm"), CylinderMesh,
        FVector(0.f, 25.f, -5.f), FVector(0.085f, 0.085f, 0.28f), FRotator(0.f, 0.f, -8.f));
    PlaceholderLeftLeg = MakePlaceholderPart(TEXT("PlaceholderLeftLeg"), CylinderMesh,
        FVector(0.f, -11.f, -62.f), FVector(0.095f, 0.095f, 0.34f), FRotator::ZeroRotator);
    PlaceholderRightLeg = MakePlaceholderPart(TEXT("PlaceholderRightLeg"), CylinderMesh,
        FVector(0.f, 11.f, -62.f), FVector(0.095f, 0.095f, 0.34f), FRotator::ZeroRotator);

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

    const float SafeUpdateInterval = FMath::Max(0.05f, StaminaUpdateInterval);
    StaminaUpdateAccumulator += DeltaSeconds;
    if (StaminaUpdateAccumulator < SafeUpdateInterval)
    {
        return;
    }

    const float UpdateDelta = StaminaUpdateAccumulator;
    StaminaUpdateAccumulator = 0.f;

    if (PlayerStats && bSprintRequested)
    {
        const float StaminaCost = FMath::Max(0.f, SprintStaminaDrainPerSecond) * UpdateDelta;
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
        PlayerStats->RestoreStamina(FMath::Max(0.f, StaminaRegenPerSecond) * UpdateDelta);
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
