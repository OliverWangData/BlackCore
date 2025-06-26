// Copyright (c) Black Megacorp. All Rights Reserved.

#include "Textures/LRUTextureAtlas.h"
#include "Math/ArrayIndexing.h"

void FLRUTextureAtlasIndex::Init(
	ULRUTextureAtlas* atlas,
	int32 InNodeIndex
)
{
	Freed = true;
	Atlas = atlas;
	NodeIndex = InNodeIndex;
}

void FLRUTextureAtlasIndex::Reinit(FIntPoint InValue)
{
	check(Freed);
	Freed = false;
	ProviderSlot.Store(this);
	Value = InValue;
}

void FLRUTextureAtlasIndex::Free()
{
	check(!Freed);
	check(!this->IsInList());
	Freed = true;
	blk::TIntrusiveRefCountable<FLRUTextureAtlasIndex>::Reset();
	ProviderSlot.Store(nullptr);
}

// Moves the node back to tail on new access
void FLRUTextureAtlasIndex::OnRefIncrement() 
{ 
	if (Atlas) Atlas->MoveToTail(this); 
}

void ULRUTextureAtlas::Initialize(
	int32 InAtlasWidth, int32 InAtlasHeight,
	int32 InTileWidth, int32 InTileHeight,
	int32 InTilePadding, EPixelFormat InFormat
)
{
	Super::Initialize(
		InAtlasWidth, InAtlasHeight,
		InTileWidth, InTileHeight,
		InTilePadding, InFormat
	);

	TileIndexPool.SetWidth(InTileWidth);
}

TArray<ULRUTextureAtlas::IndexProvider> ULRUTextureAtlas::GetUnusedTiles(int32 Count)
{
	TArray<IndexProvider> OutProviders;
	OutProviders.Reserve(Count);

	// No more new tiles in the atlas, so we need to evict unused tiles
	if (TileCount + Count >= GetMaxTileCount()) Evict(Count);

	for (int i = 0; i < Count; ++i)
	{
		int32 nodeIndex = NodeIndexPool.Acquire();

		// Default constructs new unconstructed Index if needed, and initializes
		if (Nodes.Num() == nodeIndex) 
		{
			Nodes.Add(1);
			Nodes[nodeIndex].Init(this, nodeIndex);
		}

		// Sets new value for a freed Index
		Nodes[nodeIndex].Reinit(TileIndexPool.Acquire());

		// Gets a pointer to the new Index in the ChunkedArray
		Index* Ptr = &Nodes[nodeIndex];

		// Adds it to the tail (most recently used) of the lru cache
		LRU.AddTail(Ptr);

		// Adds a provider to the output TArray
		OutProviders.Emplace(Ptr);

		++TileCount;
	}

	return OutProviders;
}

void ULRUTextureAtlas::WriteTiles(
	TArray<IndexCounter>& TileIndices,
	TArray<uint8>& PixelData
)
{
	TArray<FIntPoint> DestIndices;
	DestIndices.Reserve(TileIndices.Num());

	for (int i = 0; i < TileIndices.Num(); ++i)
	{
		check(TileIndices[i]);
		DestIndices.Add(*TileIndices[i]);
	}

	WriteTiles(DestIndices, PixelData);
}

TArray<ULRUTextureAtlas::IndexProvider> ULRUTextureAtlas::WriteTiles(
	TArray<uint8>& PixelData,
	int32 Count
)
{
	// Reserves n coordinates
	TArray<IndexProvider> Providers = GetUnusedTiles(Count);

	TArray<FIntPoint> DestIndices;
	DestIndices.Reserve(Count);

	// Creates a shared pointer to the coordinate, and adds that to the dstCoords
	for (int i = 0; i < Providers.Num(); ++i)
	{
		check(Providers[i].IsValid());
		auto ptr = Providers[i].Acquire();
		check(ptr);
		DestIndices.Add(*ptr);
	}

	WriteTiles(DestIndices, PixelData);
	return Providers;
}

void ULRUTextureAtlas::Evict(int32 Count)
{
	FScopeLock Lock(&LRUMutex);
	int32 EvictedCount = 0;

	// Head is the least recently used item. Items move to tail upon use. 
	for (auto it = LRU.begin(); it != LRU.end();)
	{
		// Store reference to current iterator
		Index& Index = *it;

		// Pre-increment the iterator before doing anything destructive
		++it;

		// Finds unused nodes
		if (Index.GetRefCount() == 0)
		{
			// Notifies anyone that a coordinate will be released
			OnEvict.Broadcast(Index);

			// Releases the index
			LRU.Remove(&Index);
			Index.Free();
			NodeIndexPool.Release(Index.GetNodeIndex());
			TileIndexPool.Release(Index);

			// Only needs 'count' evictions. 
			++EvictedCount;
			if (EvictedCount == Count) return;
		}
	}

	// Not possible to evict more. Continuing will result in corrupted textures, so we hard crash.
	UE_LOG(
		LogTemp,
		Fatal,
		TEXT("ULRUTextureAtlas::Evict failed: only %zu of %zu entries evicted. "),
		EvictedCount,
		Count);
}


void ULRUTextureAtlas::MoveToTail(Index* node)
{
	FScopeLock Lock(&LRUMutex);
	node->Remove();
	LRU.AddTail(node);
}