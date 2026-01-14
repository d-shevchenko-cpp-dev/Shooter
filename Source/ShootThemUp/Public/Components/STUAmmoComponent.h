#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUAmmoComponent.generated.h"

USTRUCT(BlueprintType)
struct FAmmoData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
    int32 BulletsInClip = 30;

    /** Количество обойм */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo", meta = (EditCondition = "!InfiniteCheat"))
    int32 Bullets = 300;

    /** Бесконечные патроны */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
    bool InfiniteCheat = false;
};

DECLARE_MULTICAST_DELEGATE(FOnClipEmptySignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUAmmoComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUAmmoComponent();

    UFUNCTION(BlueprintCallable, Category = "Ammo")
    void Initialize(const FAmmoData& InitialAmmo);

    UFUNCTION(BlueprintCallable, Category = "Ammo")
    bool TryDecreaseAmmo();

    UFUNCTION(BlueprintCallable, Category = "Ammo")
    bool TryReload();

    UFUNCTION(BlueprintPure, Category = "Ammo")
    bool IsClipEmpty() const;

    UFUNCTION(BlueprintPure, Category = "Ammo")
    bool IsAmmoEmpty() const;

    UFUNCTION(BlueprintPure, Category = "Ammo")
    bool CanReload() const;

    UFUNCTION(BlueprintPure, Category = "Ammo")
    FAmmoData GetCurrentAmmo() const { return CurrentAmmo; }

    UFUNCTION(BlueprintCallable, Category = "Ammo")
    void LogAmmo() const;

    FOnClipEmptySignature OnClipEmpty;

private:
    UPROPERTY()
    FAmmoData CurrentAmmo;

    UPROPERTY()
    FAmmoData DefaultAmmo;
};
