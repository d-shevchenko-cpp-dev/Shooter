// ShootThemUp Game. All Right Reserved.

#include "Weapon/Helpers/STUWeaponTraceHelper.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponTraceHelper, All, All);

bool USTUWeaponTraceHelper::CalculateTrajectory(const FWeaponTraceParams& Params,
    FVector& TraceStart,
    FVector& TraceEnd)
{
    if (!Params.Owner || !Params.WeaponMesh || !Params.WeaponConfig)
    {
        UE_LOG(LogWeaponTraceHelper, Error, TEXT("Invalid trace parameters"));
        return false;
    }

    // Получение точки взгляда игрока
    FVector CameraLocation;
    FRotator CameraRotation;
    if (!GetPlayerViewPoint(Params.Owner, CameraLocation, CameraRotation))
    {
        return false;
    }

    // Начальная точка - позиция дула оружия
    const FTransform SocketTransform = Params.WeaponMesh->GetSocketTransform(Params.MuzzleSocketName);
    TraceStart = SocketTransform.GetLocation();

    // Вычисляем направление от оружия к точке прицеливания
    const FVector CameraDirection = CameraRotation.Vector();
    const FVector TargetPoint = CameraLocation + CameraDirection * Params.WeaponConfig->MaxRange;
    const FVector ShootDirection = (TargetPoint - TraceStart).GetSafeNormal();

    // Применяем разброс
    const float SpreadRadians = Params.WeaponConfig->GetSpreadRadians();
    const FVector FinalDirection = CalculateSpreadDirection(ShootDirection, SpreadRadians);

    // Конечная точка трассировки
    TraceEnd = TraceStart + FinalDirection * Params.WeaponConfig->MaxRange;

    return true;
}

void USTUWeaponTraceHelper::PerformLineTrace(UWorld* WorldContext,
    AActor* Owner,
    const FVector& TraceStart,
    const FVector& TraceEnd,
    FHitResult& HitResult)
{
    if (!WorldContext)
    {
        UE_LOG(LogWeaponTraceHelper, Error, TEXT("World context is null"));
        return;
    }

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.bTraceComplex = false; // Оптимизация

    WorldContext->LineTraceSingleByChannel(
        HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
}

FVector USTUWeaponTraceHelper::CalculateSpreadDirection(const FVector& BaseDirection, float SpreadRadians)
{
    return FMath::VRandCone(BaseDirection, SpreadRadians);
}

bool USTUWeaponTraceHelper::GetPlayerViewPoint(AActor* Owner, FVector& ViewLocation, FRotator& ViewRotation)
{
    const ACharacter* OwnerCharacter = Cast<ACharacter>(Owner);
    if (!OwnerCharacter)
    {
        UE_LOG(LogWeaponTraceHelper, Error, TEXT("Owner is not a Character"));
        return false;
    }

    const APlayerController* PlayerController = OwnerCharacter->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogWeaponTraceHelper, Error, TEXT("PlayerController is not valid"));
        return false;
    }

    PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}
