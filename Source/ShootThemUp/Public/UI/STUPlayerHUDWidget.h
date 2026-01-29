#pragma once

#include "CoreMinimal.h"
#include "STUWeaponComponent.h"
#include "STUHealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "Weapon/STUWeaponTypes.h"
#include "STUPlayerHUDWidget.generated.h"

UCLASS()
class SHOOTTHEMUP_API USTUPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Ui")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Ui")
    bool GetWeaponUIData(FWeaponUIData& UIData) const;

    UFUNCTION(BlueprintCallable, Category = "Ui")
    FString GetCurrentAmmo() const;

    UFUNCTION(BlueprintCallable, Category = "Ui")
    bool IsPlayerAlive() const;

    UFUNCTION(BlueprintCallable, Category = "Ui")
    bool IsSpectatingMode() const;

    UFUNCTION(BlueprintImplementableEvent, Category = "Ui")
    void OnTakeDamage();

    virtual bool Initialize() override;

private:
    USTUWeaponComponent* GetWeaponComponent() const;

    USTUHealthComponent* GetHeathComponent() const;

    void OnHealthChanged(float Health, float HealthDelta);
};
