#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/StaticMeshActor.h"
#include "SonantGodRayShowcase.generated.h"

struct FGodRayTheme {
    FString Keyword;
    FLinearColor Color;
};

// Tracks the "Equalizer" walls that pulse to the rhythm
struct FPulsingWall {
    AStaticMeshActor* Actor;
    FVector BaseScale;
    float Offset;
};

UCLASS()
class SONANT_API ASonantGodRayShowcase : public AActor {
    GENERATED_BODY()
    
public:    
    ASonantGodRayShowcase();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SpawnHelixSegment();
    void Cleanup();
    
    UFUNCTION()
    void OnImpact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // --- Components for Atmosphere ---
    UPROPERTY(VisibleAnywhere)
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere)
    UPostProcessComponent* PostProcess; // Force Bloom

    UPROPERTY(VisibleAnywhere)
    UExponentialHeightFogComponent* VolumetricFog; // Force God Rays

    // --- Assets ---
    UStaticMesh* MeshAsset;
    UMaterialInterface* MatAsset;

    // --- State ---
    TArray<FGodRayTheme> Themes;
    TArray<AActor*> JunkPile;
    TArray<FPulsingWall> WallPool;
    
    FVector HelixCursor;
    int32 StepIndex = 0;
    APawn* Player;
    
    // Logic for the Helix shape
    float HelixRadius = 1500.0f;
    float HelixTightness = 0.1f;
};