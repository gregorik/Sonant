// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundBase.h"                    
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SonantCoreConfig.generated.h"

USTRUCT(BlueprintType)
struct FSonantCoreSound {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "SonantCore|Audio")
    TObjectPtr<USoundBase> Sound;

    UPROPERTY(EditAnywhere, Category = "SonantCore|Audio")
    float Volume = 1.0f;
    
    UPROPERTY(EditAnywhere, Category = "SonantCore|Audio")
    float PitchRandomization = 0.05f;
};

USTRUCT(BlueprintType)
struct FSonantCoreSurfaceDef {
    GENERATED_BODY()

    // Map specific events (Walk, Run, Land, Impact) to sounds
    UPROPERTY(EditAnywhere, Category = "SonantCore")
    TMap<FGameplayTag, FSonantCoreSound> Sounds;
};

UCLASS()
class SONANTCORE_API USonantCoreConfig : public UPrimaryDataAsset {
    GENERATED_BODY()
public:
    // 1. Keyword Fallback: "Grass" matches "M_Ground_Grass01"
    UPROPERTY(EditDefaultsOnly, Category = "Surfaces")
    TMap<FString, FSonantCoreSurfaceDef> KeywordMap;

    // 2. Exact Physics: For specific overrides
    UPROPERTY(EditDefaultsOnly, Category = "Surfaces")
    TMap<TSoftObjectPtr<UPhysicalMaterial>, FSonantCoreSurfaceDef> PhysicsMap;

    UPROPERTY(EditDefaultsOnly, Category = "Surfaces")
    FSonantCoreSurfaceDef DefaultSurface;
};

