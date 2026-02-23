// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "SonantConfig.h"
#include "Materials/MaterialInterface.h"
#include "Tickable.h"
#include "SonantSubsystem.generated.h"

UCLASS()
class SONANT_API USonantSubsystem : public UGameInstanceSubsystem, public FTickableGameObject {
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // FTickableGameObject
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { return TStatId(); }
    virtual bool IsTickable() const override { return !IsTemplate(); }

    // API: Fire and Forget Footstep (Convenience)
    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PlayFootstep(const FVector& Location, const FHitResult& SurfaceHit);

    // API: Generalized Audio Event
    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PlaySoundAtLocation(FGameplayTag EventTag, const FVector& Location, const FHitResult& SurfaceHit, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f);

    // API: Physics Impact (Auto-calculates Volume/Tag)
    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PlayImpact(const FVector& Location, float ImpactForce, const FHitResult& SurfaceHit);

    // API: Atmosphere Stack (Recursive)
    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PushAtmosphere(FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "Sonant")
    void PopAtmosphere(FGameplayTag Tag);

    void UpdateReverb();

    // Made public for testing
    const FSonantSurfaceDef* ResolveMaterial(const FHitResult& Hit);
    void UpdateActiveMixer();

private:
    float ReverbTimerAccumulator = 0.0f;
    TSoftObjectPtr<USoundControlBusMix> CurrentReverbMix;

    // MAP: Material Interface -> Cached Result (O(1) Lookup after first hit)
    TMap<TObjectPtr<UMaterialInterface>, const FSonantSurfaceDef*> SmartCache;

    // DATA: Track nested overlaps (e.g., Player inside Cave inside Forest)
    TMap<FGameplayTag, int32> ActiveAtmosphereCounts;
    FGameplayTag CurrentAppliedMixTag;
};