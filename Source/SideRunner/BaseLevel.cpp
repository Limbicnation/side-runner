#include "BaseLevel.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseLevel::ABaseLevel()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize Trigger and SpawnLocation components
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    RootComponent = Trigger;

    SpawnLocation = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnLocation"));
    SpawnLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseLevel::BeginPlay()
{
    Super::BeginPlay();

    // Hide Trigger component in game
    if (Trigger)
    {
        Trigger->SetHiddenInGame(true);
    }
}

// Called every frame
void ABaseLevel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

UBoxComponent* ABaseLevel::GetTrigger() const
{
    return Trigger;
}

UBoxComponent* ABaseLevel::GetSpawnLocation() const
{
    return SpawnLocation;
}
