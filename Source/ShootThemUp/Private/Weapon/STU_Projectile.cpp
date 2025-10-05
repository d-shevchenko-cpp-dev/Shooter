// ShootThemUp Game. All Right Reserved.

#include "Weapon/STU_Projectile.h"
#include "Components/SphereComponent.h"

ASTU_Projectile::ASTU_Projectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereComponent"));
    CollisionComponent->InitSphereRadius(5.0f);
    SetRootComponent(CollisionComponent);
}

void ASTU_Projectile::BeginPlay()
{
    Super::BeginPlay();
}
