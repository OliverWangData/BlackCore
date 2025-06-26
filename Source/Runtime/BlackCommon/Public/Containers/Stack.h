// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

namespace blk
{
	template<typename T>
	class TStack
	{
	public:
		// Push an element onto the stack
		template <typename U>
		void Push(U&& Item)
		{
			Items.Add(Forward<U>(Item));
		}

		// Pop the top element (unsafe: caller must check IsEmpty first)
		T Pop()
		{
			check(!IsEmpty()); // Use check or ensure if you want runtime safety
			return Items.Pop(); // Returns by value, move or copy depending on T
		}

		// Peek at the top element (non-const)
		T& Top()
		{
			check(!IsEmpty());
			return Items.Last();
		}

		// Peek at the top element (const)
		const T& Top() const
		{
			check(!IsEmpty());
			return Items.Last();
		}

		// Check if the stack is empty
		bool IsEmpty() const
		{
			return Items.Num() == 0;
		}

		// Get the number of elements in the stack
		int32 Num() const
		{
			return Items.Num();
		}

		// Clear the stack
		void Clear()
		{
			Items.Reset();
		}

	private:
		TArray<T> Items;
	};
}