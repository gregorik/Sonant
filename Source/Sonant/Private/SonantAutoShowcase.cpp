#include "SonantAutoShowcase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "SonantSubsystem.h" 

ASonantAutoShowcase::ASonantAutoShowcase() {
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeAsset.Succeeded()) DefaultCube = CubeAsset.Object;

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (MatAsset.Succeeded()) DefaultMat = MatAsset.Object;

    Themes = {
        {"Stone", FLinearColor::White},
        {"Grass", FLinearColor::Green},
        {"Metal", FLinearColor(1.0f, 0.5f, 0.0f)}, // Orange
        {"Glass", FLinearColor(0.0f, 1.0f, 1.0f)}, // Cyan
        {"Wood",  FLinearColor(0.6f, 0.3f, 0.0f)}  // Brown
    };
}

void ASonantAutoShowcase::BeginPlay() {
    Super::BeginPlay();
    if (GetLocalRole() != ROLE_Authority) return;

    PlayerRef = UGameplayStatics::GetPlayerPawn(this, 0);
    NextSpawnLoc = GetActorLocation();

    // Spawn initial safe zone
    for (int i = 0; i < 20; i++) SpawnRow();
}

void ASonantAutoShowcase::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // 1. Handle Flash Decay (Visual Spectacle)
    for (int32 i = ActiveFlashes.Num() - 1; i >= 0; i--) {
        FHitFlash& Flash = ActiveFlashes[i];
        Flash.TimeRemaining -= DeltaTime * 3.0f; // Fade speed

        if (Flash.Mat) {
            // Lerp from Pure White (Flash) back to Base Theme Color
            FLinearColor CurrentColor = FMath::Lerp(Flash.BaseColor * 5.0f, FLinearColor::White * 50.0f, Flash.TimeRemaining);
            Flash.Mat->SetVectorParameterValue("Color", CurrentColor);
        }

        if (Flash.TimeRemaining <= 0) ActiveFlashes.RemoveAt(i);
    }

    if (!PlayerRef) return;

    // 2. Infinite Generation
    if (FVector::Dist(NextSpawnLoc, PlayerRef->GetActorLocation()) < 5000.0f) {
        SpawnRow();
        Cleanup();
    }
}

