#include "SpawnLevel.h"
#include "BaseLevel.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnLevel::ASpawnLevel()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpawnLevel::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the player is properly initialized
    Player = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (Player)
    {
        // Spawn the initial levels
        SpawnInitialLevels();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player not found at BeginPlay. Spawning will be delayed."));
    }
}

// Called every frame
void ASpawnLevel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Check if the player is now available and initialize levels if needed
    if (!Player)
    {
        Player = GetWorld()->GetFirstPlayerController()->GetPawn();
        if (Player)
        {
            SpawnInitialLevels();
        }
    }
}

void ASpawnLevel::SpawnInitialLevels()
{
    if (LevelList.Num() == 0)
    {
        for (int32 i = 0; i < 4; ++i)
        {
            SpawnLevel(i == 0);
        }
    }
}

void ASpawnLevel::SpawnLevel(bool IsFirst)
{
    FVector NewSpawnLocation = FVector(0.0f, 1000.0f, 0.0f);
    FRotator NewSpawnRotation = FRotator(0, 90, 0);

    if (!IsFirst && LevelList.Num() > 0)
    {
        ABaseLevel* LastLevel = LevelList.Last();
        if (LastLevel)
        {
            NewSpawnLocation = LastLevel->GetSpawnLocation()->GetComponentTransform().GetTranslation();
        }
    }

    int32 MaxLevels = 6; // Adjust according to the number of level types available
    int32 RandomLevel = FMath::RandRange(1, MaxLevels);
    ABaseLevel* NewLevel = nullptr;

    TSubclassOf<ABaseLevel> LevelClass = nullptr;
    switch (RandomLevel)
    {
    case 1: LevelClass = Level1; break;
    case 2: LevelClass = Level2; break;
    case 3: LevelClass = Level3; break;
    case 4: LevelClass = Level4; break;
    case 5: LevelClass = Level5; break;
    case 6: LevelClass = Level6; break;
    default: UE_LOG(LogTemp, Warning, TEXT("Invalid level number: %d"), RandomLevel); break;
    }

    if (LevelClass)
    {
        NewLevel = GetWorld()->SpawnActor<ABaseLevel>(LevelClass, NewSpawnLocation, NewSpawnRotation, FActorSpawnParameters());
    }

    if (NewLevel)
    {
        if (NewLevel->GetTrigger())
        {
            NewLevel->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &ASpawnLevel::OnOverlapBegin);
        }

        LevelList.Add(NewLevel);

        if (LevelList.Num() > MaxLevels)
        {
            ABaseLevel* OldestLevel = LevelList[0];
            if (OldestLevel)
            {
                LevelList.RemoveAt(0);
                OldestLevel->Destroy();
            }
        }
    }
}

void ASpawnLevel::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor == Player)
    {
        SpawnLevel(false);
    }
}
