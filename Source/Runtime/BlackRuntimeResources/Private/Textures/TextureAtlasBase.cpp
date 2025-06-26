// Copyright (c) Black Megacorp. All Rights Reserved.

#include "Textures/TextureAtlasBase.h"

FVector2D UTextureAtlasBase::GetTileUVSize() const
{
	check(IsInitialized());

	return FVector2D(
		float(TileWidth) / float(AtlasWidth),
		float(TileHeight) / float(AtlasHeight)
	);
}

FVector2D UTextureAtlasBase::GetTileUVOffset(FIntPoint TileIndex) const
{
	check(IsInitialized());
	check(TileIndex.X < MaxTileIndexX && TileIndex.Y < MaxTileIndexY);

	return FVector2D(
		TileIndex.X * TileUVStepX + PaddingUVStepX,
		TileIndex.Y * TileUVStepY + PaddingUVStepY
	);
}

void UTextureAtlasBase::Initialize(
	int32 InAtlasWidth, int32 InAtlasHeight,
	int32 InTileWidth, int32 InTileHeight,
	int32 InTilePadding, EPixelFormat InFormat
)
{
	AtlasWidth = InAtlasWidth;
	AtlasHeight = InAtlasHeight;
	TileWidth = InTileWidth;
	TileHeight = InTileHeight;
	TilePadding = InTilePadding;
	PixelFormat = InFormat;

	MaxTileIndexX = AtlasWidth / (TileWidth + TilePadding * 2) - 1;
	MaxTileIndexY = AtlasHeight / (TileHeight + TilePadding * 2) - 1;
	MaxTileCount = (MaxTileIndexX + 1) * (MaxTileIndexY + 1);

	TileUVStepX = float(TileWidth + 2 * TilePadding) / float(AtlasWidth);
	TileUVStepY = float(TileHeight + 2 * TilePadding) / float(AtlasHeight);
	PaddingUVStepX = float(TilePadding) / float(AtlasWidth);
	PaddingUVStepY = float(TilePadding) / float(AtlasHeight);

	AtlasTexture = UTexture2D::CreateTransient(AtlasWidth, AtlasHeight, PixelFormat);
	AtlasTexture->Filter = TF_Nearest;
	AtlasTexture->NeverStream = true;
	AtlasTexture->SRGB = false;
	AtlasTexture->UpdateResource();
}

void UTextureAtlasBase::WriteTile(
	FIntPoint Index,
	TArray<uint8>& PixelData
)
{
	TArray<FIntPoint> TileIndices({ Index });
	WriteTiles(TileIndices, PixelData);
}

void UTextureAtlasBase::WriteTiles(
	TArray<FIntPoint>& TileIndices,
	TArray<uint8>& PixelData
)
{
	check(IsInitialized());

	int32 BytesPerPixel = GPixelFormats[PixelFormat].BlockBytes;
	int32 TileCount = TileIndices.Num();

	check(PixelData.Num() >= TileWidth * TileHeight * BytesPerPixel * TileCount);

	TArray<FUpdateTextureRegion2D> Regions;

	for (int32 i = 0; i < TileCount; ++i)
	{
		FIntPoint Index = TileIndices[i];
		check(Index.X <= MaxTileIndexX && Index.Y <= MaxTileIndexY);

		FUpdateTextureRegion2D Region;
		Region.DestX = (TilePadding * 2 + TileWidth) * Index.X;
		Region.DestY = (TilePadding * 2 + TileHeight) * Index.Y;
		Region.SrcX = 0;
		Region.SrcY = TileHeight * i;
		Region.Width = TileWidth;
		Region.Height = TileHeight;
		Regions.Add(Region);
	}

	AtlasTexture->UpdateTextureRegions(
		0,
		TileCount,
		Regions.GetData(),
		TileWidth * BytesPerPixel,
		BytesPerPixel,
		PixelData.GetData()
	);
}