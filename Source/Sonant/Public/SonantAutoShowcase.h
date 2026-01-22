#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/PointLightComponent.h" // ADDED
#include "SonantAutoShowcase.generated.h"

struct FShowcaseSurface {
    FString Keyword;
    FLinearColor Color;
};

// Track platforms that were hit so we can animate their glow fading out
struct FHitFlash {
    UMaterialInstanceDynamic* Mat;
    FLinearColor BaseColor;
    float TimeRemaining;
};

UCLASS()
class SONANT_API ASonantAutoShowcase : public AActor {
    GENERATED_BODY()

public:
    ASonantAutoShowcase();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SpawnRow();
    void Cleanup();

    UFUNCTION()
    void OnDebrisHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY()
    UStaticMesh* DefaultCube;

    UPROPERTY()
    UMaterialInterface* DefaultMat;

    FVector NextSpawnLoc;
    TArray<AActor*> ActiveActors;
    class APawn* PlayerRef;
    TArray<FShowcaseSurface> Themes;
    int32 StepCounter = 0;

    // Animation Pool for flashy impacts
    TArray<FHitFlash> ActiveFlashes;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;
};