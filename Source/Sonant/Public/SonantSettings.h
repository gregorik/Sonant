// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#pragma once
#include "Engine/DeveloperSettings.h"
#include "SonantConfig.h"
#include "SonantSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Sonant Audio"))
class SONANT_API USonantSettings : public UDeveloperSettings {
    GENERATED_BODY()
public:
    UPROPERTY(Config, EditAnywhere, Category="Core")
    TSoftObjectPtr<USonantConfig> MainConfig;

    static const USonantConfig* Get() {
        return GetDefault<USonantSettings>()->MainConfig.LoadSynchronous();
    }
};