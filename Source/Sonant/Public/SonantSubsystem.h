// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "SonantConfig.h"
#include "Materials/MaterialInterface.h"
#include "SonantSubsystem.generated.h"

UCLASS()
class SONANT_API USonantSubsystem : public UGameInstanceSubsystem {
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // API: Fire and Forget
    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PlayFootstep(const FVector& Location, const FHitResult& SurfaceHit);

    // API: Atmosphere Stack (Recursive)
    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PushAtmosphere(FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PopAtmosphere(FGameplayTag Tag);

private:
    const FSonantSurfaceDef* ResolveMaterial(const FHitResult& Hit);
    void UpdateActiveMixer();

    // MAP: Material Interface -> Cached Result (O(1) Lookup after first hit)
    TMap<TObjectPtr<UMaterialInterface>, const FSonantSurfaceDef*> SmartCache;

    // DATA: Track nested overlaps (e.g., Player inside Cave inside Forest)
    TMap<FGameplayTag, int32> ActiveAtmosphereCounts;
    FGameplayTag CurrentAppliedMixTag;
};