#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LOTNFoliageCluster.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UStaticMesh;

UCLASS()
class LORDOFTHENORTH_API ALOTNFoliageCluster : public AActor
{
    GENERATED_BODY()

public:
    ALOTNFoliageCluster();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Environment")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeInstances;

    UFUNCTION(BlueprintCallable, Category="LOTN|Environment")
    void ConfigureTreeMesh(UStaticMesh* Mesh);

    UFUNCTION(BlueprintCallable, Category="LOTN|Environment")
    void AddTreeInstance(FVector Location, float Yaw, float UniformScale);
};
