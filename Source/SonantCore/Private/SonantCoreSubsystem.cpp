// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "SonantCoreSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SonantCoreSettings.h" 
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
// If we dropped Niagara from deps, we should comment out or handle conditionally. But for now we might leave basic generic spawned VFX if the user keeps the module. 
// User said: "no longer require Niagara if it's only for Pro VFX, etc. Keep basic FSonantSound data simple". I will comment out Niagara spawn.
#include "Kismet/KismetMathLibrary.h"

void USonantCoreSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);
    SmartCache.Reserve(128);
    if (const USonantCoreSettings* Settings = GetDefault<USonantCoreSettings>()) {
        CachedConfig = Settings->MainConfig.LoadSynchronous();
    }
}

void USonantCoreSubsystem::PlayFootstep(const FVector& Location, const FHitResult& SurfaceHit) {
    const USonantCoreSettings* Settings = GetDefault<USonantCoreSettings>();
    FGameplayTag Tag = Settings ? Settings->DefaultFootstepTag : FGameplayTag::RequestGameplayTag(FName("SonantCore.Event.Footstep"));
    PlaySoundAtLocation(Tag, Location, SurfaceHit);
}

void USonantCoreSubsystem::PlaySoundAtLocation(FGameplayTag EventTag, const FVector& Location, const FHitResult& SurfaceHit, float VolumeMultiplier, float PitchMultiplier) {
    if (const FSonantCoreSurfaceDef* Def = ResolveMaterial(SurfaceHit)) {
        if (const FSonantCoreSound* SoundDef = Def->Sounds.Find(EventTag)) {
            if (SoundDef->Sound) {
                float P = PitchMultiplier * (1.0f + FMath::RandRange(-SoundDef->PitchRandomization, SoundDef->PitchRandomization));
                float V = VolumeMultiplier * SoundDef->Volume;
                UGameplayStatics::PlaySoundAtLocation(this, SoundDef->Sound, Location, V, P);
            }

            // VFX and Decals kept basic
        }
    }
}

void USonantCoreSubsystem::PlayImpact(const FVector& Location, float ImpactForce, const FHitResult& SurfaceHit) {
    if (ImpactForce < 50.0f) return; 
    
    // Scale intensity 0.0 to 1.0 based on generic reasonable force ranges
    float Intensity = FMath::Clamp(ImpactForce / 5000.0f, 0.0f, 1.0f);
    PlayImpactByIntensity(Location, Intensity, SurfaceHit);
}

void USonantCoreSubsystem::PlayImpactByIntensity(const FVector& Location, float Intensity, const FHitResult& SurfaceHit) {
    if (Intensity <= 0.01f) return;

    const USonantCoreSettings* Settings = GetDefault<USonantCoreSettings>();
    FGameplayTag ImpactHeavyTag = Settings ? Settings->DefaultHeavyImpactTag : FGameplayTag::RequestGameplayTag(FName("SonantCore.Event.Impact.Heavy"));
    FGameplayTag ImpactLightTag = Settings ? Settings->DefaultLightImpactTag : FGameplayTag::RequestGameplayTag(FName("SonantCore.Event.Impact.Light"));

    FGameplayTag TagToPlay;
    float Vol = 1.0f;
    float Pitch = 1.0f;

    if (Intensity > 0.4f) {
        TagToPlay = ImpactHeavyTag;
        Vol = FMath::Clamp(Intensity * 2.0f, 0.8f, 2.0f); 
        Pitch = 0.8f; 
    } else {
        TagToPlay = ImpactLightTag;
        Vol = FMath::Clamp((Intensity / 0.4f), 0.2f, 1.0f);
        Pitch = 1.2f; 
    }

    PlaySoundAtLocation(TagToPlay, Location, SurfaceHit, Vol, Pitch);
}

const FSonantCoreSurfaceDef* USonantCoreSubsystem::ResolveMaterial(const FHitResult& Hit) {
    const USonantCoreConfig* Config = USonantCoreSettings::Get();
    if (!Config) return nullptr;

    // 1. FAST PATH: Physical Material
    if (UPhysicalMaterial* Phys = Hit.PhysMaterial.Get()) {
        if (const FSonantCoreSurfaceDef* Found = Config->PhysicsMap.Find(Phys)) return Found;
    }

    // 2. CHECK CACHE (Material Interface)
    UMaterialInterface* Mat = nullptr;

    if (UPrimitiveComponent* Comp = Hit.GetComponent()) {
        if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Comp)) {
            if (Hit.FaceIndex != -1) {
                int32 SectionIndex;
                Mat = SMC->GetMaterialFromCollisionFaceIndex(Hit.FaceIndex, SectionIndex);
            }
        }
        
        if (!Mat) {
            Mat = Comp->GetMaterial(0);
        }
    }

    if (!Mat) return &Config->DefaultSurface;

    if (const FSonantCoreSurfaceDef** Cached = SmartCache.Find(Mat)) return *Cached;

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
