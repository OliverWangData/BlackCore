// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "IntrusiveRefCounter.h"

namespace blk
{/**
     * Intrusive ref counter provider (weak handle). Templates on derived type.
     * Allows reviving a strong reference if the object is still alive in the container.
     * If a TIntrusiveRefProvider exists, the original object must outlive it, or it
     * must be destroyed when the original object is freed.
     */
    template <typename T>
    struct TIntrusiveRefProvider
    {
        FORCEINLINE TIntrusiveRefProvider() = default;
        FORCEINLINE explicit TIntrusiveRefProvider(T* Obj)
            : Slot(Obj ? &Obj->ProviderSlot : nullptr)
        {
        }

        /** Acquire a strong ref if object still valid. */
        FORCEINLINE TIntrusiveRefCounter<T> Acquire() const
        {
            if (!Slot)
            {
                return nullptr;
            }

            // 1) Load the stored pointer
            T* Ptr = Slot->Load();
            if (!Ptr)
            {
                return nullptr;
            }

            // 2) Bump strong count (calls OnRefIncrement hook)
            Ptr->AddRef();

            // 3) Re-check slot to avoid race with destruction
            if (Slot->Load() != Ptr)
            {
                Ptr->Release();
                return nullptr;
            }

            return TIntrusiveRefCounter<T>(Ptr, ENoAddRef{});
        }

        /** True if the object slot is non-null (may still have been destroyed). */
        FORCEINLINE bool IsValid() const
        {
            return Slot && Slot->Load() != nullptr;
        }

    private:
        // Pointer to the object's ProviderSlot
        TAtomic<T*>* Slot = nullptr;
    };
}