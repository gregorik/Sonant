// Copyright (c) 2025 GregOrigin. All Rights Reserved.

#pragma once
#include "Components/BoxComponent.h"
#include "SonantSubsystem.h"
#include "Engine/World.h"                    // <--- FIX: Added for GetWorld()
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "SonantVolume.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class SONANT_API USonantVolume : public UBoxComponent {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sonant")
    FGameplayTag AtmosphereTag;

protected:
    virtual void BeginPlay() override {
        Super::BeginPlay();
        OnComponentBeginOverlap.AddDynamic(this, &USonantVolume::OnOverlap);
        OnComponentEndOverlap.AddDynamic(this, &USonantVolume::OnEndOverlap);
    }

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* O, AActor* A, UPrimitiveComponent* OC, int32 I, bool bS, const FHitResult& R) {
        if (A == GetWorld()->GetFirstPlayerController()->GetPawn()) {
            GetWorld()->GetGameInstance()->GetSubsystem<USonantSubsystem>()->PushAtmosphere(AtmosphereTag);
        }
    }

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* O, AActor* A, UPrimitiveComponent* OC, int32 I) {
        if (A == GetWorld()->GetFirstPlayerController()->GetPawn()) {
            GetWorld()->GetGameInstance()->GetSubsystem<USonantSubsystem>()->PopAtmosphere(AtmosphereTag);
        }
    }
};