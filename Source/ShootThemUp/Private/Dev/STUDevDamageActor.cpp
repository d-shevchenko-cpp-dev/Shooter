// Игра ShootThemUp. Все права защищены.


#include "Dev/STUDevDamageActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Dev/STUFireDamageType.h"
#include "Dev/STUIceDamageType.h"

// Установка значений по умолчанию
ASTUDevDamageActor::ASTUDevDamageActor()
{
    // Установка этого актора для вызова Tick() каждый кадр. Вы можете отключить это для улучшения производительности, если вам это не нужно.
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    SetRootComponent(SceneComponent);
}

// Вызывается при запуске игры или при появлении
void ASTUDevDamageActor::BeginPlay()
{
    Super::BeginPlay();
}

// Вызывается каждый кадр
void ASTUDevDamageActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 24, SphereColor);
    UGameplayStatics::ApplyRadialDamage(GetWorld(),
        Damage,
        GetActorLocation(),
        Radius,
        DamageType,
        {},
        this,
        nullptr,
        DoFullDamage);
}