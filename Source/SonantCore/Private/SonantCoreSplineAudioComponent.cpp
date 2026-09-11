// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "SonantCoreSplineAudioComponent.h"

#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSonantSpline, Log, All);

USonantCoreSplineAudioComponent::USonantCoreSplineAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USonantCoreSplineAudioComponent::BeginPlay()
{
    Super::BeginPlay();
    PrimaryComponentTick.TickInterval = FMath::Max(0.02f, UpdateInterval);
    AppliedUpdateInterval = UpdateInterval;
    CachedSpline = GetOwner() ? GetOwner()->FindComponentByClass<USplineComponent>() : nullptr;
    if (CachedSpline)
    {
        BeginAsyncLoad();
    }
    else
    {
        UE_LOG(
            LogSonantSpline,
            Warning,
            TEXT("%s has no USplineComponent on its owner; spline audio is disabled."),
            *GetPathNameSafe(this));
    }
}

void USonantCoreSplineAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SoundLoadHandle.IsValid() && !SoundLoadHandle->HasLoadCompleted())
    {
        SoundLoadHandle->CancelHandle();
    }
    SoundLoadHandle.Reset();
    if (ActiveAudio)
    {
        ActiveAudio->Stop();
        ActiveAudio->DestroyComponent();
        ActiveAudio = nullptr;
    }
    CachedSpline = nullptr;
    Super::EndPlay(EndPlayReason);
}

void USonantCoreSplineAudioComponent::BeginAsyncLoad()
{
    if (SoundToPlay.IsNull())
    {
        return;
    }
    if (SoundToPlay.Get())
    {
        CreateAudioComponent();
        return;
    }

    const TWeakObjectPtr<USonantCoreSplineAudioComponent> WeakThis(this);
    SoundLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        SoundToPlay.ToSoftObjectPath(),
        [WeakThis]()
        {
            if (USonantCoreSplineAudioComponent* Component = WeakThis.Get())
            {
                Component->SoundLoadHandle.Reset();
                Component->CreateAudioComponent();
            }
        },
        FStreamableManager::DefaultAsyncLoadPriority,
        false,
        false,
        TEXT("Sonant Spline Audio"));
}

void USonantCoreSplineAudioComponent::CreateAudioComponent()
{
    AActor* Owner = GetOwner();
    USoundBase* Sound = SoundToPlay.Get();
    if (!Owner || !CachedSpline || !Sound || ActiveAudio)
    {
        return;
    }

    ActiveAudio = NewObject<UAudioComponent>(
        Owner,
        MakeUniqueObjectName(Owner, UAudioComponent::StaticClass(), TEXT("SonantSplineAudio")));
    ActiveAudio->bAutoActivate = false;
    ActiveAudio->bAutoDestroy = false;
    ActiveAudio->SetupAttachment(CachedSpline);
    ActiveAudio->SetSound(Sound);
    ActiveAudio->RegisterComponent();
    SetComponentTickEnabled(true);
}

void USonantCoreSplineAudioComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!FMath::IsNearlyEqual(AppliedUpdateInterval, UpdateInterval))
    {
        AppliedUpdateInterval = UpdateInterval;
        PrimaryComponentTick.TickInterval = FMath::Max(0.02f, UpdateInterval);
    }

    if (!CachedSpline || !ActiveAudio)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    float BestDistanceSquared = TNumericLimits<float>::Max();
    FVector BestSplinePoint = FVector::ZeroVector;
    bool bFoundListener = false;
    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (!PlayerController || !PlayerController->IsLocalController())
        {
            continue;
        }

        FVector ListenerLocation;
        FVector Front;
        FVector Right;
        PlayerController->GetAudioListenerPosition(ListenerLocation, Front, Right);
        const FVector SplinePoint = CachedSpline->FindLocationClosestToWorldLocation(
            ListenerLocation,
            ESplineCoordinateSpace::World);
        const float DistanceSquared = FVector::DistSquared(ListenerLocation, SplinePoint);
        if (DistanceSquared < BestDistanceSquared)
        {
            BestDistanceSquared = DistanceSquared;
            BestSplinePoint = SplinePoint;
            bFoundListener = true;
        }
    }

    const bool bInRange = bFoundListener
        && (ActivationRadius <= 0.0f || BestDistanceSquared <= FMath::Square(ActivationRadius));
    if (!bInRange)
    {
        if (ActiveAudio->IsPlaying())
        {
            ActiveAudio->Stop();
        }
        return;
    }

    ActiveAudio->SetWorldLocation(BestSplinePoint);
    if (!ActiveAudio->IsPlaying())
    {
        ActiveAudio->Play();
    }
}
