#include "Weapon/STURiffleWeapon.h"
#include "Weapon/Helpers/STUWeaponTraceHelper.h"
#include "Components/STUAmmoComponent.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRiffleWeapon, All, All);

ASTURiffleWeapon::ASTURiffleWeapon()
{
    AmmoComponent->Initialize({ 30, 5, false });
}

void ASTURiffleWeapon::StartFire()
{
    if (!CanFire())
    {
        UE_LOG(LogRiffleWeapon, Warning, TEXT("Cannot fire rifle"));
        return;
    }

    UE_LOG(LogRiffleWeapon, Display, TEXT("Starting rifle fire"));

    MakeShot();
    StartAutomaticFire();
}

void ASTURiffleWeapon::StopFire()
{
    StopAutomaticFire();
}

void ASTURiffleWeapon::StartAutomaticFire()
{
    if (!WeaponConfiguration)
    {
        return;
    }

    const float ShotDelay = WeaponConfiguration->ShotDelay;
    if (ShotDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURiffleWeapon::MakeShot, ShotDelay, true);
    }
}

void ASTURiffleWeapon::StopAutomaticFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void ASTURiffleWeapon::MakeShot()
{
    if (!IsValidForShooting())
    {
        if (AmmoComponent && AmmoComponent->IsAmmoEmpty())
        {
            StopFire();
        }
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
        UE_LOG(LogRiffleWeapon, Error, TEXT("Failed to calculate trajectory"));
        return;
    }

    FHitResult HitResult;
    USTUWeaponTraceHelper::PerformLineTrace(GetWorld(), GetOwner(), TraceStart, TraceEnd, HitResult);

    DrawDebugInformation(TraceStart, TraceEnd, HitResult);

    ProcessHitResult(HitResult);

    if (AmmoComponent)
    {
        AmmoComponent->TryDecreaseAmmo();
    }
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
    if (HitResult.bBlockingHit)
    {
        UE_LOG(LogRiffleWeapon,
            Log,
            TEXT("Rifle hit target: %s at bone: %s"),
            HitResult.GetActor() ? *HitResult.GetActor()->GetName() : TEXT("Unknown"),
            *HitResult.BoneName.ToString());

        if (AActor* HitActor = HitResult.GetActor())
        {
            ApplyDamageToTarget(HitActor, HitResult);
        }
    }
}
