/**
 * Copyright (C) 2021-2022 Hypertheory
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

namespace Ink {

enum RenderingSide {
	FRONT_SIDE,
	BACK_SIDE,
	DOUBLE_SIDE,
};

enum ImageType {
	IMAGE_UBYTE,
	IMAGE_BYTE,
	IMAGE_USHORT,
	IMAGE_SHORT,
	IMAGE_UINT,
	IMAGE_INT,
	IMAGE_HALF_FLOAT,
	IMAGE_FLOAT,
	IMAGE_UINT_24_8,
};

enum ComparisonFunction {
	FUNC_NEVER,
	FUNC_LESS,
	FUNC_EQUAL,
	FUNC_LEQUAL,
	FUNC_GREATER,
	FUNC_NOTEQUAL,
	FUNC_GEQUAL,
	FUNC_ALWAYS,
};

enum StencilOperation {
	STENCIL_ZERO,
	STENCIL_KEEP,
	STENCIL_REPLACE,
	STENCIL_INCR,
	STENCIL_DECR,
	STENCIL_INCR_WRAP,
	STENCIL_DECR_WRAP,
	STENCIL_INVERT,
};

enum BlendOperation {
	BLEND_ADD,
	BLEND_SUBTRACT,
	BLEND_REVERSE_SUBTRACT,
	BLEND_MIN,
	BLEND_MAX,
};

enum BlendFactor {
	FACTOR_ZERO,
	FACTOR_ONE,
	FACTOR_SRC_COLOR,
	FACTOR_ONE_MINUS_SRC_COLOR,
	FACTOR_DST_COLOR,
	FACTOR_ONE_MINUS_DST_COLOR,
	FACTOR_SRC_ALPHA,
	FACTOR_ONE_MINUS_SRC_ALPHA,
	FACTOR_DST_ALPHA,
	FACTOR_ONE_MINUS_DST_ALPHA,
};

enum TextureType {
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_CUBE,
	TEXTURE_1D_ARRAY,
	TEXTURE_2D_ARRAY,
	TEXTURE_CUBE_ARRAY,
};

enum TextureFormat {
	TEXTURE_R8_UNORM,
	TEXTURE_R8_SNORM,
	TEXTURE_R16_UNORM,
	TEXTURE_R16_SNORM,
	TEXTURE_R8G8_UNORM,
	TEXTURE_R8G8_SNORM,
	TEXTURE_R16G16_UNORM,
	TEXTURE_R16G16_SNORM,
	TEXTURE_R3G3B2_UNORM,
	TEXTURE_R4G4B4_UNORM,
	TEXTURE_R5G5B5_UNORM,
	TEXTURE_R8G8B8_UNORM,
	TEXTURE_R8G8B8_SNORM,
	TEXTURE_R10G10B10_UNORM,
	TEXTURE_R12G12B12_UNORM,
	TEXTURE_R16G16B16_UNORM,
	TEXTURE_R2G2B2A2_UNORM,
	TEXTURE_R4G4B4A4_UNORM,
	TEXTURE_R5G5B5A1_UNORM,
	TEXTURE_R8G8B8A8_UNORM,
	TEXTURE_R8G8B8A8_SNORM,
	TEXTURE_R10G10B10A2_UNORM,
	TEXTURE_R10G10B10A2_UINT,
	TEXTURE_R12G12B12A12_UNORM,
	TEXTURE_R16G16B16A16_UNORM,
	TEXTURE_R8G8B8_SRGB,
	TEXTURE_R8G8B8A8_SRGB,
	TEXTURE_R16_SFLOAT,
	TEXTURE_R16G16_SFLOAT,
	TEXTURE_R16G16B16_SFLOAT,
	TEXTURE_R16G16B16A16_SFLOAT,
	TEXTURE_R32_SFLOAT,
	TEXTURE_R32G32_SFLOAT,
	TEXTURE_R32G32B32_SFLOAT,
	TEXTURE_R32G32B32A32_SFLOAT,
	TEXTURE_R11G11B10_SFLOAT,
	TEXTURE_R9G9B9E5_UNORM,
	TEXTURE_R8_SINT,
	TEXTURE_R8_UINT,
	TEXTURE_R16_SINT,
	TEXTURE_R16_UINT,
	TEXTURE_R32_SINT,
	TEXTURE_R32_UINT,
	TEXTURE_R8G8_SINT,
	TEXTURE_R8G8_UINT,
	TEXTURE_R16G16_SINT,
	TEXTURE_R16G16_UINT,
	TEXTURE_R32G32_SINT,
	TEXTURE_R32G32_UINT,
	TEXTURE_R8G8B8_SINT,
	TEXTURE_R8G8B8_UINT,
	TEXTURE_R16G16B16_SINT,
	TEXTURE_R16G16B16_UINT,
	TEXTURE_R32G32B32_SINT,
	TEXTURE_R32G32B32_UINT,
	TEXTURE_R8G8B8A8_SINT,
	TEXTURE_R8G8B8A8_UINT,
	TEXTURE_R16G16B16A16_SINT,
	TEXTURE_R16G16B16A16_UINT,
	TEXTURE_R32G32B32A32_SINT,
	TEXTURE_R32G32B32A32_UINT,
	TEXTURE_D16_UNORM,
	TEXTURE_D24_UNORM,
	TEXTURE_D32_SFLOAT,
	TEXTURE_D24_UNORM_S8_UINT,
	TEXTURE_D32_SFLOAT_S8_UINT,
};

enum TextureWrappingMode {
	TEXTURE_REPEAT,
	TEXTURE_MIRRORED_REPEAT,
	TEXTURE_CLAMP_TO_EDGE,
	TEXTURE_CLAMP_TO_BORDER,
};

enum TextureFilter {
	TEXTURE_NEAREST,
	TEXTURE_LINEAR,
	TEXTURE_NEAREST_MIPMAP_NEAREST,
	TEXTURE_LINEAR_MIPMAP_NEAREST,
	TEXTURE_NEAREST_MIPMAP_LINEAR,
	TEXTURE_LINEAR_MIPMAP_LINEAR,
};

}
