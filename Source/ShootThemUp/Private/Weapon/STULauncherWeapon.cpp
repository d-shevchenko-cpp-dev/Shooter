#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "Weapon/Helpers/STUWeaponTraceHelper.h"
#include "Components/STUAmmoComponent.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogLauncherWeapon, All, All);

ASTULauncherWeapon::ASTULauncherWeapon() {}

void ASTULauncherWeapon::StartFire()
{
    if (!CanFire())
    {
        return;
    }

    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!IsValidForShooting() || !ProjectileClass)
    {
        return;
    }

    // Подготавливаем параметры для трассировки
    FWeaponTraceParams TraceParams;
    TraceParams.Owner = GetOwner();
    TraceParams.WeaponMesh = GetWeaponMesh();
    TraceParams.MuzzleSocketName = MuzzleSocketName;
    TraceParams.WeaponConfig = GetWeaponConfiguration();

    // Вычисляем траекторию с помощью хелпера
    FVector TraceStart, TraceEnd;
    if (!USTUWeaponTraceHelper::CalculateTrajectory(TraceParams, TraceStart, TraceEnd))
    {
        UE_LOG(LogLauncherWeapon, Error, TEXT("Failed to calculate trajectory"));
        return;
    }

    LaunchProjectile(TraceStart, TraceEnd);

    if (AmmoComponent)
    {
        AmmoComponent->TryDecreaseAmmo();
    }
}

void ASTULauncherWeapon::BeginPlay()
{
    Super::BeginPlay();
    AmmoComponent->Initialize({ 1, 10, false });
}

void ASTULauncherWeapon::LaunchProjectile(const FVector& TraceStart, const FVector& TraceEnd)
{
    // Вычисляем направление запуска снаряда
    const FVector LaunchDirection = (TraceEnd - TraceStart).GetSafeNormal();
    const FRotator LaunchRotation = LaunchDirection.Rotation();

    // Создаем снаряд в позиции дула с правильным направлением
    const FTransform SpawnTransform(LaunchRotation, TraceStart);
    auto Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);

    if (Projectile)
    {
        Projectile->SetShootDirection(LaunchDirection);
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(SpawnTransform);
    }
    else
    {
        UE_LOG(LogLauncherWeapon, Error, TEXT("Failed to spawn projectile"));
    }
}
