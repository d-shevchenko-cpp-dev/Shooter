// Игра ShootThemUp. Все права защищены.


#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/STUDamageComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    DamageComponent = CreateDefaultSubobject<USTUDamageComponent>("DamageComponent");
}

void ASTUBaseWeapon::Fire()
{
    UE_LOG(LogBaseWeapon, Display, TEXT("Fire"));

    MakeShot();
}

// Вызывается при запуске игры или при появлении
void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();
    check(WeaponMesh);
}

void ASTUBaseWeapon::MakeShot()
{
    if (!IsValidForShooting())
    {
        return;
    }

    const ACharacter* Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Owner is not a Character"));
        return;
    }
    
    const APlayerController* PlayerController = Player->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("PlayerController is not valid"));
        return;
    }
    
    // Получение позиции и направления камеры
    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
    
    // Определение точек трейсинга
    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
    const FVector TraceStart = SocketTransform.GetLocation();
    const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * MaxRange;

    // Выполнение линейного трейсинга
    FHitResult HitResult;
    PerformLineTrace(TraceStart, TraceEnd, HitResult);

    // Отображение отладочной информации
    if (bDrawDebugTrace)
    {
        DrawDebugTrace(TraceStart, TraceEnd, HitResult);
    }

    // Обработка результата попадания и применение урона
    if (HitResult.bBlockingHit)
    {
        UE_LOG(LogBaseWeapon, Display, TEXT("Hit target at bone: %s"), *HitResult.BoneName.ToString());
        
        // Применение урона к пораженной цели
        if (AActor* HitActor = HitResult.GetActor())
        {
            ApplyDamageToTarget(HitActor, HitResult);
        }
    }
}

bool ASTUBaseWeapon::IsValidForShooting() const
{
    if (!GetWorld())
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("World is not valid"));
        return false;
    }

    if (!WeaponMesh)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("WeaponMesh is not valid"));
        return false;
    }

    const ACharacter* Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Owner is not a Character"));
        return false;
    }

    const APlayerController* PlayerController = Player->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("PlayerController is not valid"));
        return false;
    }

    return true;
}

void ASTUBaseWeapon::PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bReturnPhysicalMaterial = true;

    GetWorld()->LineTraceSingleByChannel(
        HitResult, 
        TraceStart, 
        TraceEnd, 
        ECollisionChannel::ECC_Visibility, 
        QueryParams
    );
}

void ASTUBaseWeapon::DrawDebugTrace(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const
{
    // Отрисовка линии трейсинга
    DrawDebugLine(
        GetWorld(), 
        TraceStart, 
        TraceEnd, 
        FColor::Red, 
        false, 
        DebugTraceDuration, 
        0, 
        3.0f
    );

    // Если попали в цель, рисуем сферу в точке попадания
    if (HitResult.bBlockingHit)
    {
        DrawDebugSphere(
            GetWorld(), 
            HitResult.ImpactPoint, 
            DebugHitSphereRadius, 
            24, 
            FColor::Red, 
            false, 
            DebugTraceDuration
        );
    }
}

void ASTUBaseWeapon::ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult)
{
    if (!Target || !DamageComponent)
    {
        return;
    }

    // Вычисление финального количества урона на основе места попадания
    float FinalDamage = DamageAmount;
    bool bIsHeadshot = false;

    // Проверка на headshot
    if (HitResult.BoneName == HeadBoneName || HitResult.BoneName == NeckBoneName)
    {
        FinalDamage *= HeadshotMultiplier;
        bIsHeadshot = true;
        UE_LOG(LogBaseWeapon, Log, TEXT("Headshot! Damage multiplied by %f"), HeadshotMultiplier);
    }

    // Использование компонента урона для нанесения урона
    DamageComponent->DealDamage(
        Target,
        FinalDamage,
        DamageType,
        HitResult.ImpactPoint,
        HitResult.BoneName,
        bIsHeadshot
    );
}