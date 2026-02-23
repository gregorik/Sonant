// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#include "SonantSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AudioModulationStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SonantSettings.h" 
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"

void USonantSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);
    SmartCache.Reserve(128);
    ReverbTimerAccumulator = 0.0f;
}

void USonantSubsystem::Tick(float DeltaTime) {
    ReverbTimerAccumulator += DeltaTime;
    if (ReverbTimerAccumulator > 0.5f) { // 2Hz
        ReverbTimerAccumulator = 0.0f;
        UpdateReverb();
    }
}

void USonantSubsystem::UpdateReverb() {
    const USonantConfig* Config = USonantSettings::Get();
    if (!Config || Config->ReverbSettings.Num() == 0) return;

    UWorld* World = GetWorld();
    if (!World) return;

    APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!Player) return;

    FVector Origin = Player->GetActorLocation();
    
    // 6-Direction Trace
    FVector Directions[] = { FVector::UpVector, FVector::DownVector, FVector::LeftVector, FVector::RightVector, FVector::ForwardVector, FVector::BackwardVector };
    float TotalDist = 0.0f;
    int32 Hits = 0;
    const float MaxTrace = 5000.0f; 

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Player);

    for (const FVector& Dir : Directions) {
        FHitResult Hit;
        if (World->LineTraceSingleByChannel(Hit, Origin, Origin + (Dir * MaxTrace), ECC_Visibility, Params)) {
            TotalDist += Hit.Distance;
            Hits++;
        } else {
            TotalDist += MaxTrace; // Open space
        }
    }

    // Estimate Radius
    float AvgRadius = TotalDist / 6.0f;
    if (Hits == 0) AvgRadius = MaxTrace; // Outdoors

    // Find best matching preset (Smallest MaxRoomRadius that is >= AvgRadius)
    const FSonantReverbDef* BestMatch = nullptr;
    float BestDiff = FLT_MAX;
    
    for (const FSonantReverbDef& Def : Config->ReverbSettings) {
        if (AvgRadius <= Def.MaxRoomRadius) {
            float Diff = Def.MaxRoomRadius - AvgRadius;
            if (Diff < BestDiff) {
                BestDiff = Diff;
                BestMatch = &Def;
            }
        }
    }
    
    TSoftObjectPtr<USoundControlBusMix> NewMix = BestMatch ? BestMatch->ReverbMix : nullptr;
    
    // If we changed mixes, deactivate old and activate new
    if (NewMix != CurrentReverbMix) {
        if (!CurrentReverbMix.IsNull()) {
             UAudioModulationStatics::DeactivateBusMix(this, CurrentReverbMix.LoadSynchronous());
        }
        if (!NewMix.IsNull()) {
             UAudioModulationStatics::ActivateBusMix(this, NewMix.LoadSynchronous());
        }
        CurrentReverbMix = NewMix;
    }
}

void USonantSubsystem::PlayFootstep(const FVector& Location, const FHitResult& SurfaceHit) {
    // Default to 'Sonant.Event.Footstep'
    PlaySoundAtLocation(FGameplayTag::RequestGameplayTag(FName("Sonant.Event.Footstep")), Location, SurfaceHit);
}

void USonantSubsystem::PlaySoundAtLocation(FGameplayTag EventTag, const FVector& Location, const FHitResult& SurfaceHit, float VolumeMultiplier, float PitchMultiplier) {
    if (const FSonantSurfaceDef* Def = ResolveMaterial(SurfaceHit)) {
        if (const FSonantSound* SoundDef = Def->Sounds.Find(EventTag)) {
            if (!SoundDef->Sound.IsNull()) {
                float P = PitchMultiplier * (1.0f + FMath::RandRange(-SoundDef->PitchRandomization, SoundDef->PitchRandomization));
                float V = VolumeMultiplier * SoundDef->Volume;
                UGameplayStatics::PlaySoundAtLocation(this, SoundDef->Sound.LoadSynchronous(), Location, V, P);
            }
        }
    }
}

void USonantSubsystem::PlayImpact(const FVector& Location, float ImpactForce, const FHitResult& SurfaceHit) {
    if (ImpactForce < 50.0f) return; // Too quiet

    // Determine Intensity
    FGameplayTag TagToPlay;
    float Vol = 1.0f;
    float Pitch = 1.0f;

    if (ImpactForce > 2000.0f) {
        TagToPlay = FGameplayTag::RequestGameplayTag(FName("Sonant.Event.Impact.Heavy"));
        Vol = FMath::Clamp(ImpactForce / 5000.0f, 0.8f, 2.0f); // Scale volume
        Pitch = 0.8f; // Heavy = lower pitch
    } else {
        TagToPlay = FGameplayTag::RequestGameplayTag(FName("Sonant.Event.Impact.Light"));
        Vol = FMath::Clamp(ImpactForce / 2000.0f, 0.2f, 1.0f);
        Pitch = 1.2f; // Light = higher pitch
    }

    PlaySoundAtLocation(TagToPlay, Location, SurfaceHit, Vol, Pitch);
}

const FSonantSurfaceDef* USonantSubsystem::ResolveMaterial(const FHitResult& Hit) {
    const USonantConfig* Config = USonantSettings::Get();
    if (!Config) return nullptr;

    // 1. FAST PATH: Physical Material
    if (UPhysicalMaterial* Phys = Hit.PhysMaterial.Get()) {
        if (const FSonantSurfaceDef* Found = Config->PhysicsMap.Find(Phys)) return Found;
    }

    // 2. CHECK CACHE (Material Interface)
    UMaterialInterface* Mat = nullptr;

    if (UPrimitiveComponent* Comp = Hit.GetComponent()) {
        // Try to get exact material from collision face (Static Mesh only)
        if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Comp)) {
            if (Hit.FaceIndex != -1) {
                int32 SectionIndex;
                Mat = SMC->GetMaterialFromCollisionFaceIndex(Hit.FaceIndex, SectionIndex);
            }
        }
        
        // Fallback to first material if failed or not a static mesh
        if (!Mat) {
            Mat = Comp->GetMaterial(0);
        }
    }

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