// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "IntrusiveRefCounter.h"
#include "IntrusiveRefProvider.h"

namespace blk
{
    /**
     CRTP base for intrusive AddRef/Release with a single weak provider slot.
     Derived classes can hook OnRefIncrement() without virtual dispatch.
     */
    template <typename Derived>
    class TIntrusiveRefCountable
    {
    protected:
        // Prevent copy/move construction; only assignment to reinitialize in-place.
        TIntrusiveRefCountable(const TIntrusiveRefCountable&) = delete;
        TIntrusiveRefCountable(TIntrusiveRefCountable&&) = delete;

    public:
        FORCEINLINE TIntrusiveRefCountable() = default;
        FORCEINLINE ~TIntrusiveRefCountable() { Reset(); }

        /** Increment strong reference count and call hook. */
        FORCEINLINE void AddRef()
        {
            RefCount.IncrementExchange();
            // Hook for derived classes (e.g. LRU tail move)
            static_cast<Derived*>(this)->OnRefIncrement();
        }

        /** Decrement strong reference count. Asserts on underflow. Returns new count. */
        FORCEINLINE int32 Release()
        {
            int32 Prev = RefCount.DecrementExchange();
            checkf(Prev > 0, TEXT("TIntrusiveRefCountable Double Release()"));
            return Prev - 1;
        }

        FORCEINLINE void Reset()
        {
            // Must not be shared at this point
            checkf(GetRefCount() == 0,
                TEXT("TIntrusiveRefCountable::Reset() called with non-zero refcount"));

            // Invalidate old weak provider
            ProviderSlot.Store(nullptr);

            // Clear strong count again (in case it's reused)
            RefCount.Store(0);
        }

        /** Get current strong ref count. */
        FORCEINLINE int32 GetRefCount() const { return RefCount.Load(); }

        /** Default hook called after AddRef; no-op unless overridden. */
        void OnRefIncrement() {}

    protected:
        // Single slot storing a pointer to this object for weak providers
        friend struct TIntrusiveRefProvider<Derived>;
        TAtomic<Derived*> ProviderSlot{ nullptr };

    private:
        // Atomic strong reference count
        TAtomic<int32> RefCount{ 0 };

    };
}