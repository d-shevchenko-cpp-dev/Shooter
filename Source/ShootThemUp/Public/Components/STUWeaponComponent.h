#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/STUWeaponTypes.h"
#include "STUBaseWeapon.h"
#include "STUWeaponComponent.generated.h"

USTRUCT(BlueprintType)
struct FWeaponReloadAnimMontageData
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ASTUBaseWeapon> WeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* ReloadAnimMontage;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUWeaponComponent();

    void StartShooting();

    void StopShooting();

    void SwitchWeapon();

    void Reload();

    ASTUBaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

    bool GetWeaponUIData(FWeaponUIData& UIData) const;

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SpawnWeapons();

    void AttachWeaponsToActor();

    void AttachWeaponToSocket(USkeletalMeshComponent* Mesh, ASTUBaseWeapon* Weapon, const FName& SocketName);

    void EquipWeapon(int32 WeaponIndex);

    void PlayAnimMontage(UAnimMontage* AnimMontage);

    void InitAnimations();

    void OnWeaponEquipFinished(USkeletalMeshComponent* MeshComponent);

    void OnReloadFinished(USkeletalMeshComponent* MeshComponent);

    ACharacter* GetCharacter() const;

    USkeletalMeshComponent* GetCharacterMesh() const;

    UAnimMontage* GetReloadAnimMontage(ASTUBaseWeapon* Weapon) const;

    bool IsWeaponReadyForAction() const;

    bool IsAnimationInProcess() const;

    template <class T>
    T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (!Animation)
        {
            return nullptr;
        }
        const auto NotifyEffects = Animation->Notifies;
        for (auto NotifyEffect : NotifyEffects)
        {
            if (auto AnimNotify = Cast<T>(NotifyEffect.Notify))
            {
                return AnimNotify;
            }
        }
        return nullptr;
    }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<FWeaponReloadAnimMontageData> WeaponData;

    /** Socket в руке*/
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponSocket";

    /** Socket на спине */
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "ArmorySocket";

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* EquipAnimMontage;

private:
    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon;

    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    int32 CurrentWeaponIndex{ 0 };

    bool bAnimInProcess{ false };
};
