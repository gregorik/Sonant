#include "SonantGodRayShowcase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "SonantSubsystem.h" 

ASonantGodRayShowcase::ASonantGodRayShowcase() {
    PrimaryActorTick.bCanEverTick = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneRoot;

    // 1. FORCE THE AESTHETIC (Post Process)
    PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("CyberFX"));
    PostProcess->SetupAttachment(RootComponent);
    PostProcess->bUnbound = true; // Global Effect
    PostProcess->Settings.bOverride_BloomMethod = true;
    PostProcess->Settings.BloomMethod = BM_SOG;
    PostProcess->Settings.bOverride_BloomIntensity = true;
    PostProcess->Settings.BloomIntensity = 4.0f; // Blinding Neon
    PostProcess->Settings.bOverride_AutoExposureMinBrightness = true;
    PostProcess->Settings.AutoExposureMinBrightness = 1.0f; // Prevent darkness
    PostProcess->Settings.bOverride_AutoExposureMaxBrightness = true;
    PostProcess->Settings.AutoExposureMaxBrightness = 1.0f;

    // 2. FORCE GOD RAYS (Volumetric Fog)
    VolumetricFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("Volumetrics"));
    VolumetricFog->SetupAttachment(RootComponent);
    VolumetricFog->SetVolumetricFog(true);
    VolumetricFog->SetVolumetricFogScatteringDistribution(0.9f); // Forward scattering (God rays)
    VolumetricFog->SetFogDensity(0.02f);
    VolumetricFog->SetFogHeightFalloff(0.2f);
    VolumetricFog->SetVolumetricFogExtinctionScale(2.0f);

    // 3. LOAD ASSETS
    static ConstructorHelpers::FObjectFinder<UStaticMesh> C(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (C.Succeeded()) MeshAsset = C.Object;
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> M(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (M.Succeeded()) MatAsset = M.Object;

    // 4. CONFIG
    Themes = {
        {"Glass", FLinearColor(0.0f, 1.0f, 1.0f)}, // Cyan
        {"Metal", FLinearColor(1.0f, 0.2f, 0.0f)}, // Orange
        {"Stone", FLinearColor::White},
        {"Grass", FLinearColor(0.0f, 1.0f, 0.0f)} // Matrix Green
    };
}

void ASonantGodRayShowcase::BeginPlay() {
    Super::BeginPlay();
    if (GetLocalRole() != ROLE_Authority) return;

    Player = UGameplayStatics::GetPlayerPawn(this, 0);
    HelixCursor = GetActorLocation();

    // Spawn 50 steps immediately
    for(int i=0; i<50; i++) SpawnHelixSegment();
}

void ASonantGodRayShowcase::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // 1. ANIMATE WALLS (The "Equalizer" Effect)
    // Makes the world pulse to a fake beat
    float Time = GetWorld()->GetTimeSeconds();
    for (FPulsingWall& Wall : WallPool) {
        if (Wall.Actor) {
            float Beat = FMath::Abs(FMath::Sin(Time * 5.0f + Wall.Offset));
            FVector NewScale = Wall.BaseScale;
            NewScale.Z *= (0.5f + Beat * 2.0f); // Stretch vertically violently
            Wall.Actor->SetActorScale3D(NewScale);
        }
    }

    if (!Player) return;

    // 2. INFINITE GENERATION
    if (FVector::Dist(HelixCursor, Player->GetActorLocation()) < 8000.0f) {
        SpawnHelixSegment();
        Cleanup();
    }
}

