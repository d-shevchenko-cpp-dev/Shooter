# Система урона ShootThemUp

## Обзор

Система урона в проекте ShootThemUp состоит из нескольких компонентов, которые работают вместе для обработки получения и нанесения урона:

## Основные компоненты

### 1. STUHealthComponent
Компонент здоровья, который управляет жизнями персонажа и обрабатывает получение урона.

**Основные функции:**
- Управление здоровьем персонажа
- Автоматическое восстановление здоровья
- Обработка смерти персонажа
- События изменения здоровья и смерти

**Использование:**
```cpp
// Получение текущего здоровья
float CurrentHealth = HealthComponent->GetHealth();

// Проверка на смерть
bool bIsDead = HealthComponent->IsDead();

// Добавление здоровья
bool bHealed = HealthComponent->AddHealth(50.0f);
```

### 2. STUDamageComponent
Компонент для нанесения урона другим акторам.

**Основные функции:**
- Нанесение урона целям
- Поддержка различных типов урона
- События нанесения урона
- Поддержка headshot'ов

**Использование:**
```cpp
// Простое нанесение урона
DamageComponent->DealDamage(TargetActor, 25.0f, DamageTypeClass);

// Нанесение урона с информацией о попадании
DamageComponent->DealDamage(TargetActor, 25.0f, DamageTypeClass,
                           HitLocation, BoneName, bIsHeadshot);
```

### 3. STUBaseWeapon
Базовый класс оружия с интегрированной системой урона.

**Основные функции:**
- Стрельба с трейсингом
- Нанесение урона при попадании
- Поддержка headshot'ов
- Настройка урона и множителей

**Настройки в редакторе:**
- `DamageAmount` - базовый урон
- `HeadshotMultiplier` - множитель для headshot'ов
- `HeadBoneName` - имя кости головы
- `NeckBoneName` - имя кости шеи

### 4. STUDamageEvent
Структура для передачи информации о событии урона.

**Содержит:**
- Целевой актор
- Источник урона
- Количество урона
- Информация о headshot'е
- Место попадания
- Имя кости

## Типы урона

### STUWeaponDamageType
Базовый тип урона для оружия.

### STUFireDamageType
Тип урона от огня.

### STUIceDamageType
Тип урона от льда.

## События

### События здоровья
- `OnDeath` - персонаж умер
- `OnHealthChanged(float)` - здоровье изменилось
- `OnDamageReceived(FSTUDamageEvent)` - получен урон (Blueprint)

### События урона
- `OnDamageDealt(FSTUDamageEvent)` - нанесен урон (Blueprint)

## Примеры использования

### В Blueprint
1. Подпишитесь на событие `OnDamageReceived` в компоненте здоровья
2. Обработайте событие для показа эффектов урона
3. Используйте `OnDamageDealt` для отслеживания нанесенного урона

### В C++
```cpp
// Подписка на события
HealthComponent->OnDeath.AddDynamic(this, &AMyCharacter::OnCharacterDeath);
HealthComponent->OnHealthChanged.AddDynamic(this, &AMyCharacter::OnHealthChanged);

// Нанесение урона
if (DamageComponent)
{
    DamageComponent->DealDamage(Target, 30.0f, USTUWeaponDamageType::StaticClass());
}
```

## Настройка

1. Добавьте `STUHealthComponent` к вашему персонажу
2. Добавьте `STUDamageComponent` к оружию или другим источникам урона
3. Настройте параметры урона в редакторе
4. Подпишитесь на нужные события для обработки урона

## Расширение системы

Для добавления новых типов урона:
1. Создайте новый класс, наследующий от `UDamageType`
2. Настройте свойства урона в конструкторе
3. Используйте новый тип в компонентах урона
