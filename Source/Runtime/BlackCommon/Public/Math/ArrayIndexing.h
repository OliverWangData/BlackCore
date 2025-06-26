// Copyright (c) Black Megacorp. All Rights Reserved.

#pragma once

namespace blk
{
	// Returns total number of elements in a 2D grid (Width x Height)
	inline constexpr int32 NumElements2D(const int32 Width, const int32 Height) {
		return Width * Height;
	}

	// Returns total number of elements in a 3D grid (Width x Height x Depth)
	inline constexpr int32 NumElements3D(const int32 Width, const int32 Height, const int32 Depth) {
		return Width * Height * Depth;
	}

	// Converts 2D coordinates (X,Y) to a flattened 1D index
	inline constexpr int32 Index2DTo1D(const int32 X, const int32 Y, const int32 Width) {
		return (Y * Width) + X;
	}

	// Converts 3D coordinates (X,Y,Z) to a flattened 1D index
	inline constexpr int32 Index3DTo1D(
		const int32 X, const int32 Y, const int32 Z,
		const int32 Width, const int32 Height) {
		return (Z * Width * Height) + (Y * Width) + X;
	}

	// Converts a 1D index back into 2D coordinates (X,Y)
	inline constexpr void Index1DTo2D(const int32 Index, const int32 Width, int32& OutX, int32& OutY) {
		OutY = Index / Width;
		OutX = Index % Width;
	}

	// Converts a 1D index back into 3D coordinates (X,Y,Z)
	inline constexpr void Index1DTo3D(
		const int32 Index,
		const int32 Width, const int32 Height,
		int32& OutX, int32& OutY, int32& OutZ) {
		const int32 SizeXY = Width * Height;
		const int32 Remainder = Index % SizeXY;
		OutZ = Index / SizeXY;
		OutY = Remainder / Width;
		OutX = Remainder % Width;
	}

	// Adjust a 1D index by offsetting by (X,Y) in a 2D grid
	inline constexpr int32 AdjustIndex2D(const int32 Index, const int32 Width, int32 OffsetX, int32 OffsetY) {
		return Index + Index2DTo1D(OffsetX, OffsetY, Width);
	}

	// Adjust a 1D index by offsetting by (X,Y,Z) in a 3D grid
	inline constexpr int32 AdjustIndex3D(
		const int32 Index, const int32 Width, const int32 Height,
		const int32 OffsetX, const int32 OffsetY, const int32 OffsetZ) {
		return Index + Index3DTo1D(OffsetX, OffsetY, OffsetZ, Width, Height);
	}

	// Returns padding needed to align Count up to the nearest multiple of Alignment
	inline constexpr int32 GetPaddingForAlignment(int32 Count, int32 Alignment) {
		int32 Pad = Alignment - (Count % Alignment);
		return (Pad == Alignment) ? 0 : Pad;
	}
}