void ASonantAutoShowcase::SpawnRow() {
    if (!GetWorld() || !DefaultCube || !DefaultMat) return;

    int32 Idx = StepCounter % Themes.Num();
    FShowcaseSurface Theme = Themes[Idx];

    FActorSpawnParameters P;
    P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // --- CURVED RUNWAY LOGIC ---
    // Create a Sine wave elevation change
    float HeightOffset = FMath::Sin(StepCounter * 0.2f) * 600.0f;
    FVector SpawnPos = NextSpawnLoc;
    SpawnPos.Z += HeightOffset; // Apply wave

    // Spawn Floor
    AStaticMeshActor* Floor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnPos, FRotator::ZeroRotator, P);
    if (Floor) {
        Floor->SetMobility(EComponentMobility::Movable);
        Floor->GetStaticMeshComponent()->SetStaticMesh(DefaultCube);
        Floor->SetActorScale3D(FVector(4.0f, 4.0f, 0.2f));

        // Material Name for Sonant Logic
        FString MatName = FString::Printf(TEXT("M_Showcase_%s_%d"), *Theme.Keyword, StepCounter);
        UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(DefaultMat, Floor, FName(*MatName));
        DynMat->SetVectorParameterValue("Color", Theme.Color * 5.0f);
        Floor->GetStaticMeshComponent()->SetMaterial(0, DynMat);

        ActiveActors.Add(Floor);

        // Diegetic Text
        UTextRenderComponent* Text = NewObject<UTextRenderComponent>(Floor);
        Text->RegisterComponent();
        Text->AttachToComponent(Floor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        Text->SetAbsolute(false, false, true);
        Text->SetWorldScale3D(FVector(1.0f));
        Text->SetText(FText::FromString(Theme.Keyword.ToUpper()));
        Text->SetTextRenderColor(FColor::Black);
        Text->SetHorizontalAlignment(EHTA_Center);
        Text->SetVerticalAlignment(EVRTA_TextBottom);
        Text->SetWorldSize(150.0f);
        Text->SetRelativeLocation(FVector(0, 0, 60));
        Text->SetRelativeRotation(FRotator(90, 180, 0));
    }

    // --- METEOR DEBRIS (High Spectacle) ---
    if (StepCounter % 4 == 0) {
        FVector DropLoc = SpawnPos + FVector(0, 0, 2500); // High up
        AStaticMeshActor* Debris = GetWorld()->SpawnActor<AStaticMeshActor>(DropLoc, FRotator(FMath::RandRange(0, 360), FMath::RandRange(0, 360), 0), P);
        if (Debris) {
            Debris->SetMobility(EComponentMobility::Movable);
            Debris->GetStaticMeshComponent()->SetStaticMesh(DefaultCube);
            Debris->GetStaticMeshComponent()->SetSimulatePhysics(true);

            // Bind Hit
            Debris->GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
            Debris->GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &ASonantAutoShowcase::OnDebrisHit);

            // Logic: The Debris is made of the SAME Material as the floor below it
            if (Floor) Debris->GetStaticMeshComponent()->SetMaterial(0, Floor->GetStaticMeshComponent()->GetMaterial(0));

            // ADD LIGHT: Make it a glowing meteor
            UPointLightComponent* Light = NewObject<UPointLightComponent>(Debris);
            Light->RegisterComponent();
            Light->AttachToComponent(Debris->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            Light->SetLightColor(Theme.Color);
            Light->SetIntensity(5000.0f);
            Light->SetAttenuationRadius(1000.0f);

            // Add spin
            Debris->GetStaticMeshComponent()->AddAngularImpulseInDegrees(FVector(FMath::RandRange(-500, 500), FMath::RandRange(-500, 500), 0), NAME_None, true);

            ActiveActors.Add(Debris);
        }
    }

    NextSpawnLoc += FVector(400, 0, 0);

    // Vertical Step Logic: Ensure next spawn aligns with the Sine wave derivative slightly for smoothness, 
    // or just let the Sine reset absolute Z (simplest for parkour).
    // We already calculate Z absoulte above based on StepCounter, so just move X/Y forward.
    StepCounter++;
}

void ASonantAutoShowcase::Cleanup() {
    if (ActiveActors.Num() > 150) {
        if (ActiveActors[0]) ActiveActors[0]->Destroy();
        ActiveActors.RemoveAt(0);
    }
}

void ASonantAutoShowcase::OnDebrisHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
    if (NormalImpulse.Size() < 50.0f) return;

    // 1. TRIGGER AUDIO (The Logic)
    if (UGameInstance* GI = GetWorld()->GetGameInstance()) {
        if (USonantSubsystem* Sonant = GI->GetSubsystem<USonantSubsystem>()) {
            Sonant->PlayFootstep(Hit.Location, Hit);
        }
    }

    // 2. TRIGGER VISUALS (The Flash)
    // Identify what we hit (OtherActor = The Floor)
    if (OtherActor && OtherActor != HitComp->GetOwner()) {
        if (AStaticMeshActor* FloorActor = Cast<AStaticMeshActor>(OtherActor)) {
            UMaterialInterface* Mat = FloorActor->GetStaticMeshComponent()->GetMaterial(0);
            if (UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(Mat)) {

                // Check if already flashing
                bool bFound = false;
                for (auto& F : ActiveFlashes) if (F.Mat == DynMat) bFound = true;

                if (!bFound) {
                    FLinearColor CurrentColor;
                    DynMat->GetVectorParameterValue(FName("Color"), CurrentColor);

                    // Push to manager to animate clamp-down
                    // We divide by 5.0f because we multiplied earlier, approximating base color
                    FHitFlash NewFlash;
                    NewFlash.Mat = DynMat;
                    NewFlash.BaseColor = CurrentColor / 5.0f;
                    NewFlash.TimeRemaining = 1.0f;
                    ActiveFlashes.Add(NewFlash);

                    // Instant Spike to White
                    DynMat->SetVectorParameterValue("Color", FLinearColor::White * 100.0f);
                }
            }
        }
    }
}