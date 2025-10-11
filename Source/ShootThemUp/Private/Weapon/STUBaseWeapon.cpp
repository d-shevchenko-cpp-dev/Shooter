// ShootThemUp Game. All Right Reserved.

#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/STUDamageComponent.h"
#include "Components/STUHealthComponent.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    DamageComponent = CreateDefaultSubobject<USTUDamageComponent>(TEXT("DamageComponent"));

    WeaponConfiguration = CreateDefaultSubobject<USTUWeaponConfiguration>(TEXT("WeaponConfiguration"));

    CachedSpreadRadians = -1.0f;
}

bool ASTUBaseWeapon::CanFire() const
{
    if (!IsValidForShooting())
    {
        return false;
    }

    if (!WeaponConfiguration)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Weapon configuration is not valid"));
        return false;
    }

    const float ShotDelay = WeaponConfiguration->ShotDelay;
    return ShotDelay >= USTUWeaponConfiguration::MIN_SHOT_DELAY && ShotDelay <= USTUWeaponConfiguration::MAX_SHOT_DELAY;
}

void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (!ValidateComponents())
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("Weapon components validation failed"));
        return;
    }

    if (WeaponConfiguration)
    {
        WeaponConfiguration->ValidateConfiguration();
    }

    CurrentAmmo = DefaultAmmo;
}

void ASTUBaseWeapon::StartAutomaticFire()
{
    if (!WeaponConfiguration)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Weapon configuration is not valid"));
        return;
    }

    const float ShotDelay = WeaponConfiguration->ShotDelay;
    if (ShotDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTUBaseWeapon::MakeShot, ShotDelay, true);
    }
}

void ASTUBaseWeapon::StopAutomaticFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

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

    if (IsAmmoEmpty())
    {
        return false;
    }

    return true;
}

bool ASTUBaseWeapon::GetShotTrajectoryPoints(FVector& TraceStart, FVector& TraceEnd) const
{
    // Получение владельца оружия
    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("Owner is not a Character"));
        return false;
    }

    // Получение контроллера игрока
    const APlayerController* PlayerController = OwnerCharacter->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("PlayerController is not valid"));
        return false;
    }

    // Получение позиции и направления камеры
    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // Определение начальной точки трейсинга (позиция сокета дула)
    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
    TraceStart = SocketTransform.GetLocation();

    // Вычисляем направление от позиции оружия к точке, куда смотрит камера
    const FVector CameraDirection = CameraRotation.Vector();
    const FVector TargetPoint = CameraLocation + CameraDirection * WeaponConfiguration->MaxRange;
    const FVector ShootDirection = (TargetPoint - TraceStart).GetSafeNormal();
    const FVector FinalShootDirection = GetShootDirection(ShootDirection);
    TraceEnd = TraceStart + FinalShootDirection * WeaponConfiguration->MaxRange;

    return true;
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

float ASTUBaseWeapon::ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult)
{
    if (!Target || !DamageComponent || !WeaponConfiguration)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Invalid target, damage component, or weapon configuration"));
        return 0.0f;
    }

    // Вычисление финального количества урона на основе места попадания
    float FinalDamage = WeaponConfiguration->DamageAmount;
    bool bIsHeadshot = IsHeadshot(HitResult.BoneName);

    // Применение множителя headshot
    if (bIsHeadshot)
    {
        FinalDamage *= WeaponConfiguration->HeadshotMultiplier;
        UE_LOG(LogBaseWeapon, Log, TEXT("Headshot! Damage multiplied by %f"), WeaponConfiguration->HeadshotMultiplier);
    }

    // Использование компонента урона для нанесения урона
    const bool bDamageDealt = DamageComponent->DealDamage(
        Target, FinalDamage, WeaponConfiguration->DamageType, HitResult.ImpactPoint, HitResult.BoneName, bIsHeadshot);

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

bool ASTUBaseWeapon::IsHeadshot(const FName& HitBoneName) const
{
    if (!WeaponConfiguration)
    {
        return false;
    }

    return WeaponConfiguration->IsHeadshot(HitBoneName);
}

FVector ASTUBaseWeapon::GetShootDirection(const FVector& BaseDirection) const
{
    const float SpreadRadians = GetCachedSpreadRadians();
    return FMath::VRandCone(BaseDirection, SpreadRadians);
}

float ASTUBaseWeapon::GetCachedSpreadRadians() const
{
    if (CachedSpreadRadians < 0.0f && WeaponConfiguration)
    {
        CachedSpreadRadians = WeaponConfiguration->GetSpreadRadians();
    }

    // Применяем модификатор здоровья, если он включен
    if (WeaponConfiguration && WeaponConfiguration->bEnableHealthSpreadModifier)
    {
        // Здесь должна быть логика получения множителя здоровья
        // Пока что возвращаем базовый разброс
        return CachedSpreadRadians;
    }

    return CachedSpreadRadians;
}

bool ASTUBaseWeapon::ValidateComponents() const
{
    if (!WeaponMesh)
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("WeaponMesh component is not valid"));
        return false;
    }

    if (!DamageComponent)
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("DamageComponent is not valid"));
        return false;
    }

    if (!WeaponConfiguration)
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("WeaponConfiguration is not valid"));
        return false;
    }

    return true;
}

void ASTUBaseWeapon::DecreaseAmmo()
{
    CurrentAmmo.Bullets--;
    LogAmmo();

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        ChangeClip();
    }
}

bool ASTUBaseWeapon::IsAmmoEmpty() const
{
    return IsClipEmpty() && !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0;
}

bool ASTUBaseWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

void ASTUBaseWeapon::ChangeClip()
{
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;

    if (!CurrentAmmo.Infinite)
    {
        CurrentAmmo.Clips--;
    }
    UE_LOG(LogBaseWeapon, Display, TEXT("ASTUBaseWeapon::ChangeClip"));
}

void ASTUBaseWeapon::LogAmmo()
{
    FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.Bullets) + "/\n";
    AmmoInfo += CurrentAmmo.Infinite ? "Infinite\n" : FString::FromInt(CurrentAmmo.Clips);
    UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);
}
