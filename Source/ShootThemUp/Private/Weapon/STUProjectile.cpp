#include "Weapon/STUProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogProjectile, All, All);

ASTUProjectile::ASTUProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->InitialSpeed = 2000.0f;
    MovementComponent->MaxSpeed = 2000.0f;
    MovementComponent->ProjectileGravityScale = 1.0f;
}

void ASTUProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (!MovementComponent)
    {
        UE_LOG(LogProjectile, Error, TEXT("MovementComponent is not valid"));
        return;
    }

    if (!CollisionComponent)
    {
        UE_LOG(LogProjectile, Error, TEXT("CollisionComponent is not valid"));
        return;
    }

    // Настройка движения снаряда
    MovementComponent->Velocity = ShootDirection * MovementComponent->InitialSpeed;
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);

    // Подписка на событие столкновения
    CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);

    // Установка времени жизни
    SetLifeSpan(LifeTime);

    UE_LOG(LogProjectile, Log, TEXT("Projectile initialized successfully"));
}

void ASTUProjectile::SetShootDirection(const FVector& Direction)
{
    ShootDirection = Direction.GetSafeNormal();
}

AController* ASTUProjectile::GetOwnerController() const
{
    const APawn* OwnerPawn = Cast<APawn>(GetOwner());
    return OwnerPawn ? OwnerPawn->GetController() : nullptr;
}

void ASTUProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!GetWorld())
    {
        UE_LOG(LogProjectile, Warning, TEXT("World is not valid"));
        return;
    }

    UE_LOG(LogProjectile, Log, TEXT("Projectile hit: %s"), OtherActor ? *OtherActor->GetName() : TEXT("Unknown"));

    // Останавливаем движение снаряда
    MovementComponent->StopMovementImmediately();

    // Применяем урон в радиусе взрыва
    ApplyExplosionDamage(GetActorLocation());

    // Отрисовываем отладочную информацию
    DrawExplosionDebug(GetActorLocation());

    Destroy();
}

void ASTUProjectile::ApplyExplosionDamage(const FVector& ExplosionLocation)
{
    if (!GetWorld())
    {
        return;
    }

    UGameplayStatics::ApplyRadialDamage(GetWorld(),
        ExplosionDamage,
        ExplosionLocation,
        ExplosionRadius,
        UDamageType::StaticClass(),
        TArray<AActor*>(), // Игнорируемые акторы
        this,
        GetOwnerController(),
        bApplyFullDamageToAll);

    UE_LOG(LogProjectile, Log, TEXT("Applied explosion damage: %f at radius: %f"), ExplosionDamage, ExplosionRadius);
}

void ASTUProjectile::DrawExplosionDebug(const FVector& ExplosionLocation) const
{
    if (!GetWorld())
    {
        return;
    }

    // Отрисовываем сферу взрыва
    DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 24, FColor::Red, false, 5.0f);
}
