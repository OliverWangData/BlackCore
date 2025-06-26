// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "Stack.h"

namespace blk
{
	template <typename TIndex = int32>
	class TIndexPool
	{
	public:
		TIndexPool()
		{
			Clear();
		}

		TIndex Acquire()
		{
			if (Indices.IsEmpty()) return Next++;
			return Indices.Pop();
		}

		void Release(TIndex Index)
		{
			Indices.Push(Index);
		}

		// Reset the pool to start fresh (optionally with a given max index)
		void Clear(TIndex Start = 0)
		{
			Next = Start;
			Indices.Clear();
		}

	private:
		TIndex Next;
		TStack<TIndex> Indices;
	};
}