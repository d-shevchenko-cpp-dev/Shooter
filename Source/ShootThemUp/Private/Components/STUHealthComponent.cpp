#include "Components/STUHealthComponent.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

USTUHealthComponent::USTUHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Проверка параметров в конструкторе
    if (!ValidateHealthParameters())
    {
        UE_LOG(LogHealthComponent, Warning, TEXT("Invalid health parameters detected in constructor"));
    }
}

float USTUHealthComponent::GetHealth() const
{
    return Health;
}

float USTUHealthComponent::GetHealthPercent() const
{
    return MaxHealth > 0.f ? Health / MaxHealth : 0.f;
}

bool USTUHealthComponent::IsDead() const
{
    return Health <= MIN_HEALTH_THRESHOLD;
}

bool USTUHealthComponent::IsFullHealth() const
{
    return FMath::IsNearlyEqual(Health, MaxHealth, MIN_HEALTH_THRESHOLD);
}

bool USTUHealthComponent::AddHealth(float HealAmount)
{
    if (HealAmount <= 0.f || IsFullHealth())
    {
        return false;
    }

    SetHealth(Health + HealAmount);
    return true;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // Инициализация здоровья до максимального значения
    SetHealth(MaxHealth);

    // Привязка события урона, если владелец существует
    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamageHandle);
        UE_LOG(LogHealthComponent, Log, TEXT("Health component initialized for actor: %s"), *Owner->GetName());
    }
    else
    {
        UE_LOG(LogHealthComponent, Error, TEXT("Health component has no owner!"));
    }
}

void USTUHealthComponent::OnTakeAnyDamageHandle(AActor* DamagedActor,
    float Damage,
    const class UDamageType* DamageType,
    class AController* InstigatedBy,
    AActor* DamageCauser)
{
    // Условия раннего выхода
    if (Damage <= 0.0f || IsDead() || !GetWorld())
    {
        return;
    }

    UE_LOG(LogHealthComponent,
        Log,
        TEXT("Actor %s took %f damage from %s"),
        DamagedActor ? *DamagedActor->GetName() : TEXT("Unknown"),
        Damage,
        DamageCauser ? *DamageCauser->GetName() : TEXT("Unknown"));

    // Применение урона
    SetHealth(Health - Damage);

    // Очистка существующего таймера лечения
    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

    // Обработка смерти или запуск автоматического лечения
    if (IsDead())
    {
        UE_LOG(LogHealthComponent,
            Warning,
            TEXT("Actor %s has died"),
            DamagedActor ? *DamagedActor->GetName() : TEXT("Unknown"));
        OnDeath.Broadcast();
    }
    else if (AutoHeal)
    {
        // Запуск таймера автоматического лечения
        GetWorld()->GetTimerManager().SetTimer(
            HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);

        UE_LOG(LogHealthComponent,
            Log,
            TEXT("Auto-healing started for actor %s"),
            DamagedActor ? *DamagedActor->GetName() : TEXT("Unknown"));
    }
}

void USTUHealthComponent::HealUpdate()
{
    // Добавление количества лечения
    SetHealth(Health + HealModifier);

    // Остановка лечения, если на полном здоровье
    if (IsFullHealth() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
        UE_LOG(LogHealthComponent,
            Log,
            TEXT("Auto-healing completed for actor %s"),
            GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    const float OldHealth = Health;
    Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

    // Трансляция только если здоровье действительно изменилось
    if (!FMath::IsNearlyEqual(OldHealth, Health, MIN_HEALTH_THRESHOLD))
    {
        OnHealthChanged.Broadcast(Health);
        UE_LOG(LogHealthComponent, VeryVerbose, TEXT("Health changed from %f to %f"), OldHealth, Health);
    }
}

bool USTUHealthComponent::ValidateHealthParameters() const
{
    bool bIsValid = true;

    if (MaxHealth <= 0.f)
    {
        UE_LOG(LogHealthComponent, Error, TEXT("MaxHealth must be greater than 0"));
        bIsValid = false;
    }

    if (AutoHeal)
    {
        if (HealUpdateTime <= 0.f)
        {
            UE_LOG(LogHealthComponent, Error, TEXT("HealUpdateTime must be greater than 0 when AutoHeal is enabled"));
            bIsValid = false;
        }

        if (HealModifier <= 0.f)
        {
            UE_LOG(LogHealthComponent, Error, TEXT("HealModifier must be greater than 0 when AutoHeal is enabled"));
            bIsValid = false;
        }

        if (HealDelay < 0.f)
        {
            UE_LOG(LogHealthComponent, Error, TEXT("HealDelay cannot be negative"));
            bIsValid = false;
        }
    }

    return bIsValid;
}
