// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "SonantCoreFoleyComponent.generated.h"

struct FStreamableHandle;
class UAudioComponent;
class USoundBase;

/** A lightweight, optionally velocity-driven loop with an external intensity API. */
UCLASS(ClassGroup = (SonantCore), meta = (BlueprintSpawnableComponent))
class SONANTCORE_API USonantCoreFoleyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USonantCoreFoleyComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "SonantCore|Foley")
    void SetFoleyIntensity(float NormalizedIntensity);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SonantCore|Foley")
    TSoftObjectPtr<USoundBase> FoleyLoopSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley")
    bool bUseOwnerVelocity = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley")
    bool bOnlyLocallyControlled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "0.01"))
    float UpdateInterval = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "0.0"))
    float MinVelocityThreshold = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "1.0"))
    float MaxVelocityThreshold = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "0.0"))
    float SmoothingSpeed = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "0.01"))
    float MinimumPitch = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "0.01"))
    float MaximumPitch = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley")
    bool bSendAudioParameters = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley")
    FName VolumeParameterName = TEXT("Volume");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley")
    FName PitchParameterName = TEXT("Pitch");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SonantCore|Foley", meta = (ClampMin = "0.0"))
    float StopWhenSilentDelay = 1.0f;

private:
    void BeginAsyncLoad();
    void CreateAudioComponent();

    UPROPERTY(Transient)
    TObjectPtr<UAudioComponent> FoleyAudio;

    TSharedPtr<FStreamableHandle> SoundLoadHandle;
    float AppliedUpdateInterval = 0.0f;
    float ManualIntensity = 0.0f;
    float SmoothedIntensity = 0.0f;
    float SilentTime = 0.0f;
};
