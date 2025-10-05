// ShootThemUp Game. All Right Reserved.

#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"

DEFINE_LOG_CATEGORY_STATIC(LogLauncherWeapon, All, All);

ASTULauncherWeapon::ASTULauncherWeapon()
{
    // Конструктор по умолчанию
}

void ASTULauncherWeapon::Fire()
{
    if (!CanFire())
    {
        UE_LOG(LogLauncherWeapon, Warning, TEXT("Cannot fire launcher"));
        return;
    }

    UE_LOG(LogLauncherWeapon, Display, TEXT("Starting launcher fire"));

    // Выполняем один выстрел (гранатометы обычно стреляют по одному выстрелу)
    MakeShot();

    // Вызываем событие начала стрельбы
    OnWeaponFireStarted.Broadcast();
}

void ASTULauncherWeapon::StopFire()
{
    UE_LOG(LogLauncherWeapon, Display, TEXT("Stopping launcher fire"));

    // Гранатометы обычно не требуют остановки стрельбы
    // В будущем здесь может быть логика отмены прицеливания или перезарядки

    // Вызываем событие остановки стрельбы
    OnWeaponFireStopped.Broadcast();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!IsValidForShooting())
    {
        UE_LOG(LogLauncherWeapon, Warning, TEXT("Launcher is not valid for shooting"));
        return;
    }

    UE_LOG(LogLauncherWeapon, Log, TEXT("Launcher shot - using trajectory points"));

    // Получение точек траектории выстрела
    FVector TraceStart, TraceEnd;
    if (!GetShotTrajectoryPoints(TraceStart, TraceEnd))
    {
        UE_LOG(LogLauncherWeapon, Error, TEXT("Failed to get shot trajectory points"));
        return;
    }

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

    // TODO: В будущем здесь будет реализация:
    // 1. Передача направления и силы снаряду
    // 2. Установка параметров запуска (сила, гравитация)
    // 3. Обработка событий попадания/промаха

    // Временная заглушка - создаем пустой результат попадания
    FHitResult EmptyHitResult;
    OnWeaponShot.Broadcast(EmptyHitResult, 0.0f, false);
}