void ASonantGodRayShowcase::SpawnHelixSegment() {
    if (!GetWorld() || !MeshAsset) return;

    FGodRayTheme Theme = Themes[StepIndex % Themes.Num()];
    FActorSpawnParameters P;
    P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // --- CALCULATE HELIX POSITION ---
    // x = r * cos(t), y = r * sin(t), z = t
    float Angle = StepIndex * 0.2f;
    FVector CenterPos = HelixCursor;
    
    // The visual curve
    CenterPos += FVector(0, FMath::Cos(Angle) * 500.0f, FMath::Sin(Angle) * 500.0f); 

    // --- 1. THE FLOOR (Runway) ---
    AStaticMeshActor* Floor = GetWorld()->SpawnActor<AStaticMeshActor>(CenterPos, FRotator::ZeroRotator, P);
    if (Floor) {
        Floor->SetMobility(EComponentMobility::Movable);
        Floor->GetStaticMeshComponent()->SetStaticMesh(MeshAsset);
        Floor->SetActorScale3D(FVector(4.0f, 4.0f, 0.2f));
        
        // Sonant Registry Name
        FString Name = FString::Printf(TEXT("M_Helix_%s_%d"), *Theme.Keyword, StepIndex);
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(MatAsset, Floor, FName(*Name));
        MID->SetVectorParameterValue("Color", Theme.Color * 10.0f); // Emissive
        Floor->GetStaticMeshComponent()->SetMaterial(0, MID);

        // Diegetic Text
        UTextRenderComponent* Txt = NewObject<UTextRenderComponent>(Floor);
        Txt->RegisterComponent();
        Txt->AttachToComponent(Floor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        Txt->SetAbsolute(false, false, true);
        Txt->SetWorldScale3D(FVector(1.0f));
        Txt->SetText(FText::FromString(Theme.Keyword.ToUpper()));
        Txt->SetTextRenderColor(FColor::Black);
        Txt->SetHorizontalAlignment(EHTA_Center);
        Txt->SetVerticalAlignment(EVRTA_TextBottom);
        Txt->SetWorldSize(150.0f);
        Txt->SetRelativeLocation(FVector(0,0,60));
        Txt->SetRelativeRotation(FRotator(90, 180, 0));

        JunkPile.Add(Floor);
    }

    // --- 2. THE EQUALIZER WALLS (Pulsing Pillars) ---
    // Spawn two towers on Left and Right that pulse to music
    for(int Side = -1; Side <= 1; Side += 2) {
        FVector WallPos = CenterPos + (FVector(0, Side * 1200.0f, 0));
        AStaticMeshActor* Wall = GetWorld()->SpawnActor<AStaticMeshActor>(WallPos, FRotator::ZeroRotator, P);
        if (Wall) {
            Wall->SetMobility(EComponentMobility::Movable);
            Wall->GetStaticMeshComponent()->SetStaticMesh(MeshAsset);
            
            // Base Scale (Thin and Tall)
            FVector Scale(1.0f, 1.0f, 10.0f);
            Wall->SetActorScale3D(Scale);
            
            // Use same material as floor
            if (Floor) Wall->GetStaticMeshComponent()->SetMaterial(0, Floor->GetStaticMeshComponent()->GetMaterial(0));

            // Track for Tick animation
            FPulsingWall PW;
            PW.Actor = Wall;
            PW.BaseScale = Scale;
            PW.Offset = StepIndex * 0.5f; // Phase shift the wave
            WallPool.Add(PW);
            JunkPile.Add(Wall);
        }
    }

    // --- 3. THE LIGHT METEOR (Physics Audio) ---
    if (StepIndex % 4 == 0) {
        FVector DropPos = CenterPos + FVector(0, 0, 3000);
        AStaticMeshActor* Meteor = GetWorld()->SpawnActor<AStaticMeshActor>(DropPos, FRotator(FMath::RandRange(0,360),0,0), P);
        if (Meteor) {
            Meteor->SetMobility(EComponentMobility::Movable);
            Meteor->GetStaticMeshComponent()->SetStaticMesh(MeshAsset);
            Meteor->GetStaticMeshComponent()->SetSimulatePhysics(true);
            Meteor->GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
            Meteor->GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &ASonantGodRayShowcase::OnImpact);
            
            if (Floor) Meteor->GetStaticMeshComponent()->SetMaterial(0, Floor->GetStaticMeshComponent()->GetMaterial(0));

            // LIGHT FOR GOD RAYS
            UPointLightComponent* Light = NewObject<UPointLightComponent>(Meteor);
            Light->RegisterComponent();
            Light->AttachToComponent(Meteor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            Light->SetLightColor(Theme.Color);
            Light->SetIntensity(50000.0f); // Massive for volumetrics
            Light->SetAttenuationRadius(2000.0f);
            Light->SetCastShadows(true); // Needed for God Rays

            JunkPile.Add(Meteor);
        }
    }

    // Move Helix Forward linearly
    HelixCursor += FVector(400, 0, 0);
    StepIndex++;
}

void ASonantGodRayShowcase::OnImpact(UPrimitiveComponent* HitComp, AActor* O, UPrimitiveComponent* OC, FVector Imp, const FHitResult& Hit) {
    if (Imp.Size() < 50.0f) return;

    // Trigger Audio Logic (Sonant)
    if (UGameInstance* GI = GetWorld()->GetGameInstance()) {
        if (USonantSubsystem* Sys = GI->GetSubsystem<USonantSubsystem>()) {
            Sys->PlayFootstep(Hit.Location, Hit);
        }
    }

    // Flash Effect (Modify Material on impact)
    if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(HitComp->GetMaterial(0))) {
        MID->SetVectorParameterValue("Color", FLinearColor::White * 200.0f); // Blinding flash
    }
}

void ASonantGodRayShowcase::Cleanup() {
    if (JunkPile.Num() > 300) {
        if (JunkPile[0]) JunkPile[0]->Destroy();
        JunkPile.RemoveAt(0);
    }
    // Cleanup WallPool as well to prevent memory leak (simplified check)
    if (WallPool.Num() > 150) {
        WallPool.RemoveAt(0);
    }
}