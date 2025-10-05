# Рефакторинг системы отладки оружия

## Обзор изменений

Была проведена полная рефакторизация системы отладки оружия для улучшения гибкости и удобства использования из блюпринтов.

## Новые компоненты

### 1. STUWeaponDebugManager
- **Файл**: `Source/ShootThemUp/Public/Weapon/STUWeaponDebugManager.h`
- **Описание**: Централизованный менеджер для управления отладочной информацией оружия
- **Возможности**:
  - Категоризация отладочной информации (Trace, Hit, Camera, Spread)
  - Гибкие настройки цветов, размеров и длительности отображения
  - Возможность включения/выключения отдельных категорий
  - Blueprint-совместимый API

### 2. ESTUWeaponDebugCategory (Enum)
- **Trace**: Линии трейсинга пуль
- **Hit**: Точки попадания
- **Camera**: Направление камеры
- **Spread**: Разброс пуль
- **All**: Все категории

### 3. FSTUWeaponDebugSettings (Struct)
- Настройки отладки с возможностью редактирования в редакторе
- Поддержка условного отображения параметров
- Настройка цветов, размеров и длительности

## Обновления существующих классов

### STUBaseWeapon
- Добавлен компонент `DebugManager`
- Новые методы для управления отладкой:
  - `SetDebugEnabledForCategory()`
  - `SetDebugEnabled()`
  - `UpdateDebugSettings()`
  - `GetDebugManager()`
- Рефакторинг метода `MakeShot()` для использования нового менеджера

## Преимущества нового подхода

1. **Модульность**: Отладочная логика вынесена в отдельный класс
2. **Гибкость**: Возможность настройки отдельных категорий отладки
3. **Blueprint-совместимость**: Полный доступ к функциональности из блюпринтов
4. **Производительность**: Оптимизированная отрисовка с проверками
5. **Расширяемость**: Легко добавлять новые категории отладки

## Использование в блюпринтах

```cpp
// Включить отладку трейсинга
Weapon->SetDebugEnabledForCategory(ESTUWeaponDebugCategory::Trace, true);

// Выключить отладку попаданий
Weapon->SetDebugEnabledForCategory(ESTUWeaponDebugCategory::Hit, false);

// Включить всю отладку
Weapon->SetDebugEnabled(true);

// Обновить настройки
FSTUWeaponDebugSettings Settings;
Settings.bEnableDebug = true;
Settings.DebugDuration = 5.0f;
Settings.TraceLineColor = FColor::Green;
Weapon->UpdateDebugSettings(Settings);
```

## Обратная совместимость

Старая структура `FSTUWeaponDebugData` помечена как устаревшая, но сохранена для обратной совместимости. Рекомендуется использовать новый `STUWeaponDebugManager`.

## Файлы изменений

- `Source/ShootThemUp/Public/Weapon/STUWeaponDebugManager.h` (новый)
- `Source/ShootThemUp/Private/Weapon/STUWeaponDebugManager.cpp` (новый)
- `Source/ShootThemUp/Public/Weapon/STUBaseWeapon.h` (обновлен)
- `Source/ShootThemUp/Private/Weapon/STUBaseWeapon.cpp` (обновлен)
- `Source/ShootThemUp/Examples/WeaponDebugUsageExample.txt` (новый)
