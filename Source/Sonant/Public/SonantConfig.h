// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#pragma once
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SoundControlBusMix.h"
#include "Sound/SoundBase.h"                    
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SonantConfig.generated.h"

USTRUCT(BlueprintType)
struct FSonantSurfaceDef {
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, Category = "Sonant")
    TSoftObjectPtr<USoundBase> Sound;

    UPROPERTY(EditAnywhere, Category = "Sonant")
    float Volume = 1.0f;
    
    UPROPERTY(EditAnywhere, Category = "Sonant")
    float PitchRandomization = 0.05f;
};

// Auto-resolves which Control Bus Mix governs the player's ears
USTRUCT(BlueprintType)
struct FSonantMixDef {
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, Category = "Sonant")
    TSoftObjectPtr<USoundControlBusMix> MixAsset;

    // Higher number wins (e.g., Underwater=100, Indoor=10, Outdoor=0)
    UPROPERTY(EditAnywhere, Category = "Sonant")
    int32 Priority = 0;
};

UCLASS()
class SONANT_API USonantConfig : public UPrimaryDataAsset {
    GENERATED_BODY()
public:
    // 1. Keyword Fallback: "Grass" matches "M_Ground_Grass01"
    UPROPERTY(EditDefaultsOnly, Category = "Surfaces")
    TMap<FString, FSonantSurfaceDef> KeywordMap;

    // 2. Exact Physics: For specific overrides
    UPROPERTY(EditDefaultsOnly, Category = "Surfaces")
    TMap<TSoftObjectPtr<UPhysicalMaterial>, FSonantSurfaceDef> PhysicsMap;

    UPROPERTY(EditDefaultsOnly, Category = "Surfaces")
    FSonantSurfaceDef DefaultSurface;

    // 3. Atmosphere Stacking
    UPROPERTY(EditDefaultsOnly, Category = "Atmospheres")
    TMap<FGameplayTag, FSonantMixDef> Atmospheres;
};