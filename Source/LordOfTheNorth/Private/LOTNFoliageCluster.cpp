#include "LOTNFoliageCluster.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"

ALOTNFoliageCluster::ALOTNFoliageCluster()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    TreeInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TreeInstances"));
    TreeInstances->SetupAttachment(SceneRoot);
    TreeInstances->SetMobility(EComponentMobility::Static);
    TreeInstances->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TreeInstances->SetCanEverAffectNavigation(false);
    TreeInstances->SetCastShadow(true);
    TreeInstances->InstanceStartCullDistance = 20000;
    TreeInstances->InstanceEndCullDistance = 280000;
}

void ALOTNFoliageCluster::ConfigureTreeMesh(UStaticMesh* Mesh)
{
    if (TreeInstances && Mesh)
    {
        TreeInstances->SetStaticMesh(Mesh);
    }
}

void ALOTNFoliageCluster::AddTreeInstance(FVector Location, float Yaw, float UniformScale)
{
    if (!TreeInstances || !TreeInstances->GetStaticMesh())
    {
        return;
    }

    const float SafeScale = FMath::Clamp(UniformScale, 0.1f, 4.0f);
    const FTransform InstanceTransform(
        FRotator(0.0f, Yaw, 0.0f),
        Location,
        FVector(SafeScale, SafeScale, SafeScale));
    TreeInstances->AddInstance(InstanceTransform, true);
}
