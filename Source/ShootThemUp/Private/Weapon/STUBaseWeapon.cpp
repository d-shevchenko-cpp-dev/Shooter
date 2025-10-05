// Игра ShootThemUp. Все права защищены.

#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/STUDamageComponent.h"
#include "Player/Components/STUHealthComponent.h"
#include "Weapon/STUWeaponDebugManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    // Создание компонентов
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    DamageComponent = CreateDefaultSubobject<USTUDamageComponent>(TEXT("DamageComponent"));

    // Создание менеджера отладки
    DebugManager = CreateDefaultSubobject<USTUWeaponDebugManager>(TEXT("DebugManager"));

    // Инициализация данных оружия значениями по умолчанию
    WeaponData = FSTUWeaponData();
    DebugData = FSTUWeaponDebugData();
}

// Fire() и StopFire() теперь виртуальные пустые методы
// Конкретная логика стрельбы должна быть реализована в дочерних классах

bool ASTUBaseWeapon::CanFire() const
{
    return IsValidForShooting() && WeaponData.ShotDelay >= MIN_SHOT_DELAY && WeaponData.ShotDelay <= MAX_SHOT_DELAY;
}

void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    // Валидация компонентов
    checkf(WeaponMesh, TEXT("WeaponMesh component is not valid"));
    checkf(DamageComponent, TEXT("DamageComponent is not valid"));
    checkf(DebugManager, TEXT("DebugManager is not valid"));

    // Инициализация менеджера отладки
    FSTUWeaponDebugSettings DebugSettings;
    DebugSettings.bEnableDebug = DebugData.bDrawDebugTrace;
    DebugSettings.DebugDuration = DebugData.DebugTraceDuration;
    DebugSettings.HitSphereRadius = DebugData.DebugHitSphereRadius;
    DebugManager->Initialize(DebugSettings);

    // Валидация данных оружия
    if (WeaponData.ShotDelay < MIN_SHOT_DELAY || WeaponData.ShotDelay > MAX_SHOT_DELAY)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Invalid ShotDelay: %f. Clamping to valid range"), WeaponData.ShotDelay);
        WeaponData.ShotDelay = FMath::Clamp(WeaponData.ShotDelay, MIN_SHOT_DELAY, MAX_SHOT_DELAY);
    }

    if (WeaponData.DamageAmount < MIN_DAMAGE || WeaponData.DamageAmount > MAX_DAMAGE)
    {
        UE_LOG(
            LogBaseWeapon, Warning, TEXT("Invalid DamageAmount: %f. Clamping to valid range"), WeaponData.DamageAmount);
        WeaponData.DamageAmount = FMath::Clamp(WeaponData.DamageAmount, MIN_DAMAGE, MAX_DAMAGE);
    }

    if (WeaponData.HeadshotMultiplier < MIN_HEADSHOT_MULTIPLIER ||
        WeaponData.HeadshotMultiplier > MAX_HEADSHOT_MULTIPLIER)
    {
        UE_LOG(LogBaseWeapon,
            Warning,
            TEXT("Invalid HeadshotMultiplier: %f. Clamping to valid range"),
            WeaponData.HeadshotMultiplier);
        WeaponData.HeadshotMultiplier =
            FMath::Clamp(WeaponData.HeadshotMultiplier, MIN_HEADSHOT_MULTIPLIER, MAX_HEADSHOT_MULTIPLIER);
    }

    UE_LOG(LogBaseWeapon, Log, TEXT("Weapon initialized successfully"));
}

// MakeShot() теперь виртуальный пустой метод
// Конкретная логика выстрела должна быть реализована в дочерних классах

bool ASTUBaseWeapon::IsValidForShooting() const
{
    if (!GetWorld())
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("World is not valid"));
        return false;
    }

    if (!WeaponMesh)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("WeaponMesh is not valid"));
        return false;
    }

    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Owner is not a Character"));
        return false;
    }

    // Проверяем, не мертв ли владелец оружия
    if (const auto* HealthComponent = OwnerCharacter->FindComponentByClass<USTUHealthComponent>())
    {
        if (HealthComponent->IsDead())
        {
            UE_LOG(LogBaseWeapon, Warning, TEXT("Owner is dead, cannot shoot"));
            return false;
        }
    }

    const APlayerController* PlayerController = OwnerCharacter->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("PlayerController is not valid"));
        return false;
    }

    return true;
}

FVector ASTUBaseWeapon::GetShootDirection(const FVector& BaseDirection) const
{
    const float SpreadRadians = GetCachedSpreadRadians();
    return FMath::VRandCone(BaseDirection, SpreadRadians);
}

bool ASTUBaseWeapon::IsHeadshot(const FName& HitBoneName) const
{
    return HitBoneName == WeaponData.HeadBoneName || HitBoneName == WeaponData.NeckBoneName;
}

