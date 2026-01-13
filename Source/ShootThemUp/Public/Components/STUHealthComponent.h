#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeath)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUHealthComponent();

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    bool IsDead() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    bool IsFullHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    bool AddHealth(float HealAmount);

    FOnDeath OnDeath;

    FOnHealthChanged OnHealthChanged;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor* DamagedActor,
        float Damage,
        const class UDamageType* DamageType,
        class AController* InstigatedBy,
        AActor* DamageCauser);

    void HealUpdate();

    void SetHealth(float NewHealth);

    bool ValidateHealthParameters() const;

protected:
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Health",
        meta = (ClampMin = 1.f, ClampMax = 10000.f, ToolTip = "Maximum health value"))
    float MaxHealth{ 100.f };

    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta = (ToolTip = "Enable automatic health regeneration"))
    bool AutoHeal{ true };

    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta = (EditCondition = "AutoHeal",
            ClampMin = 0.01f,
            ClampMax = 10.f,
            ToolTip = "Time interval between each heal update"))
    float HealUpdateTime{ 0.3f };

    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta = (EditCondition = "AutoHeal",
            ClampMin = 0.f,
            ClampMax = 60.f,
            ToolTip = "Delay before auto-healing starts after taking damage"))
    float HealDelay{ 3.f };

    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Heal",
        meta = (EditCondition = "AutoHeal",
            ClampMin = 0.1f,
            ClampMax = 100.f,
            ToolTip = "Amount of health restored per heal update"))
    float HealModifier{ 1.f };

private:
    FTimerHandle HealTimerHandle;

    float Health{ 0.f };

    static constexpr float MIN_HEALTH_THRESHOLD = 0.01f;
    static constexpr float HEALTH_PERCENTAGE_MULTIPLIER = 100.f;
};
