// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeath)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

/**
 * Health component that manages character health, damage, and auto-healing functionality.
 * Provides events for health changes and death state.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUHealthComponent();

    /**
     * Gets the current health value.
     * @return Current health amount
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    float GetHealth() const;

    /**
     * Gets the maximum health value.
     * @return Maximum health amount
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    float GetMaxHealth() const { return MaxHealth; }

    /**
     * Gets the health percentage (0.0 to 1.0).
     * @return Health percentage
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    float GetHealthPercent() const;

    /**
     * Checks if the character is dead (health <= 0).
     * @return True if dead, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    bool IsDead() const;

    /**
     * Checks if the character is at full health.
     * @return True if at full health, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    bool IsFullHealth() const;

    /**
     * Adds health to the current health value.
     * @param HealAmount Amount of health to add
     * @return True if health was added, false if already at max health
     */
    UFUNCTION(BlueprintCallable, Category="Health")
    bool AddHealth(float HealAmount);

    /**
     * Event triggered when the character dies.
     */
    FOnDeath OnDeath;

    /**
     * Event triggered when health changes.
     * @param NewHealth The new health value
     */
    FOnHealthChanged OnHealthChanged;

protected:
    virtual void BeginPlay() override;

private:
    /**
     * Handles damage taken by the actor.
     * @param DamagedActor The actor that took damage
     * @param Damage Amount of damage taken
     * @param DamageType Type of damage
     * @param InstigatedBy Controller that caused the damage
     * @param DamageCauser Actor that caused the damage
     */
    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, 
        class AController* InstigatedBy, AActor* DamageCauser);

    /**
     * Updates health during auto-healing process.
     */
    void HealUpdate();

    /**
     * Sets the health value and triggers health changed event.
     * @param NewHealth New health value to set
     */
    void SetHealth(float NewHealth);

    /**
     * Validates health-related parameters.
     * @return True if parameters are valid, false otherwise
     */
    bool ValidateHealthParameters() const;

protected:
    /** Maximum health value */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", 
        meta=(ClampMin = 1.f, ClampMax = 10000.f, ToolTip="Maximum health value"))
    float MaxHealth {100.f};

    /** Whether auto-healing is enabled */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal",
        meta=(ToolTip="Enable automatic health regeneration"))
    bool AutoHeal {true};
    
    /** Time interval between heal updates */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", 
        meta=(EditCondition = "AutoHeal", ClampMin = 0.1f, ClampMax = 10.f,
        ToolTip="Time interval between each heal update"))
    float HealUpdateTime {0.3f};
    
    /** Delay before auto-healing starts after taking damage */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", 
        meta=(EditCondition = "AutoHeal", ClampMin = 0.f, ClampMax = 60.f,
        ToolTip="Delay before auto-healing starts after taking damage"))
    float HealDelay {3.f};
    
    /** Amount of health restored per heal update */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", 
        meta=(EditCondition = "AutoHeal", ClampMin = 0.1f, ClampMax = 100.f,
        ToolTip="Amount of health restored per heal update"))
    float HealModifier {1.f};

private:
    /** Timer handle for auto-healing */
    FTimerHandle HealTimerHandle;
    
    /** Current health value */
    float Health {0.f};

    /** Constants */
    static constexpr float MIN_HEALTH_THRESHOLD = 0.01f;
    static constexpr float HEALTH_PERCENTAGE_MULTIPLIER = 100.f;
};