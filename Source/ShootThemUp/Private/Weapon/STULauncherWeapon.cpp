#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "Weapon/Helpers/STUWeaponTraceHelper.h"
#include "Components/STUAmmoComponent.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogLauncherWeapon, All, All);

ASTULauncherWeapon::ASTULauncherWeapon()
{
    AmmoComponent->Initialize({ 1, 10, false });
}

void ASTULauncherWeapon::StartFire()
{
    if (!CanFire())
    {
        UE_LOG(LogLauncherWeapon, Warning, TEXT("Cannot fire launcher"));
        return;
    }

    UE_LOG(LogLauncherWeapon, Display, TEXT("Starting launcher fire"));

    // Выполняем один выстрел (гранатометы стреляют по одному)
    MakeShot();

    OnWeaponFireStarted.Broadcast();
}

void ASTULauncherWeapon::StopFire()
{
    UE_LOG(LogLauncherWeapon, Display, TEXT("Stopping launcher fire"));

    // Гранатометы обычно не требуют остановки стрельбы
    OnWeaponFireStopped.Broadcast();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!IsValidForShooting())
    {
        UE_LOG(LogLauncherWeapon, Warning, TEXT("Launcher is not valid for shooting"));
        return;
    }

    if (!ProjectileClass)
    {
        UE_LOG(LogLauncherWeapon, Error, TEXT("Projectile class is not set"));
        return;
    }

    UE_LOG(LogLauncherWeapon, Log, TEXT("Launcher shot - creating projectile"));

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

    // Создание и запуск снаряда
    LaunchProjectile(TraceStart, TraceEnd);

    // Уменьшаем патроны через компонент
    if (AmmoComponent)
    {
        AmmoComponent->TryDecreaseAmmo();
    }

    // Создаем пустой результат попадания для события
    FHitResult EmptyHitResult;
    OnWeaponShot.Broadcast(EmptyHitResult, 0.0f, false);
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

        UE_LOG(LogLauncherWeapon, Log, TEXT("Projectile launched successfully"));
    }
    else
    {
        UE_LOG(LogLauncherWeapon, Error, TEXT("Failed to spawn projectile"));
    }
}
