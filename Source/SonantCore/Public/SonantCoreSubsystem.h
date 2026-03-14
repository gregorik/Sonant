// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SonantCoreConfig.h"
#include "Materials/MaterialInterface.h"
#include "SonantCoreSubsystem.generated.h"

UCLASS()
class SONANTCORE_API USonantCoreSubsystem : public ULocalPlayerSubsystem {
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // API: Fire and Forget Footstep (Convenience)
    UFUNCTION(BlueprintCallable, Category = "SonantCore")
    void PlayFootstep(const FVector& Location, const FHitResult& SurfaceHit);

    // API: Generalized Audio Event
    UFUNCTION(BlueprintCallable, Category = "SonantCore")
    void PlaySoundAtLocation(FGameplayTag EventTag, const FVector& Location, const FHitResult& SurfaceHit, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f);

    // API: Physics Impact (Auto-calculates Volume/Tag)
    UFUNCTION(BlueprintCallable, Category = "SonantCore")
    void PlayImpact(const FVector& Location, float ImpactForce, const FHitResult& SurfaceHit);

    // API: Physics Impact (Generic Intensity 0.0 - 1.0)
    UFUNCTION(BlueprintCallable, Category = "SonantCore")
    void PlayImpactByIntensity(const FVector& Location, float Intensity, const FHitResult& SurfaceHit);

    // Made public for testing
    const FSonantCoreSurfaceDef* ResolveMaterial(const FHitResult& Hit);

private:
    UPROPERTY()
    TObjectPtr<USonantCoreConfig> CachedConfig;

    // MAP: Material Interface -> Cached Result (O(1) Lookup after first hit)
    TMap<TObjectPtr<UMaterialInterface>, const FSonantCoreSurfaceDef*> SmartCache;
};