// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

#include "PixelFormat.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TextureAtlasBase.generated.h"

UCLASS(BlueprintType, Abstract)
class BLACKRUNTIMERESOURCES_API UTextureAtlasBase : public UObject
{
	GENERATED_BODY()

public:
	// --- Setup ---
	virtual void Initialize(
		int32 InAtlasWidth, int32 InAtlasHeight,
		int32 InTileWidth, int32 InTileHeight,
		int32 InTilePadding, EPixelFormat InFormat
	);

	// --- Blueprint Accessors ---
	UFUNCTION(BlueprintPure)
	inline FVector2D GetTileUVSize() const;

	UFUNCTION(BlueprintPure)
	inline FVector2D GetTileUVOffset(FIntPoint TileIndex) const;

	// --- Atlas Info ---
	FORCEINLINE bool IsInitialized() const { return AtlasTexture != nullptr; }
	FORCEINLINE int32 GetAtlasWidth() const { return AtlasWidth; }
	FORCEINLINE int32 GetAtlasHeight() const { return AtlasHeight; }

	// --- Tile Info ---
	FORCEINLINE int32 GetTileWidth() const { return TileWidth; }
	FORCEINLINE int32 GetTileHeight() const { return TileHeight; }
	FORCEINLINE int32 GetTilePadding() const { return TilePadding; }
	FORCEINLINE EPixelFormat GetPixelFormat() const { return PixelFormat; }
	FORCEINLINE UTexture2D* GetAtlasTexture() const { return AtlasTexture; }

	// --- Derived Info ---
	FORCEINLINE int32 GetMaxTileIndexX() const { return MaxTileIndexX; }
	FORCEINLINE int32 GetMaxTileIndexY() const { return MaxTileIndexY; }
	FORCEINLINE int32 GetMaxTileCount() const { return MaxTileCount; }


protected:
	// --- Tile management ---
	virtual void WriteTile(
		FIntPoint Index,
		TArray<uint8>& PixelData
	);

	virtual void WriteTiles(
		TArray<FIntPoint>& TileIndices,
		TArray<uint8>& PixelData
	);

	// --- Atlas Properties ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 AtlasWidth = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 AtlasHeight = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 TileWidth = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 TileHeight = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 TilePadding = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EPixelFormat> PixelFormat = PF_Unknown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AtlasTexture = nullptr;

	// --- Derived Cache (not exposed) ---
	int32 MaxTileIndexX = 0;
	int32 MaxTileIndexY = 0;
	int32 MaxTileCount = 0;

	float TileUVStepX = 0.f;
	float TileUVStepY = 0.f;
	float PaddingUVStepX = 0.f;
	float PaddingUVStepY = 0.f;
};