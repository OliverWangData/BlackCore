// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "Stack.h"

namespace blk
{
	template <typename T>
	class TObjectPool
	{
	public:
		// Default constructor func
		TObjectPool(size_t Count = 0) requires std::is_default_constructible_v<T>
			: TObjectPool(Count, []() { return T{}; }) {}

		// Specify factory only
		template <typename UFactory>
		TObjectPool(UFactory&& factory) : TObjectPool(0, Forward<UFactory>(factory)) {
		}
		// Full constructor
		template <typename UFactory>
		TObjectPool(size_t Count, UFactory&& factory) : Factory(Forward<UFactory>(factory))
		{
			for (size_t i = 0; i < Count; ++i)
			{
				Objects.Push(this->Factory());
			}
		}

		T Acquire()
		{
			if (Objects.Num() == 0) return Factory();
			return Objects.Pop();
		}

		template <typename U>
		void Release(U&& Item)
		{
			Objects.Push(Forward<U>(Item));
		}

	private:
		TFunction<T()> Factory;
		TStack<T> Objects;
	};
}