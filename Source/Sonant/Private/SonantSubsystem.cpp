// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#include "SonantSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AudioModulationStatics.h"
#include "Components/PrimitiveComponent.h"
#include "SonantSettings.h" 

void USonantSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);
    SmartCache.Reserve(128);
}

void USonantSubsystem::PlayFootstep(const FVector& Location, const FHitResult& SurfaceHit) {
    if (const FSonantSurfaceDef* Def = ResolveMaterial(SurfaceHit)) {
        if (!Def->Sound.IsNull()) {
            float P = 1.0f + FMath::RandRange(-Def->PitchRandomization, Def->PitchRandomization);
            UGameplayStatics::PlaySoundAtLocation(this, Def->Sound.LoadSynchronous(), Location, Def->Volume, P);
        }
    }
}

const FSonantSurfaceDef* USonantSubsystem::ResolveMaterial(const FHitResult& Hit) {
    const USonantConfig* Config = USonantSettings::Get();
    if (!Config) return nullptr;

    // 1. FAST PATH: Physical Material
    if (UPhysicalMaterial* Phys = Hit.PhysMaterial.Get()) {
        if (const FSonantSurfaceDef* Found = Config->PhysicsMap.Find(Phys)) return Found;
    }

    // 2. CHECK CACHE (Material Interface)
    UMaterialInterface* Mat = Hit.GetComponent() ? Hit.GetComponent()->GetMaterial(0) : nullptr;
    if (!Mat) return &Config->DefaultSurface;

    if (const FSonantSurfaceDef** Cached = SmartCache.Find(Mat)) return *Cached;

    // 3. SLOW PATH: Substring Search
    FString MatName = Mat->GetName();
    for (const auto& Kvp : Config->KeywordMap) {
        if (MatName.Contains(Kvp.Key)) {
            SmartCache.Add(Mat, &Kvp.Value);
            return &Kvp.Value;
        }
    }

    SmartCache.Add(Mat, &Config->DefaultSurface);
    return &Config->DefaultSurface;
}

// --- THIS IS THE SECTION YOU WERE LIKELY MISSING ---

void USonantSubsystem::PushAtmosphere(FGameplayTag Tag) {
    ActiveAtmosphereCounts.FindOrAdd(Tag)++;
    UpdateActiveMixer();
}

void USonantSubsystem::PopAtmosphere(FGameplayTag Tag) {
    if (ActiveAtmosphereCounts.Contains(Tag)) {
        ActiveAtmosphereCounts[Tag]--;
        if (ActiveAtmosphereCounts[Tag] <= 0) {
            ActiveAtmosphereCounts.Remove(Tag);
        }
    }
    UpdateActiveMixer();
}

void USonantSubsystem::UpdateActiveMixer() {
    const USonantConfig* Config = USonantSettings::Get();
    if (!Config) return;

    int32 BestPriority = -1;
    FGameplayTag BestTag;

    for (const auto& Kvp : ActiveAtmosphereCounts) {
        if (Kvp.Value > 0) {
            if (const FSonantMixDef* Def = Config->Atmospheres.Find(Kvp.Key)) {
                if (Def->Priority > BestPriority) {
                    BestPriority = Def->Priority;
                    BestTag = Kvp.Key;
                }
            }
        }
    }

    if (BestTag != CurrentAppliedMixTag) {
        if (CurrentAppliedMixTag.IsValid()) {
            if (const FSonantMixDef* OldDef = Config->Atmospheres.Find(CurrentAppliedMixTag)) {
                if (!OldDef->MixAsset.IsNull()) {
                    UAudioModulationStatics::DeactivateBusMix(this, OldDef->MixAsset.LoadSynchronous());
                }
            }
        }

        if (BestTag.IsValid()) {
            if (const FSonantMixDef* NewDef = Config->Atmospheres.Find(BestTag)) {
                if (!NewDef->MixAsset.IsNull()) {
                    UAudioModulationStatics::ActivateBusMix(this, NewDef->MixAsset.LoadSynchronous());
                }
            }
        }

        CurrentAppliedMixTag = BestTag;
    }
}