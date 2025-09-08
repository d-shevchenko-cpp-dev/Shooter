// ShootThemUp Game. All Right Reserved.


#include "STUBaseCharacter.h"
#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/SpringArmComponent.h>

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
    

}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    constexpr auto FORWARD_AXIS_NAME = "MoveForward";
    constexpr auto RIGHT_AXIS_NAME = "MoveRight";
    constexpr auto LOOK_UP_AXIS_NAME = "LookUp";
    constexpr auto TURN_AROUND_AXIS_NAME = "TurnAround";
    
    PlayerInputComponent->BindAxis(FORWARD_AXIS_NAME, this, &ASTUBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis(RIGHT_AXIS_NAME, this, &ASTUBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis(LOOK_UP_AXIS_NAME, this, &ASTUBaseCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis(TURN_AROUND_AXIS_NAME, this, &ASTUBaseCharacter::AddControllerYawInput);
}

void ASTUBaseCharacter::MoveForward(float Amount)
{
    AddMovementInput(GetActorForwardVector(), Amount);
}

void ASTUBaseCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}