#pragma once

#include "CoreMinimal.h"
#include "STUWeaponTraceHelper.generated.h"

class USTUWeaponConfiguration;
class ACharacter;
struct FHitResult;

USTRUCT(BlueprintType)
struct FWeaponTraceParams
{
    GENERATED_BODY()

    UPROPERTY()
    AActor* Owner = nullptr;

    UPROPERTY()
    USkeletalMeshComponent* WeaponMesh = nullptr;

    UPROPERTY()
    FName MuzzleSocketName = TEXT("MuzzleSocket");

    UPROPERTY()
    const USTUWeaponConfiguration* WeaponConfig = nullptr;
};

UCLASS()
class SHOOTTHEMUP_API USTUWeaponTraceHelper : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Вычисляет точки начала и конца трассировки с учетом разброса
     * @param Params Параметры трассировки
     * @param TraceStart Начальная точка трассировки (результат)
     * @param TraceEnd Конечная точка трассировки (результат)
     * @return True если расчет успешен
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Trace")
    static bool CalculateTrajectory(const FWeaponTraceParams& Params, FVector& TraceStart, FVector& TraceEnd);

    /**
     * Выполняет линейную трассировку от начальной до конечной точки
     * @param WorldContext Контекст мира
     * @param Owner Владелец оружия (будет игнорироваться при трассировке)
     * @param TraceStart Начальная точка
     * @param TraceEnd Конечная точка
     * @param HitResult Результат трассировки
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Trace")
    static void PerformLineTrace(UWorld* WorldContext,
        AActor* Owner,
        const FVector& TraceStart,
        const FVector& TraceEnd,
        FHitResult& HitResult);

    /**
     * Вычисляет направление с учетом разброса
     * @param BaseDirection Базовое направление
     * @param SpreadRadians Разброс в радианах
     * @return Направление с учетом разброса
     */
    UFUNCTION(BlueprintPure, Category = "Weapon Trace")
    static FVector CalculateSpreadDirection(const FVector& BaseDirection, float SpreadRadians);

private:
    /**
     * Получает точку взгляда игрока
     * @param Owner Владелец оружия
     * @param ViewLocation Позиция камеры (результат)
     * @param ViewRotation Поворот камеры (результат)
     * @return True если получение успешно
     */
    static bool GetPlayerViewPoint(AActor* Owner, FVector& ViewLocation, FRotator& ViewRotation);
};
