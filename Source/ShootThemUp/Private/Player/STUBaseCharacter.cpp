// Игра ShootThemUp. Все права защищены.

#include "STUBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacter, All, All);

ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    // Инициализация Spring Arm компонента
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
    check(SpringArmComponent);
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->SocketOffset = FVector(0.0f, SPRING_ARM_SOCKET_OFFSET_Y, SPRING_ARM_SOCKET_OFFSET_Z);

    // Инициализация Camera компонента
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    check(CameraComponent);
    CameraComponent->SetupAttachment(SpringArmComponent);

    // Инициализация Health компонента
    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>(TEXT("HealthComponent"));
    check(HealthComponent);

    // Инициализация Health Text компонента
    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthTextComponent"));
    check(HealthTextComponent);
    HealthTextComponent->SetupAttachment(GetRootComponent());
    HealthTextComponent->SetOwnerNoSee(true);

    // Инициализация Weapon компонента
    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>(TEXT("WeaponComponent"));
    check(WeaponComponent);
}

void ASTUBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Инициализация отображения здоровья
    check(HealthComponent);
    OnHealthChanged(HealthComponent->GetHealth());
    
    // Привязка событий компонента здоровья
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    // Привязка делегата приземления для урона от падения
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);
}

void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    check(WeaponComponent);

    // Привязки ввода движения
    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUBaseCharacter::MoveRight);
    
    // Привязки ввода камеры
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUBaseCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUBaseCharacter::AddControllerYawInput);

    // Привязки действий
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUBaseCharacter::Jump);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUBaseCharacter::OnStartRunning);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUBaseCharacter::OnStopRunning);
    
    // Привязки оружия
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Fire);
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

    const float Degrees = FMath::RadiansToDegrees(AngleBetween);
    return FMath::IsNearlyZero(CrossProduct.Z) ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
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

    // Воспроизведение анимации смерти, если доступна
    if (DeathAnimMontage)
    {
        PlayAnimMontage(DeathAnimMontage);
    }

    // Отключение движения персонажа
    check(GetCharacterMovement());
    GetCharacterMovement()->DisableMovement();

    // Установка времени жизни перед уничтожением
    SetLifeSpan(DEATH_LIFESPAN);

    // Изменение состояния контроллера на наблюдение
    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

void ASTUBaseCharacter::OnHealthChanged(float Health)
{
    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit)
{
    // Вычисление скорости падения (отрицательная Z скорость)
    const auto FallVelocityZ = GetCharacterMovement()->Velocity.Z * -1;

    // Проверка, если скорость падения ниже минимального порога урона
    if (FallVelocityZ < LandedDamageVelocity.X)
    {
        return;
    }

    // Вычисление урона на основе скорости падения с использованием сопоставленного диапазона
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity, LandedDamage, FallVelocityZ);
    TakeDamage(FinalDamage, FDamageEvent{}, nullptr, nullptr);
}

