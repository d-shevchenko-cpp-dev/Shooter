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

    // Получение точек траектории выстрела
    FVector TraceStart, TraceEnd;
    if (!GetShotTrajectoryPoints(TraceStart, TraceEnd))
    {
        UE_LOG(LogRiffleWeapon, Error, TEXT("Failed to get shot trajectory points"));
        return;
    }

    // Выполнение линейного трейсинга
    FHitResult HitResult;
    PerformLineTrace(TraceStart, TraceEnd, HitResult);

    // Отображение отладочной информации
    DrawDebugInformation(TraceStart, TraceEnd, HitResult);

    // Обработка результата попадания и применение урона
    ProcessHitResult(HitResult);
}

void ASTURiffleWeapon::DrawDebugInformation(const FVector& TraceStart,
    const FVector& TraceEnd,
    const FHitResult& HitResult) const
{
    if (!GetWorld())
    {
        return;
    }

    // Отрисовка линии трейсинга
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 3.0f, 0, 3.0f);

    // Получение позиции камеры для отладочной информации
    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    const APlayerController* PlayerController =
        OwnerCharacter ? OwnerCharacter->GetController<APlayerController>() : nullptr;

    if (PlayerController)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

        const FVector CameraDirection = CameraRotation.Vector();
        const FVector TargetPoint = CameraLocation + CameraDirection * GetWeaponConfiguration()->MaxRange;

        // Отрисовка направления камеры
        DrawDebugLine(GetWorld(), CameraLocation, TargetPoint, FColor::Blue, false, 3.0f, 0, 2.0f);

        // Отрисовка сферы разброса
        DrawDebugSphere(GetWorld(), TargetPoint, 5.0f, 8, FColor::Yellow, false, 3.0f);
    }

    // Если попали в цель, рисуем сферу в точке попадания
    if (HitResult.bBlockingHit)
    {
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 3.0f);
    }
}

void ASTURiffleWeapon::ProcessHitResult(const FHitResult& HitResult)
{
    float DamageDealt = 0.0f;
    bool bIsHeadshot = false;

    if (HitResult.bBlockingHit)
    {
        UE_LOG(LogRiffleWeapon,
            Log,
            TEXT("Rifle hit target: %s at bone: %s"),
            HitResult.GetActor() ? *HitResult.GetActor()->GetName() : TEXT("Unknown"),
            *HitResult.BoneName.ToString());

        if (AActor* HitActor = HitResult.GetActor())
        {
            bIsHeadshot = IsHeadshot(HitResult.BoneName);
            DamageDealt = ApplyDamageToTarget(HitActor, HitResult);
        }
    }

    OnWeaponShot.Broadcast(HitResult, DamageDealt, bIsHeadshot);
}
