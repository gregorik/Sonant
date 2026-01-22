#include "SonantDemoGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

ASonantDemoGenerator::ASonantDemoGenerator() {
    PrimaryActorTick.bCanEverTick = true;

    // Create the dummy root so we can place it in the level
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneRoot;

    // Define the "Logic Palette"
    // These keywords MUST match what is in your DA_Sonant_Global config!
    SurfaceTypes.Add({ "Stone", FLinearColor(0.8f, 0.8f, 0.8f, 1.0f) }); // White
    SurfaceTypes.Add({ "Grass", FLinearColor(0.0f, 1.0f, 0.0f, 1.0f) }); // Green
    SurfaceTypes.Add({ "Metal", FLinearColor(1.0f, 0.2f, 0.0f, 1.0f) }); // Orange
    SurfaceTypes.Add({ "Glass", FLinearColor(0.0f, 1.0f, 1.0f, 1.0f) }); // Cyan
    SurfaceTypes.Add({ "Wood",  FLinearColor(0.6f, 0.4f, 0.2f, 1.0f) }); // Brown
}

void ASonantDemoGenerator::BeginPlay() {
    Super::BeginPlay();

    PlayerRef = UGameplayStatics::GetPlayerPawn(this, 0);
    NextSpawnLocation = GetActorLocation();

    // FIREWALL: Only run visual simulation on the machine controlling the camera.
    // This stops the Server from spawning invisible "ghost" platforms that overlap yours.
    if (PlayerRef && PlayerRef->IsLocallyControlled()) {
        // Pre-seed 20 steps so the runway exists immediately
        for (int i = 0; i < 20; i++) {
            SpawnPlatform();
        }
    }
}

void ASonantDemoGenerator::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // FIREWALL: Stop logic if we are a server or remote client
    if (!PlayerRef || !PlayerRef->IsLocallyControlled()) return;

    // Infinite Generation Logic
    if (GetWorld()) {
        float Dist = FVector::Dist(PlayerRef->GetActorLocation(), NextSpawnLocation);

        // If player is within 40 meters (4000 units) of the edge, spawn more
        if (Dist < 4000.0f) {
            SpawnPlatform();
        }

        CleanupOldPlatforms();
    }
}

void ASonantDemoGenerator::SpawnPlatform() {
    if (!GetWorld() || SurfaceTypes.Num() == 0) return;

    // 1. Pick a random surface logic
    int32 RndIndex = FMath::RandRange(0, SurfaceTypes.Num() - 1);
    const FDemoSurface& SelectedType = SurfaceTypes[RndIndex];

    // 2. Spawn the Mesh Actor
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AStaticMeshActor* NewPlat = GetWorld()->SpawnActor<AStaticMeshActor>(NextSpawnLocation, FRotator::ZeroRotator, Params);

    if (NewPlat) {
        // CLIENT-SIDE VISUALS ONLY: Disable replication to prevent "Ghost" platforms from Server
        NewPlat->SetReplicates(false);
        NewPlat->SetReplicateMovement(false);
        NewPlat->SetMobility(EComponentMobility::Movable);

        if (PlatformMesh) {
            NewPlat->GetStaticMeshComponent()->SetStaticMesh(PlatformMesh);
            NewPlat->SetActorScale3D(FVector(2.0f, 2.0f, 0.2f));
        }

        // 3. APPLY MATERIAL & SONANT LOGIC
        // Naming format: "M_Procedural_[Keyword]_[Index]"
        FString UniqueName = FString::Printf(TEXT("M_Procedural_%s_%d"), *SelectedType.Description, StepCount);

        if (BaseMaterial) {
            UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMaterial, NewPlat, FName(*UniqueName));
            if (DynMat) {
                DynMat->SetVectorParameterValue("BaseColor", SelectedType.Color);
                DynMat->SetScalarParameterValue("Emissive", 5.0f);
                NewPlat->GetStaticMeshComponent()->SetMaterial(0, DynMat);
            }
        }

        // 4. ATTACH TEXT
        UTextRenderComponent* Text = NewObject<UTextRenderComponent>(NewPlat);
        if (Text) {
            Text->RegisterComponent();

            // FIX: Disable collision so the footstep trace hits the FLOOR, not the text
            Text->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Text->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

            Text->AttachToComponent(NewPlat->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

            // FIX: Ignore parent scaling (2.0) to prevent garbled text
            Text->SetAbsolute(false, false, true);
            Text->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

            // FIX: Formatting and Orientation
            FString Label = FString::Printf(TEXT("DETECTED:\n%s"), *SelectedType.Description.ToUpper());
            Text->SetText(FText::FromString(Label));

            Text->SetHorizontalAlignment(EHTA_Center);
            Text->SetVerticalAlignment(EVRTA_TextBottom); // Grow upwards
            Text->SetTextRenderColor(FColor::Black);      // Black text on Neon floor
            Text->SetWorldSize(60.0f);

            Text->SetRelativeLocation(FVector(0, 0, 50.0f));

            // FIX: Rotate 180 on Yaw so it is readable by the player (not inverted)
            Text->SetRelativeRotation(FRotator(90.0f, 180.0f, 0.0f));
        }

        ActivePlatforms.Add(NewPlat);
    }

    // 5. Advance Logic
    NextSpawnLocation += FVector(400.0f, 0.0f, 0.0f);
    StepCount++;
}

// THIS WAS THE MISSING FUNCTION CAUSING THE LINKER ERROR
void ASonantDemoGenerator::CleanupOldPlatforms() {
    // Keep only the last 40 platforms to save performance
    if (ActivePlatforms.Num() > 40) {
        if (ActivePlatforms[0]) {
            ActivePlatforms[0]->Destroy();
        }
        ActivePlatforms.RemoveAt(0);
    }
}