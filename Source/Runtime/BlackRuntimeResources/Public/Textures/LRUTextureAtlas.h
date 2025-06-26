// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "Templates/IntrusiveRefCounter.h"
#include "Templates/IntrusiveRefProvider.h"
#include "Templates/IntrusiveRefCountable.h"
#include "Containers/IntrusiveDoubleLinkedList.h"
#include "Containers/IndexPool.h"
#include "Containers/IndexPool2D.h"
#include "TextureAtlasBase.h"
#include "LRUTextureAtlas.generated.h"

// A struct that represents an FIntPoint but also supports the following:
// IntrusiveRefCounters
//	- Allows ref counting via the RefCounters, while not destroying the FIndex on 0 ref
//	- Allows users to hold a Provider to create more refcounter pointers and keep index alive
// 
// DoubleLinkedList
//	- Allows the FIndexs to be stored in a chunked array (pointer stability)
//	- Allows iterating via lru, by moving newly used FIndex to the tail with thread safety
struct BLACKRUNTIMERESOURCES_API FLRUTextureAtlasIndex :
	public TIntrusiveDoubleLinkedList<FLRUTextureAtlasIndex>::NodeType,
	public blk::TIntrusiveRefCountable<FLRUTextureAtlasIndex>
{
public:
	// Default constructor for TChunkedArray
	FLRUTextureAtlasIndex() = default;

	// Should be called after default constructor
	void Init(ULRUTextureAtlas* atlas, int32 InNodeIndex);

	// Reinitializes a freed Index
	void Reinit(FIntPoint InValue);

	// Frees the index
	void Free();

	// Moves the node back to tail on new access
	void OnRefIncrement();

	// Implicitly uses this class as FIntPoint
	FORCEINLINE operator FIntPoint() const { return Value; }

	FORCEINLINE int32 GetNodeIndex() const { return NodeIndex; }

private:
	FIntPoint Value; // Index of the tile on the atlas
	int32 NodeIndex; // Index inside the TChunkedArray
	ULRUTextureAtlas* Atlas; // Used to move node to tail
	bool Freed; // Mostly to make sure indices are being freed properly
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEvict, FIntPoint, Index);


UCLASS(BlueprintType)
class BLACKRUNTIMERESOURCES_API ULRUTextureAtlas : public UTextureAtlasBase
{
	GENERATED_BODY()

public:

	// Aliases
	using Index = FLRUTextureAtlasIndex;
	using IndexProvider = blk::TIntrusiveRefProvider<Index>;
	using IndexCounter = blk::TIntrusiveRefCounter<Index>;

	// --- Setup ---
	UFUNCTION(
		BlueprintCallable,
		Category = "TextureAtlas",
		meta = (DisplayName = "Initialize Atlas"))
	virtual void Initialize(
		int32 InAtlasWidth, int32 InAtlasHeight,
		int32 InTileWidth, int32 InTileHeight,
		int32 InTilePadding, EPixelFormat InFormat
	) override;


	// --- Tile management ---
	TArray<IndexProvider> GetUnusedTiles(int32 Count);

	void WriteTiles(
		TArray<IndexCounter>& TileIndices,
		TArray<uint8>& PixelData
	);

	TArray<IndexProvider> WriteTiles(
		TArray<uint8>& PixelData,
		int32 Count
	);

	FOnEvict OnEvict;

protected:
	// Brings the parent's WriteTiles up so we can create "WriteTiles" with a different signature
	using UTextureAtlasBase::WriteTiles;

	// Evicts the least recently used non ref'd values by iterating backwards through the list
	void Evict(int32 Count);

	friend struct FLRUTextureAtlasIndex;
	void MoveToTail(Index* node);

	blk::TIndexPool2D<FIntPoint> TileIndexPool; // Unused atlas tile indices
	blk::TIndexPool<int32> NodeIndexPool; // Free'd TChunkArray node indices

	int32 TileCount = 0;
	TChunkedArray<Index> Nodes; // Guarantees pointer stability for Node allocations
	TIntrusiveDoubleLinkedList<Index> LRU; // Double linked list wrapper on Nodes
	FCriticalSection LRUMutex; // Mutex for the LRU to make AddRef thread safe
};

