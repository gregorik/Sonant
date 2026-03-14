// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once
#include "Engine/DeveloperSettings.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "SonantCoreConfig.h"
#include "SonantCoreSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="SonantCore Audio"))
class SONANTCORE_API USonantCoreSettings : public UDeveloperSettings {
    GENERATED_BODY()
public:
    USonantCoreSettings() {
        DefaultFootstepTag = FGameplayTag::RequestGameplayTag(FName("SonantCore.Event.Footstep"));
        DefaultLightImpactTag = FGameplayTag::RequestGameplayTag(FName("SonantCore.Event.Impact.Light"));
        DefaultHeavyImpactTag = FGameplayTag::RequestGameplayTag(FName("SonantCore.Event.Impact.Heavy"));
    }

    // Configurable path to the Main Config
    UPROPERTY(Config, EditAnywhere, Category="Core")
    TSoftObjectPtr<USonantCoreConfig> MainConfig;

    UPROPERTY(Config, EditAnywhere, Category="Tags")
    FGameplayTag DefaultFootstepTag;

    UPROPERTY(Config, EditAnywhere, Category="Tags")
    FGameplayTag DefaultLightImpactTag;

    UPROPERTY(Config, EditAnywhere, Category="Tags")
    FGameplayTag DefaultHeavyImpactTag;

    static const USonantCoreConfig* Get() {
        return GetDefault<USonantCoreSettings>()->MainConfig.LoadSynchronous();
    }
};