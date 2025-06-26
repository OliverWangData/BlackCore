// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "IndexPool.h"
#include <type_traits>

namespace blk
{
	template <typename TIndices = FIntPoint>
		requires requires(TIndices t) {
			requires std::integral<std::remove_reference_t<decltype(t.X)>>;
			requires std::integral<std::remove_reference_t<decltype(t.Y)>>;
			{ ++t.X } -> std::convertible_to<decltype(t.X)>;
			{ ++t.Y } -> std::convertible_to<decltype(t.Y)>;
	}
	class TIndexPool2D
	{
		using TIndex = decltype(std::declval<TIndices>().X);

	public:
		TIndexPool2D()
		{
			Clear();
		}

		void SetWidth(TIndex InWidth)
		{
			Width = InWidth;
			Clear();
		}

		TIndices Acquire()
		{
			// Make sure the width has been initialized
			check(Width != 0);
			
			if (Indices.IsEmpty())
			{
				TIndices out = Next;
				IncrementNext();
				return out;
			}
			return Indices.Pop();
		}

		void Release(TIndices Index)
		{
			Indices.Push(Index);
		}

		// Reset the pool to start fresh (optionally with a given max index)
		void Clear(TIndices Start = FIntPoint())
		{
			Next = Start;
			Indices.Clear();
		}

	private:
		void IncrementNext()
		{
			++Next.X;

			if (Next.X == Width)
			{
				Next.X = 0;
				++Next.Y;
			}

		}

		TIndex Width{ 0 };
		TIndices Next;
		TStack<TIndices> Indices;
	};
}