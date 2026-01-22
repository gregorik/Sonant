#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "SonantSpectacle.generated.h"

// Defines a visual/audio theme (e.g., "Neon Glass", "Cyber Metal")
USTRUCT(BlueprintType)
struct FSpectacleTheme {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FString Keyword;      // e.g., "Glass" - Triggers Sonant regex

    UPROPERTY(EditAnywhere)
    FLinearColor Color;   // Visual Feedback

    UPROPERTY(EditAnywhere)
    float Roughness = 0.0f; 
};

// A lightweight wrapper to track animating platforms
USTRUCT()
struct FActivePlatform {
    GENERATED_BODY()
    
    UPROPERTY()
    AStaticMeshActor* Actor = nullptr;

    UPROPERTY()
    UTextRenderComponent* DebugText = nullptr;

    FVector TargetLocation;
    float SpawnTime = 0.0f;
};

UCLASS()
class SONANT_API ASonantSpectacle : public AActor {
    GENERATED_BODY()
    
public:    
    ASonantSpectacle();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SpawnRow();
    void AnimatePlatforms(float DeltaTime);
    void CleanupVoid();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* SceneRoot;

    // --- CONFIGURATION ---
    UPROPERTY(EditAnywhere, Category="Spectacle|Config")
    TArray<FSpectacleTheme> Themes;

    UPROPERTY(EditAnywhere, Category="Spectacle|Config")
    UStaticMesh* TileMesh;

    // Needs a vector param "EmissiveColor" and scalar "Roughness"
    UPROPERTY(EditAnywhere, Category="Spectacle|Config")
    UMaterialInterface* MasterMaterial; 

    UPROPERTY(EditAnywhere, Category="Spectacle|Config")
    float LaneWidth = 400.0f;

    UPROPERTY(EditAnywhere, Category="Spectacle|Config")
    float ForwardStep = 400.0f;

    UPROPERTY(EditAnywhere, Category="Spectacle|Config")
    float SpawnDistance = 6000.0f;

    // --- STATE ---
    TArray<FActivePlatform> PlatformPool;
    FVector NextRowLocation;
    int32 TotalRowsSpawned = 0;
    AActor* PlayerRef;
};