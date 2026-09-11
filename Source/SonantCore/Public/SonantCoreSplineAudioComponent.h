// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "SonantCoreSplineAudioComponent.generated.h"

struct FStreamableHandle;
class UAudioComponent;
class USoundBase;
class USplineComponent;

/** Positions one persistent source at the point nearest any local audio listener. */
UCLASS(ClassGroup = (Sonant), meta = (BlueprintSpawnableComponent))
class SONANTCORE_API USonantCoreSplineAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USonantCoreSplineAudioComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SonantCore|Spline Audio")
    TSoftObjectPtr<USoundBase> SoundToPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Spline Audio", meta = (ClampMin = "0.0"))
    float ActivationRadius = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Spline Audio", meta = (ClampMin = "0.02"))
    float UpdateInterval = 0.1f;

private:
    void BeginAsyncLoad();
    void CreateAudioComponent();

    UPROPERTY(Transient)
    TObjectPtr<USplineComponent> CachedSpline;

    UPROPERTY(Transient)
    TObjectPtr<UAudioComponent> ActiveAudio;

    TSharedPtr<FStreamableHandle> SoundLoadHandle;
    float AppliedUpdateInterval = 0.0f;
};
