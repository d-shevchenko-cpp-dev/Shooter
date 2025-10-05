// ShootThemUp Game. All Right Reserved.

#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STU_Projectile.h"
#include "kismet/GameplayStatics.h"
#include "Engine/World.h"

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

    UE_LOG(LogLauncherWeapon, Log, TEXT("Launcher shot - placeholder implementation"));

    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
    const FVector SocketLocation = SocketTransform.GetLocation();

    const FTransform SpawnTransform(FRotator::ZeroRotator, SocketLocation);
    auto Projectile = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ProjectileClass, SpawnTransform);
    // set projectile params
    UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);

    // TODO: В будущем здесь будет реализация:
    // 1. Создание снаряда (гранаты/ракеты)
    // 2. Установка параметров запуска (сила, направление, гравитация)
    // 3. Запуск снаряда в направлении цели
    // 4. Обработка событий попадания/промаха

    // Временная заглушка - создаем пустой результат попадания
    FHitResult EmptyHitResult;
    OnWeaponShot.Broadcast(EmptyHitResult, 0.0f, false);
}
