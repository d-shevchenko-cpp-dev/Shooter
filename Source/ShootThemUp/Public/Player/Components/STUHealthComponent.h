// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeath)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

/**
 * Компонент здоровья, который управляет здоровьем персонажа, уроном и функциональностью автоматического лечения.
 * Предоставляет события для изменений здоровья и состояния смерти.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUHealthComponent();

    /**
     * Получает текущее значение здоровья.
     * @return Текущее количество здоровья
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    float GetHealth() const;

    /**
     * Получает максимальное значение здоровья.
     * @return Максимальное количество здоровья
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    float GetMaxHealth() const { return MaxHealth; }

    /**
     * Получает процент здоровья (от 0.0 до 1.0).
     * @return Процент здоровья
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    float GetHealthPercent() const;

    /**
     * Проверяет, мертв ли персонаж (здоровье <= 0).
     * @return True если мертв, false в противном случае
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    bool IsDead() const;

    /**
     * Проверяет, находится ли персонаж на полном здоровье.
     * @return True если на полном здоровье, false в противном случае
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    bool IsFullHealth() const;

    /**
     * Добавляет здоровье к текущему значению здоровья.
     * @param HealAmount Количество здоровья для добавления
     * @return True если здоровье было добавлено, false если уже на максимальном здоровье
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    bool AddHealth(float HealAmount);

    /**
     * Событие, срабатывающее при смерти персонажа.
     */
    FOnDeath OnDeath;

    /**
     * Событие, срабатывающее при изменении здоровья.
     * @param NewHealth Новое значение здоровья
     */
    FOnHealthChanged OnHealthChanged;

protected:
    virtual void BeginPlay() override;

private:
    /**
     * Обрабатывает урон, полученный актором.
     * @param DamagedActor Актор, который получил урон
     * @param Damage Количество полученного урона
     * @param DamageType Тип урона
     * @param InstigatedBy Контроллер, который причинил урон
     * @param DamageCauser Актор, который причинил урон
     */
    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor* DamagedActor,
        float Damage,
        const class UDamageType* DamageType,
        class AController* InstigatedBy,
        AActor* DamageCauser);

    /**
     * Обновляет здоровье во время процесса автоматического лечения.
     */
    void HealUpdate();

    /**
     * Устанавливает значение здоровья и запускает событие изменения здоровья.
     * @param NewHealth Новое значение здоровья для установки
     */
    void SetHealth(float NewHealth);

    /**
     * Проверяет параметры, связанные со здоровьем.
     * @return True если параметры действительны, false в противном случае
     */
    bool ValidateHealthParameters() const;

protected:
    /** Максимальное значение здоровья */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Health",
        meta=(ClampMin = 1.f, ClampMax = 10000.f, ToolTip="Maximum health value"))
    float MaxHealth{ 100.f };

    /** Включено ли автоматическое лечение */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta=(ToolTip="Enable automatic health regeneration"))
    bool AutoHeal{ true };

    /** Временной интервал между обновлениями лечения */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta=(EditCondition = "AutoHeal", ClampMin = 0.1f, ClampMax = 10.f,
            ToolTip="Time interval between each heal update"))
    float HealUpdateTime{ 0.3f };

    /** Задержка перед началом автоматического лечения после получения урона */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta=(EditCondition = "AutoHeal", ClampMin = 0.f, ClampMax = 60.f,
            ToolTip="Delay before auto-healing starts after taking damage"))
    float HealDelay{ 3.f };

    /** Количество здоровья, восстанавливаемого за одно обновление лечения */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta=(EditCondition = "AutoHeal", ClampMin = 0.1f, ClampMax = 100.f,
            ToolTip="Amount of health restored per heal update"))
    float HealModifier{ 1.f };

private:
    /** Дескриптор таймера для автоматического лечения */
    FTimerHandle HealTimerHandle;

    /** Текущее значение здоровья */
    float Health{ 0.f };

    /** Константы */
    static constexpr float MIN_HEALTH_THRESHOLD = 0.01f;
    static constexpr float HEALTH_PERCENTAGE_MULTIPLIER = 100.f;
};