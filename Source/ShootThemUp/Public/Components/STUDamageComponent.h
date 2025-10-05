// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUDamageComponent.generated.h"

/**
 * Компонент, который обрабатывает логику нанесения урона.
 * Предоставляет централизованную функциональность нанесения урона.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUDamageComponent();

    /**
     * Наносит урон целевому актору.
     * @param Target Актор для нанесения урона
     * @param DamageAmount Количество урона для нанесения
     * @param DamageType Тип урона
     * @param HitLocation Место, где был нанесен урон
     * @param HitBoneName Имя кости, которая была поражена
     * @param bIsHeadshot Является ли это headshot'ом
     * @return True если урон был успешно нанесен
     */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    bool DealDamage(AActor* Target,
        float DamageAmount,
        TSubclassOf<UDamageType> DamageType,
        const FVector& HitLocation = FVector::ZeroVector,
        const FName& HitBoneName = NAME_None,
        bool bIsHeadshot = false);

protected:
    virtual void BeginPlay() override;

private:
    /**
     * Внутренний метод для применения урона с использованием системы урона Unreal.
     * @param Target Целевой актор
     * @param DamageAmount Количество урона
     * @param DamageType Тип урона
     * @param HitLocation Место попадания
     * @param HitBoneName Имя пораженной кости
     * @param bIsHeadshot Является ли это headshot'ом
     * @return True если урон был применен
     */
    bool ApplyDamageInternal(AActor* Target,
        float DamageAmount,
        TSubclassOf<UDamageType> DamageType,
        const FVector& HitLocation,
        const FName& HitBoneName,
        bool bIsHeadshot);

    /**
     * Получает контроллер, который владеет этим компонентом.
     * @return Указатель на контроллер или nullptr, если не найден
     */
    AController* GetInstigatorController() const;
};