float ASTUBaseWeapon::GetCachedSpreadRadians() const
{
    if (CachedSpreadRadians < 0.0f)
    {
        CachedSpreadRadians = FMath::DegreesToRadians(WeaponData.BulletSpread);
    }

    // Применяем модификатор здоровья, если он включен
    if (WeaponData.bEnableHealthSpreadModifier)
    {
        const float HealthSpreadMultiplier = GetHealthSpreadMultiplier();
        return CachedSpreadRadians * HealthSpreadMultiplier;
    }

    return CachedSpreadRadians;
}

float ASTUBaseWeapon::GetHealthSpreadMultiplier() const
{
    // Получаем владельца оружия
    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Owner is not a Character, using default spread"));
        return 1.0f;
    }

    // Получаем компонент здоровья
    const USTUHealthComponent* HealthComponent = OwnerCharacter->FindComponentByClass<USTUHealthComponent>();
    if (!HealthComponent)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Health component not found, using default spread"));
        return 1.0f;
    }

    // Получаем процент здоровья
    const float HealthPercent = HealthComponent->GetHealthPercent();

    // Если здоровье выше порогового значения, разброс не изменяется
    if (HealthPercent >= WeaponData.HealthSpreadThreshold)
    {
        return 1.0f;
    }

    // Вычисляем множитель разброса на основе здоровья
    // Чем меньше здоровья, тем больше разброс
    const float HealthRatio = HealthPercent / WeaponData.HealthSpreadThreshold;
    const float SpreadMultiplier = FMath::Lerp(WeaponData.MaxHealthSpreadMultiplier, 1.0f, HealthRatio);

    UE_LOG(LogBaseWeapon,
        VeryVerbose,
        TEXT("Health-based spread: HealthPercent=%.2f, Threshold=%.2f, Multiplier=%.2f"),
        HealthPercent,
        WeaponData.HealthSpreadThreshold,
        SpreadMultiplier);

    return SpreadMultiplier;
}

void ASTUBaseWeapon::PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.bTraceComplex = false; // Оптимизация для лучшей производительности

    GetWorld()->LineTraceSingleByChannel(
        HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
}

void ASTUBaseWeapon::DrawDebugTrace(const FVector& TraceStart,
    const FVector& TraceEnd,
    const FHitResult& HitResult) const
{
    if (!DebugData.bDrawDebugTrace || !GetWorld())
    {
        return;
    }

    // Отрисовка линии трейсинга
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, DebugData.DebugTraceDuration, 0, 3.0f);

    // Если попали в цель, рисуем сферу в точке попадания
    if (HitResult.bBlockingHit)
    {
        DrawDebugSphere(GetWorld(),
            HitResult.ImpactPoint,
            DebugData.DebugHitSphereRadius,
            24,
            FColor::Red,
            false,
            DebugData.DebugTraceDuration);
    }
}

float ASTUBaseWeapon::ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult)
{
    if (!Target || !DamageComponent)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Invalid target or damage component"));
        return 0.0f;
    }

    // Вычисление финального количества урона на основе места попадания
    float FinalDamage = WeaponData.DamageAmount;
    bool bIsHeadshot = IsHeadshot(HitResult.BoneName);

    // Применение множителя headshot
    if (bIsHeadshot)
    {
        FinalDamage *= WeaponData.HeadshotMultiplier;
        UE_LOG(LogBaseWeapon, Log, TEXT("Headshot! Damage multiplied by %f"), WeaponData.HeadshotMultiplier);
    }

    // Использование компонента урона для нанесения урона
    const bool bDamageDealt = DamageComponent->DealDamage(
        Target, FinalDamage, WeaponData.DamageType, HitResult.ImpactPoint, HitResult.BoneName, bIsHeadshot);

    if (bDamageDealt)
    {
        UE_LOG(LogBaseWeapon,
            Log,
            TEXT("Successfully dealt %f damage to %s%s"),
            FinalDamage,
            *Target->GetName(),
            bIsHeadshot ? TEXT(" (HEADSHOT)") : TEXT(""));
        return FinalDamage;
    }
    else
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Failed to deal damage to %s"), *Target->GetName());
        return 0.0f;
    }
}

void ASTUBaseWeapon::SetDebugEnabledForCategory(ESTUWeaponDebugCategory Category, bool bEnabled)
{
    if (DebugManager)
    {
        DebugManager->SetDebugEnabledForCategory(Category, bEnabled);
    }
}

void ASTUBaseWeapon::SetDebugEnabled(bool bEnabled)
{
    if (DebugManager)
    {
        DebugManager->SetDebugEnabled(bEnabled);
    }
}

void ASTUBaseWeapon::UpdateDebugSettings(const FSTUWeaponDebugSettings& Settings)
{
    if (DebugManager)
    {
        DebugManager->UpdateDebugSettings(Settings);
    }
}

void ASTUBaseWeapon::StartAutomaticFire()
{
    // Устанавливаем таймер для автоматической стрельбы
    if (WeaponData.ShotDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTUBaseWeapon::MakeShot, WeaponData.ShotDelay, true);
    }
}

void ASTUBaseWeapon::StopAutomaticFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}
