// Fill out your copyright notice in the Description page of Project Settings.

#include "RunnerCharacter.h"

#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WallSpike.h"
#include "Engine.h"

const float FALL_THRESHOLD = -1000.0f;

// Sets default values
ARunnerCharacter::ARunnerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    // Stop the user from rotating the Character
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = true;
    bUseControllerRotationYaw = true;

    // Create the Camera
    SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Side View Camera"));
    // Stop the controller from rotating the Camera
    SideViewCamera->bUsePawnControlRotation = false;

    // Rotate the character towards the direction we are going
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // Rotation Rate
    GetCharacterMovement()->RotationRate = FRotator(0.0f, RotationRate, 0.0f);

    // Set defaults for controlling the Character
    GetCharacterMovement()->GravityScale = 2.5f;
    GetCharacterMovement()->AirControl = 0.5f;
    GetCharacterMovement()->JumpZVelocity = 1000.0f;
    GetCharacterMovement()->GroundFriction = 3.0f;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->MaxFlySpeed = 600.0f;

    // Find the Characters temporary position
    TempPos = GetActorLocation();
    // The Camera height position
    zPosition = TempPos.Z + 150.0f;
}

// Called when the game starts or when spawned
void ARunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Get the Capsule Component
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnOverlapBegin);

    CanMove = true;
    CanJump = true;
    bCanDoubleJump = true; // set CanDoubleJump to true by default
}

// Called every frame
void ARunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TempPos = GetActorLocation();
    TempPos.X -= 800;
    TempPos.Z = zPosition;
    SideViewCamera->SetWorldLocation(TempPos);

    // Check for falling beyond threshold
    if (GetActorLocation().Z < FALL_THRESHOLD)
    {
        RestartLevel();
    }
}

// Called to bind functionality to input
void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind Character Input functionality
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunnerCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAxis("MoveRight", this, &ARunnerCharacter::MoveRight);
}

void ARunnerCharacter::Jump()
{
    if (CanJump && !GetCharacterMovement()->IsFalling())
    {
        // First jump
        Super::Jump();
        CanDoubleJump = true;
    }
    else if (CanDoubleJump)
    {
        // Double jump
        LaunchCharacter(FVector(0, 0, DoubleJumpZVelocity), false, true);
        CanDoubleJump = false;
    }
}

void ARunnerCharacter::MoveRight(float Value)
{
    if (CanMove)
    {
        FVector Direction = FVector(0.0f, 1.0f, 0.0f); // Keep movement only along the Y-axis
        AddMovementInput(Direction, Value);
    }
}

void ARunnerCharacter::RestartLevel()
{
    // Call restart level
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void ARunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor != nullptr)
    {
        AWallSpike* WallSpike = Cast<AWallSpike>(OtherActor);
        ASpikes* Spike = Cast<ASpikes>(OtherActor);

        // Check if we collide with either Wall or Spike Actor
        if (WallSpike || Spike)
        {
            // get the mesh and deactivate it
            GetMesh()->Deactivate();
            // Set the visibility of the mesh to false
            GetMesh()->SetVisibility(false);
            // disable movement input
            CanMove = false;
            // disable Jump input
            CanJump = false;
            CanDoubleJump = false;
            JumpZVelocity = 0.0f;

            // Called in character blueprint
            DeathOfPlayer();

            // restart the Level 
            FTimerHandle UnusedHandle;
            GetWorldTimerManager().SetTimer(UnusedHandle, this, &ARunnerCharacter::RestartLevel, 2.0f, false);
        }
    }
}
