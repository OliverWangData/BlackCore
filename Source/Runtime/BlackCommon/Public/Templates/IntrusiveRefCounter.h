// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

namespace blk
{
	struct ENoAddRef { explicit ENoAddRef() = default; };
	inline constexpr ENoAddRef NoAddRef{};

	// Non owning handle that increments and decrements a ref count on the T object. 
	// Object must not be move, nor destroyed when the ref count is not 0, otherwise this 
	// becomes a dangling pointer. 
	template <typename T>
	requires requires(T* ptr) {
		ptr->AddRef();
		ptr->Release();
	}
	struct TIntrusiveRefCounter
	{
	public:
		FORCEINLINE TIntrusiveRefCounter() = default;

		FORCEINLINE TIntrusiveRefCounter(T* InPtr) : Ptr(InPtr)
		{
			if (Ptr) Ptr->AddRef();
		}

		FORCEINLINE TIntrusiveRefCounter(T* InPtr, ENoAddRef)
			: Ptr(InPtr)
		{
			// no AddRef
		}

		FORCEINLINE TIntrusiveRefCounter(const TIntrusiveRefCounter& Other)
			: Ptr(Other.Ptr)
		{
			if (Ptr) Ptr->AddRef();
		}

		FORCEINLINE TIntrusiveRefCounter& operator=(const TIntrusiveRefCounter& Other)
		{
			if (this != &Other)
			{
				if (Ptr) Ptr->Release();
				if (Other.Ptr) Other.Ptr->AddRef();
				Ptr = Other.Ptr;
			}
			return *this;
		}

		FORCEINLINE TIntrusiveRefCounter(TIntrusiveRefCounter&& Other) noexcept
			: Ptr(Other.Ptr)
		{
			Other.Ptr = nullptr;
		}

		FORCEINLINE TIntrusiveRefCounter& operator=(TIntrusiveRefCounter&& Other) noexcept
		{
			if (this != &Other)
			{
				if (Ptr) Ptr->Release();
				Ptr = Other.Ptr;
				Other.Ptr = nullptr;
			}
			return *this;
		}

		FORCEINLINE ~TIntrusiveRefCounter()
		{
			if (Ptr) Ptr->Release();
		}


		FORCEINLINE T* Get() const { return Ptr; }
		FORCEINLINE T& operator*() const { return *Ptr; }
		FORCEINLINE T* operator->() const { return Ptr; }
		FORCEINLINE explicit operator bool() const { return Ptr != nullptr; }

	private:
		T* Ptr = nullptr;
	};
}