#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/TextRenderComponent.h"
#include "SonantDemoGenerator.generated.h"

USTRUCT()
struct FDemoSurface {
    GENERATED_BODY()
    FString Description;
    FLinearColor Color;
};

UCLASS()
class SONANT_API ASonantDemoGenerator : public AActor {
    GENERATED_BODY()

public:
    ASonantDemoGenerator();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SpawnPlatform();
    void CleanupOldPlatforms();

    // --- ADDED THIS DECLARATION ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* SceneRoot;

    TArray<FDemoSurface> SurfaceTypes;

    UPROPERTY(EditAnywhere, Category = "Config")
    UStaticMesh* PlatformMesh;

    UPROPERTY(EditAnywhere, Category = "Config")
    UMaterialInterface* BaseMaterial;

    FVector NextSpawnLocation;
    int32 StepCount = 0;

    UPROPERTY()
    TArray<AActor*> ActivePlatforms;

    UPROPERTY(EditAnywhere)
    APawn* PlayerRef;
};