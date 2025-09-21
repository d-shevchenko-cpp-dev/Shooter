// ShootThemUp Game. All Right Reserved.

#include "STUBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacter, All, All);

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
    check(SpringArmComponent);
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    check(CameraComponent);
    CameraComponent->SetupAttachment(SpringArmComponent);

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>(TEXT("HealthComponent"));
    check(HealthComponent);

    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthTextComponent"));
    check(HealthTextComponent);
    HealthTextComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    OnHealthChanged(HealthComponent->GetHealth());
    
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    check(GetCharacterMovement());
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);
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
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUBaseCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUBaseCharacter::AddControllerYawInput);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUBaseCharacter::Jump);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUBaseCharacter::OnStartRunning);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUBaseCharacter::OnStopRunning);
}

bool ASTUBaseCharacter::IsRunning() const
{
    return IsCharacterMoving && IsCharacterRunning && !GetVelocity().IsZero();
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero())
    {
        return 0.f;
    }
    const auto VelocityNormal = GetVelocity().GetSafeNormal();
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
    const auto Degrees = FMath::DegreesToRadians(AngleBetween);
    return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::MoveForward(float Amount)
{
    IsCharacterMoving = Amount > 0.0f;
    AddMovementInput(GetActorForwardVector(), Amount);
}

void ASTUBaseCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}

void ASTUBaseCharacter::OnStartRunning()
{
    IsCharacterRunning = true;
}

void ASTUBaseCharacter::OnStopRunning()
{
    IsCharacterRunning = false;
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacter, All, TEXT("Player %s is dead"), *GetName());

    if (DeathAnimMontage)
    {
        PlayAnimMontage(DeathAnimMontage);
    }

    GetCharacterMovement()->DisableMovement();

    SetLifeSpan(5.0f);
}

void ASTUBaseCharacter::OnHealthChanged(float Health)
{
    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

