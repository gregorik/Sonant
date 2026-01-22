#include "SonantSpectacle.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ASonantSpectacle::ASonantSpectacle() {
    PrimaryActorTick.bCanEverTick = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Default Themes so it looks cool out of the box
    Themes.Add({"Glass", FLinearColor(0.0f, 1.0f, 1.0f, 1.0f), 0.0f}); // Cyan
    Themes.Add({"Metal", FLinearColor(1.0f, 0.2f, 0.0f, 1.0f), 0.3f}); // Orange
    Themes.Add({"Stone", FLinearColor(0.8f, 0.8f, 0.8f, 1.0f), 0.8f}); // White
    Themes.Add({"Grass", FLinearColor(0.1f, 1.0f, 0.1f, 1.0f), 1.0f}); // Matrix Green
}

void ASonantSpectacle::BeginPlay() {
    Super::BeginPlay();
    PlayerRef = UGameplayStatics::GetPlayerPawn(this, 0);
    NextRowLocation = GetActorLocation();

    // Pre-warm the runway
    for (int i = 0; i < 15; i++) {
        SpawnRow();
    }
}

void ASonantSpectacle::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (PlayerRef) {
        // 1. Check Distance to generate ahead
        float DistToEnd = FVector::Dist(PlayerRef->GetActorLocation(), NextRowLocation);
        if (DistToEnd < SpawnDistance) {
            SpawnRow();
        }

        // 2. Animate and Cleanup
        AnimatePlatforms(DeltaTime);
        CleanupVoid();
    }
}

void ASonantSpectacle::SpawnRow() {
    if (!GetWorld() || Themes.Num() == 0) return;

    // Spawn 3 Lanes (Left, Center, Right)
    for (int Lane = -1; Lane <= 1; Lane++) {
        
        // Randomly skip some tiles to create gaps (Parkour element)
        if (FMath::RandRange(0.0f, 1.0f) > 0.85f) continue;

        // Pick a theme based on Row Count (Striped pattern)
        // This organizes the chaos: 5 rows of Glass, then 5 of Metal, etc.
        int32 ThemeIndex = (TotalRowsSpawned / 5) % Themes.Num();
        // Add random noise occasionally
        if (FMath::RandBool()) ThemeIndex = FMath::RandRange(0, Themes.Num() - 1);
        
        const FSpectacleTheme& Theme = Themes[ThemeIndex];

        // Calculate Position
        FVector SpawnPos = NextRowLocation + (GetActorRightVector() * Lane * LaneWidth);
        FVector StartPos = SpawnPos - FVector(0, 0, 1000.0f); // Start deep below

        // Spawn Actor
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AStaticMeshActor* NewTile = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnPos, FRotator::ZeroRotator, Params);
        
        if (NewTile && TileMesh) {
            NewTile->SetMobility(EComponentMobility::Movable);
            NewTile->SetActorLocation(StartPos); // Set initial position for animation
            NewTile->GetStaticMeshComponent()->SetStaticMesh(TileMesh);
            NewTile->SetActorScale3D(FVector(2.0f, 2.0f, 0.2f));

            // --- THE SONANT LOGIC ---
            // Naming the material specifically to trigger regex
            FString ValidatedName = FString::Printf(TEXT("M_Gen_%s_%d_%d"), *Theme.Keyword, TotalRowsSpawned, Lane);
            
            if (MasterMaterial) {
                UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(MasterMaterial, NewTile, FName(*ValidatedName));
                MID->SetVectorParameterValue("EmissiveColor", Theme.Color * 5.0f); // HDR Bloom power
                MID->SetScalarParameterValue("Roughness", Theme.Roughness);
                NewTile->GetStaticMeshComponent()->SetMaterial(0, MID);
            }

            // --- VISUALIZER TEXT ---
            UTextRenderComponent* Txt = NewObject<UTextRenderComponent>(NewTile);
            Txt->RegisterComponent();
            Txt->AttachToComponent(NewTile->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            Txt->SetText(FText::FromString(Theme.Keyword.ToUpper()));
            Txt->SetTextRenderColor(Theme.Color.ToFColor(true));
            Txt->SetWorldSize(60.0f);
            Txt->SetRelativeLocation(FVector(0, 0, 60)); 
            Txt->SetRelativeRotation(FRotator(90, 180, 0)); // Flat on ground
            Txt->SetHorizontalAlignment(EHTA_Center);

            // Track it
            FActivePlatform ActivePlat;
            ActivePlat.Actor = NewTile;
            ActivePlat.DebugText = Txt;
            ActivePlat.TargetLocation = SpawnPos;
            ActivePlat.SpawnTime = GetWorld()->GetTimeSeconds();
            PlatformPool.Add(ActivePlat);
        }
    }

    NextRowLocation += GetActorForwardVector() * ForwardStep;
    TotalRowsSpawned++;
}

void ASonantSpectacle::AnimatePlatforms(float DeltaTime) {
    // Smoothly rise platforms from the void
    for (FActivePlatform& Plat : PlatformPool) {
        if (Plat.Actor) {
            FVector CurrentLoc = Plat.Actor->GetActorLocation();
            FVector NewLoc = FMath::VInterpTo(CurrentLoc, Plat.TargetLocation, DeltaTime, 5.0f);
            Plat.Actor->SetActorLocation(NewLoc);
        }
    }
}

void ASonantSpectacle::CleanupVoid() {
    // Delete platforms behind the player to save performance
    if (PlatformPool.Num() > 0 && PlayerRef) {
        FVector PlayerLoc = PlayerRef->GetActorLocation();
        
        // Remove if 2000 units behind player
        // Iterate backwards safely
        for (int32 i = PlatformPool.Num() - 1; i >= 0; i--) {
            if (PlatformPool[i].Actor) {
                 float Dist = FVector::Dist(PlatformPool[i].Actor->GetActorLocation(), PlayerLoc);
                 FVector ForwardDir = GetActorForwardVector();
                 FVector ToPlayer = PlayerLoc - PlatformPool[i].Actor->GetActorLocation();
                 
                 // Dot product check: is it behind?
                 // Simple distance check is safer for infinite runners
                 if (Dist > 8000.0f && FVector::DotProduct(ForwardDir, ToPlayer) > 0) {
                     PlatformPool[i].Actor->Destroy();
                     PlatformPool.RemoveAt(i);
                 }
            }
        }
    }
}