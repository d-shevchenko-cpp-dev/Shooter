// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STU_Projectile.generated.h"

class USphereComponent;

UCLASS()
class SHOOTTHEMUP_API ASTU_Projectile : public AActor
{
    GENERATED_BODY()

public:
    ASTU_Projectile();

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    USphereComponent* CollisionComponent;
};
