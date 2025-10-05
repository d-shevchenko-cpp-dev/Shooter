// ShootThemUp Game. All Right Reserved.

#include "Weapon/STUProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASTUProjectile::ASTUProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereComponent"));
    CollisionComponent->InitSphereRadius(5.0f);
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovementComponent"));
    MovementComponent->InitialSpeed = 2000.0f;
}

void ASTUProjectile::BeginPlay()
{
    Super::BeginPlay();

    checkf(MovementComponent, TEXT("ProjectileMovementComponent component is NULL"));

    MovementComponent->Velocity = ShootDirection * MovementComponent->InitialSpeed;

    SetLifeSpan(5.f);
}

void ASTUProjectile::SetShootDirection(const FVector& Direction)
{
    ShootDirection = Direction;
}
