#pragma once

struct STUUtils
{
    template <class T>
    static T* GetSTUPlayerComponent(APawn* PlayerPawn)
    {
        if (!PlayerPawn)
            return nullptr;

        const auto Component = PlayerPawn->GetComponentByClass(T::StaticClass());
        return Cast<T>(Component);
    }
};
