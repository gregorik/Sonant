// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "SonantCoreFoleyComponent.h"

#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundBase.h"

USonantCoreFoleyComponent::USonantCoreFoleyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USonantCoreFoleyComponent::BeginPlay()
{
    Super::BeginPlay();
    PrimaryComponentTick.TickInterval = FMath::Max(0.01f, UpdateInterval);
    AppliedUpdateInterval = UpdateInterval;
    BeginAsyncLoad();
}

void USonantCoreFoleyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SoundLoadHandle.IsValid() && !SoundLoadHandle->HasLoadCompleted())
    {
        SoundLoadHandle->CancelHandle();
    }
    SoundLoadHandle.Reset();

    if (FoleyAudio)
    {
        FoleyAudio->Stop();
        FoleyAudio->DestroyComponent();
        FoleyAudio = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}

void USonantCoreFoleyComponent::BeginAsyncLoad()
{
    if (FoleyLoopSound.IsNull())
    {
        return;
    }
    if (FoleyLoopSound.Get())
    {
        CreateAudioComponent();
        return;
    }

    const TWeakObjectPtr<USonantCoreFoleyComponent> WeakThis(this);
    SoundLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        FoleyLoopSound.ToSoftObjectPath(),
        [WeakThis]()
        {
            if (USonantCoreFoleyComponent* Component = WeakThis.Get())
            {
                Component->SoundLoadHandle.Reset();
                Component->CreateAudioComponent();
            }
        },
        FStreamableManager::DefaultAsyncLoadPriority,
        false,
        false,
        TEXT("Sonant Foley"));
}

void USonantCoreFoleyComponent::CreateAudioComponent()
{
    AActor* Owner = GetOwner();
    USoundBase* Sound = FoleyLoopSound.Get();
    if (!Owner || !Owner->GetRootComponent() || !Sound || FoleyAudio)
    {
        return;
    }

    // A fixed name collides when an actor carries more than one foley component.
    FoleyAudio = NewObject<UAudioComponent>(
        Owner,
        MakeUniqueObjectName(Owner, UAudioComponent::StaticClass(), TEXT("SonantFoleyAudio")));
    FoleyAudio->bAutoActivate = false;
    FoleyAudio->bAutoDestroy = false;
    FoleyAudio->SetupAttachment(Owner->GetRootComponent());
    FoleyAudio->SetSound(Sound);
    FoleyAudio->RegisterComponent();
    SetComponentTickEnabled(true);
}

void USonantCoreFoleyComponent::SetFoleyIntensity(float NormalizedIntensity)
{
    ManualIntensity = FMath::Clamp(
        FMath::IsFinite(NormalizedIntensity) ? NormalizedIntensity : 0.0f,
        0.0f,
        1.0f);
}

void USonantCoreFoleyComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // UpdateInterval is BlueprintReadWrite, so honour changes made after BeginPlay.
    if (!FMath::IsNearlyEqual(AppliedUpdateInterval, UpdateInterval))
    {
        AppliedUpdateInterval = UpdateInterval;
        PrimaryComponentTick.TickInterval = FMath::Max(0.01f, UpdateInterval);
    }

    if (!FoleyAudio || !GetOwner())
    {
        return;
    }

    float TargetIntensity = ManualIntensity;
    if (bUseOwnerVelocity)
    {
        const float Speed = GetOwner()->GetVelocity().Size();
        TargetIntensity = FMath::GetMappedRangeValueClamped(
            FVector2D(MinVelocityThreshold, FMath::Max(MinVelocityThreshold + 1.0f, MaxVelocityThreshold)),
            FVector2D(0.0f, 1.0f),
            Speed);
    }

    if (bOnlyLocallyControlled)
    {
        const APawn* Pawn = Cast<APawn>(GetOwner());
        if (!Pawn || !Pawn->IsLocallyControlled())
        {
            TargetIntensity = 0.0f;
        }
    }

    SmoothedIntensity = FMath::FInterpTo(
        SmoothedIntensity,
        TargetIntensity,
        FMath::Max(0.0f, DeltaTime),
        SmoothingSpeed);
    const float Pitch = FMath::Lerp(MinimumPitch, MaximumPitch, SmoothedIntensity);

    FoleyAudio->SetVolumeMultiplier(SmoothedIntensity);
    FoleyAudio->SetPitchMultiplier(Pitch);
    if (bSendAudioParameters)
    {
        FoleyAudio->SetFloatParameter(VolumeParameterName, SmoothedIntensity);
        FoleyAudio->SetFloatParameter(PitchParameterName, Pitch);
    }

    if (SmoothedIntensity > UE_KINDA_SMALL_NUMBER)
    {
        SilentTime = 0.0f;
        if (!FoleyAudio->IsPlaying())
        {
            FoleyAudio->Play();
        }
    }
    else if (FoleyAudio->IsPlaying())
    {
        SilentTime += FMath::Max(0.0f, DeltaTime);
        if (SilentTime >= FMath::Max(0.0f, StopWhenSilentDelay))
        {
            FoleyAudio->Stop();
            SilentTime = 0.0f;
        }
    }
}
