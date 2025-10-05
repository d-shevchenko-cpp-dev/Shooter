// ShootThemUp Game. All Right Reserved.

#include "Weapon/STURiffleWeapon.h"
#include "Player/Components/STUHealthComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRiffleWeapon, All, All);

ASTURiffleWeapon::ASTURiffleWeapon()
{
    // Конструктор по умолчанию
}

void ASTURiffleWeapon::Fire()
{
    if (!CanFire())
    {
        UE_LOG(LogRiffleWeapon, Warning, TEXT("Cannot fire rifle"));
        return;
    }

    UE_LOG(LogRiffleWeapon, Display, TEXT("Starting rifle fire"));

    // Выполняем первый выстрел немедленно
    MakeShot();

    // Запускаем автоматическую стрельбу
    StartAutomaticFire();

    // Вызываем событие начала стрельбы
    OnWeaponFireStarted.Broadcast();
}

void ASTURiffleWeapon::StopFire()
{
    UE_LOG(LogRiffleWeapon, Display, TEXT("Stopping rifle fire"));

    // Останавливаем автоматическую стрельбу
    StopAutomaticFire();

    // Вызываем событие остановки стрельбы
    OnWeaponFireStopped.Broadcast();
}

void ASTURiffleWeapon::MakeShot()
{
    if (!IsValidForShooting())
    {
        UE_LOG(LogRiffleWeapon, Warning, TEXT("Rifle is not valid for shooting"));
        return;
    }

    // Получение владельца оружия
    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogRiffleWeapon, Error, TEXT("Owner is not a Character"));
        return;
    }

    // Получение контроллера игрока
    const APlayerController* PlayerController = OwnerCharacter->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogRiffleWeapon, Error, TEXT("PlayerController is not valid"));
        return;
    }

    // Получение позиции и направления камеры
    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // Определение точек трейсинга
    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
    const FVector TraceStart = SocketTransform.GetLocation();

    // Вычисляем направление от позиции оружия к точке, куда смотрит камера
    const FVector CameraDirection = CameraRotation.Vector();
    const FVector TargetPoint = CameraLocation + CameraDirection * WeaponData.MaxRange;
    const FVector ShootDirection = (TargetPoint - TraceStart).GetSafeNormal();
    const FVector FinalShootDirection = GetShootDirection(ShootDirection);
    const FVector TraceEnd = TraceStart + FinalShootDirection * WeaponData.MaxRange;

    // Выполнение линейного трейсинга
    FHitResult HitResult;
    PerformLineTrace(TraceStart, TraceEnd, HitResult);

    // Отображение отладочной информации через менеджер
    if (DebugManager)
    {
        DebugManager->DrawTraceLine(GetWorld(), TraceStart, TraceEnd);
        DebugManager->DrawCameraLine(GetWorld(), CameraLocation, TargetPoint);
        DebugManager->DrawSpreadSphere(GetWorld(), TargetPoint);

        if (HitResult.bBlockingHit)
        {
            DebugManager->DrawHitSphere(GetWorld(), HitResult.ImpactPoint);
        }
    }

    // Обработка результата попадания и применение урона
    float DamageDealt = 0.0f;
    bool bIsHeadshot = false;

    if (HitResult.bBlockingHit)
    {
        UE_LOG(LogRiffleWeapon,
            Log,
            TEXT("Rifle hit target: %s at bone: %s"),
            HitResult.GetActor() ? *HitResult.GetActor()->GetName() : TEXT("Unknown"),
            *HitResult.BoneName.ToString());

        // Применение урона к пораженной цели
        if (AActor* HitActor = HitResult.GetActor())
        {
            bIsHeadshot = IsHeadshot(HitResult.BoneName);
            DamageDealt = ApplyDamageToTarget(HitActor, HitResult);
        }
    }
    else
    {
        UE_LOG(LogRiffleWeapon, VeryVerbose, TEXT("Rifle shot missed"));
    }

    // Вызываем событие выстрела
    OnWeaponShot.Broadcast(HitResult, DamageDealt, bIsHeadshot);
}
