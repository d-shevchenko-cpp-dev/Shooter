#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUDamageComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUDamageComponent();

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
    bool ApplyDamageInternal(AActor* Target,
        float DamageAmount,
        TSubclassOf<UDamageType> DamageType,
        const FVector& HitLocation,
        const FName& HitBoneName,
        bool bIsHeadshot);

    AController* GetInstigatorController() const;
};
