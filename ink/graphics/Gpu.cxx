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

#include "Gpu.h"

#include <iostream>
#include <memory>

#include "../../libs/glad/glad.h"

#include "../core/File.h"

namespace Ink::Gpu {

constexpr uint32_t GL_COMPARISON_FUNCTIONS[] = {
	GL_NEVER,                                                 /**< FUNC_NEVER */
	GL_LESS,                                                  /**< FUNC_LESS */
	GL_EQUAL,                                                 /**< FUNC_EQUAL */
	GL_LEQUAL,                                                /**< FUNC_LEQUAL */
	GL_GREATER,                                               /**< FUNC_GREATER */
	GL_NOTEQUAL,                                              /**< FUNC_NOTEQUAL */
	GL_GEQUAL,                                                /**< FUNC_GEQUAL */
	GL_ALWAYS,                                                /**< FUNC_ALWAYS */
};

constexpr uint32_t GL_STENCIL_OPERATIONS[] = {
	GL_ZERO,                                                  /**< STENCIL_ZERO */
	GL_KEEP,                                                  /**< STENCIL_KEEP */
	GL_REPLACE,                                               /**< STENCIL_REPLACE */
	GL_INCR,                                                  /**< STENCIL_INCR */
	GL_DECR,                                                  /**< STENCIL_DECR */
	GL_INCR_WRAP,                                             /**< STENCIL_INCR_WRAP */
	GL_DECR_WRAP,                                             /**< STENCIL_DECR_WRAP */
	GL_INVERT,                                                /**< STENCIL_INVERT */
};

constexpr uint32_t GL_BLEND_OPERATIONS[] = {
	GL_FUNC_ADD,                                              /**< BLEND_ADD */
	GL_FUNC_SUBTRACT,                                         /**< BLEND_SUBTRACT */
	GL_FUNC_REVERSE_SUBTRACT,                                 /**< BLEND_REVERSE_SUBTRACT */
	GL_MIN,                                                   /**< BLEND_MIN */
	GL_MAX,                                                   /**< BLEND_MAX */
};

constexpr uint32_t GL_BLEND_FACTORS[] = {
	GL_ZERO,	                                              /**< FACTOR_ZERO */
	GL_ONE,                                                   /**< FACTOR_ONE */
	GL_SRC_COLOR,                                             /**< FACTOR_SRC_COLOR */
	GL_ONE_MINUS_SRC_COLOR,                                   /**< FACTOR_ONE_MINUS_SRC_COLOR */
	GL_DST_COLOR,                                             /**< FACTOR_DST_COLOR */
	GL_ONE_MINUS_DST_COLOR,                                   /**< FACTOR_ONE_MINUS_DST_COLOR */
	GL_SRC_ALPHA,                                             /**< FACTOR_SRC_ALPHA */
	GL_ONE_MINUS_SRC_ALPHA,                                   /**< FACTOR_ONE_MINUS_SRC_ALPHA */
	GL_DST_ALPHA,                                             /**< FACTOR_DST_ALPHA */
	GL_ONE_MINUS_DST_ALPHA,                                   /**< FACTOR_ONE_MINUS_DST_ALPHA */
};

constexpr uint32_t GL_IMAGE_TYPES[] = {
	GL_UNSIGNED_BYTE,                                         /**< IMAGE_UBYTE */
	GL_BYTE,                                                  /**< IMAGE_BYTE */
	GL_UNSIGNED_SHORT,                                        /**< IMAGE_USHORT */
	GL_SHORT,                                                 /**< IMAGE_SHORT */
	GL_UNSIGNED_INT,                                          /**< IMAGE_UINT */
	GL_INT,                                                   /**< IMAGE_INT */
	GL_HALF_FLOAT,                                            /**< IMAGE_HALF_FLOAT */
	GL_FLOAT,                                                 /**< IMAGE_FLOAT */
	GL_UNSIGNED_INT_24_8,                                     /**< IMAGE_UINT_24_8 */
};

constexpr uint32_t GL_TEXTURE_TYPES[] = {
	GL_TEXTURE_1D,                                            /**< TEXTURE_1D */
	GL_TEXTURE_2D,                                            /**< TEXTURE_2D */
	GL_TEXTURE_3D,                                            /**< TEXTURE_3D */
	GL_TEXTURE_CUBE_MAP,                                      /**< TEXTURE_CUBE */
	GL_TEXTURE_1D_ARRAY,                                      /**< TEXTURE_1D_ARRAY */
	GL_TEXTURE_2D_ARRAY,                                      /**< TEXTURE_2D_ARRAY */
	GL_TEXTURE_CUBE_MAP_ARRAY,                                /**< TEXTURE_CUBE_ARRAY */
};

constexpr std::pair<uint32_t, int32_t> GL_TEXTURE_FORMATS[] = {
	{GL_RED            , GL_R8                },              /**< TEXTURE_R8_UNORM */
	{GL_RED            , GL_R8_SNORM          },              /**< TEXTURE_R8_SNORM */
	{GL_RED            , GL_R16               },              /**< TEXTURE_R16_UNORM */
	{GL_RED            , GL_R16_SNORM         },              /**< TEXTURE_R16_SNORM */
	{GL_RG             , GL_RG8               },              /**< TEXTURE_R8G8_UNORM */
	{GL_RG             , GL_RG8_SNORM         },              /**< TEXTURE_R8G8_SNORM */
	{GL_RG             , GL_RG16              },              /**< TEXTURE_R16G16_UNORM */
	{GL_RG             , GL_RG16_SNORM        },              /**< TEXTURE_R16G16_SNORM */
	{GL_RGB            , GL_R3_G3_B2          },              /**< TEXTURE_R3G3B2_UNORM */
	{GL_RGB            , GL_RGB4              },              /**< TEXTURE_R4G4B4_UNORM */
	{GL_RGB            , GL_RGB5              },              /**< TEXTURE_R5G5B5_UNORM */
	{GL_RGB            , GL_RGB8              },              /**< TEXTURE_R8G8B8_UNORM */
	{GL_RGB            , GL_RGB8_SNORM        },              /**< TEXTURE_R8G8B8_SNORM */
	{GL_RGB            , GL_RGB10             },              /**< TEXTURE_R10G10B10_UNORM */
	{GL_RGB            , GL_RGB12             },              /**< TEXTURE_R12G12B12_UNORM */
	{GL_RGB            , GL_RGB16_SNORM       },              /**< TEXTURE_R16G16B16_UNORM */
	{GL_RGB            , GL_RGBA2             },              /**< TEXTURE_R2G2B2A2_UNORM */
	{GL_RGB            , GL_RGBA4             },              /**< TEXTURE_R4G4B4A4_UNORM */
	{GL_RGBA           , GL_RGB5_A1           },              /**< TEXTURE_R5G5B5A1_UNORM */
	{GL_RGBA           , GL_RGBA8             },              /**< TEXTURE_R8G8B8A8_UNORM */
	{GL_RGBA           , GL_RGBA8_SNORM       },              /**< TEXTURE_R8G8B8A8_SNORM */
	{GL_RGBA           , GL_RGB10_A2          },              /**< TEXTURE_R10G10B10A2_UNORM */
	{GL_RGBA           , GL_RGB10_A2UI        },              /**< TEXTURE_R10G10B10A2_UINT */
	{GL_RGBA           , GL_RGBA12            },              /**< TEXTURE_R12G12B12A12_UNORM */
	{GL_RGBA           , GL_RGBA16            },              /**< TEXTURE_R16G16B16A16_UNORM */
	{GL_RGB            , GL_SRGB8             },              /**< TEXTURE_R8G8B8_SRGB */
	{GL_RGBA           , GL_SRGB8_ALPHA8      },              /**< TEXTURE_R8G8B8A8_SRGB */
	{GL_RED            , GL_R16F              },              /**< TEXTURE_R16_SFLOAT */
	{GL_RG             , GL_RG16F             },              /**< TEXTURE_R16G16_SFLOAT */
	{GL_RGB            , GL_RGB16F            },              /**< TEXTURE_R16G16B16_SFLOAT */
	{GL_RGBA           , GL_RGBA16F           },              /**< TEXTURE_R16G16B16A16_SFLOAT */
	{GL_RED            , GL_R32F              },              /**< TEXTURE_R32_SFLOAT */
	{GL_RG             , GL_RG32F             },              /**< TEXTURE_R32G32_SFLOAT */
	{GL_RGB            , GL_RGB32F            },              /**< TEXTURE_R32G32B32_SFLOAT */
	{GL_RGBA           , GL_RGBA32F           },              /**< TEXTURE_R32G32B32A32_SFLOAT */
	{GL_RGB            , GL_R11F_G11F_B10F    },              /**< TEXTURE_R11G11B10_SFLOAT */
	{GL_RGB            , GL_RGB9_E5           },              /**< TEXTURE_R9G9B9E5_UNORM */
	{GL_RED            , GL_R8I               },              /**< TEXTURE_R8_SINT */
	{GL_RED            , GL_R8UI              },              /**< TEXTURE_R8_UINT */
	{GL_RED            , GL_R16I              },              /**< TEXTURE_R16_SINT */
	{GL_RED            , GL_R16UI             },              /**< TEXTURE_R16_UINT */
	{GL_RED            , GL_R32I              },              /**< TEXTURE_R32_SINT */
	{GL_RED            , GL_R32UI             },              /**< TEXTURE_R32_UINT */
	{GL_RG             , GL_RG8I              },              /**< TEXTURE_R8G8_SINT */
	{GL_RG             , GL_RG8UI             },              /**< TEXTURE_R8G8_UINT */
	{GL_RG             , GL_RG16I             },              /**< TEXTURE_R16G16_SINT */
	{GL_RG             , GL_RG16UI            },              /**< TEXTURE_R16G16_UINT */
	{GL_RG             , GL_RG32I             },              /**< TEXTURE_R32G32_SINT */
	{GL_RG             , GL_RG32UI            },              /**< TEXTURE_R32G32_UINT */
	{GL_RGB            , GL_RGB8I             },              /**< TEXTURE_R8G8B8_SINT */
	{GL_RGB            , GL_RGB8UI            },              /**< TEXTURE_R8G8B8_UINT */
	{GL_RGB            , GL_RGB16I            },              /**< TEXTURE_R16G16B16_SINT */
	{GL_RGB            , GL_RGB16UI           },              /**< TEXTURE_R16G16B16_UINT */
	{GL_RGB            , GL_RGB32I            },              /**< TEXTURE_R32G32B32_SINT */
	{GL_RGB            , GL_RGB32UI           },              /**< TEXTURE_R32G32B32_UINT */
	{GL_RGBA           , GL_RGBA8I            },              /**< TEXTURE_R8G8B8A8_SINT */
	{GL_RGBA           , GL_RGBA8UI           },              /**< TEXTURE_R8G8B8A8_UINT */
	{GL_RGBA           , GL_RGBA16I           },              /**< TEXTURE_R16G16B16A16_SINT */
	{GL_RGBA           , GL_RGBA16UI          },              /**< TEXTURE_R16G16B16A16_UINT */
	{GL_RGBA           , GL_RGBA32I           },              /**< TEXTURE_R32G32B32A32_SINT */
	{GL_RGBA           , GL_RGBA32UI          },              /**< TEXTURE_R32G32B32A32_UINT */
	{GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16 },              /**< TEXTURE_D16_UNORM */
	{GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24 },              /**< TEXTURE_D24_UNORM */
	{GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F},              /**< TEXTURE_D32_SFLOAT */
	{GL_DEPTH_STENCIL  , GL_DEPTH24_STENCIL8  },              /**< TEXTURE_D24_UNORM_S8_UINT */
	{GL_DEPTH_STENCIL  , GL_DEPTH32F_STENCIL8 },              /**< TEXTURE_D32_SFLOAT_S8_UINT */
};

constexpr int32_t GL_TEXTURE_WRAPPINGS[] = {
	GL_REPEAT,                                                /**< TEXTURE_REPEAT */
	GL_MIRRORED_REPEAT,                                       /**< TEXTURE_MIRRORED_REPEAT */
	GL_CLAMP_TO_EDGE,                                         /**< TEXTURE_CLAMP_TO_EDGE */
	GL_CLAMP_TO_BORDER,                                       /**< TEXTURE_CLAMP_TO_BORDER */
};

constexpr int32_t GL_TEXTURE_FILTERS[] = {
	GL_NEAREST,                                               /**< TEXTURE_NEAREST */
	GL_LINEAR,                                                /**< TEXTURE_LINEAR */
	GL_NEAREST_MIPMAP_NEAREST,                                /**< TEXTURE_NEAREST_MIPMAP_NEAREST */
	GL_LINEAR_MIPMAP_NEAREST,                                 /**< TEXTURE_LINEAR_MIPMAP_NEAREST */
	GL_NEAREST_MIPMAP_LINEAR,                                 /**< TEXTURE_NEAREST_MIPMAP_LINEAR */
	GL_LINEAR_MIPMAP_LINEAR,                                  /**< TEXTURE_LINEAR_MIPMAP_LINEAR */
};

constexpr int TEXTURE_DEFAULT_FORMATS[][2] = {
	{TEXTURE_R8_UNORM      , TEXTURE_R16_SFLOAT         },
	{TEXTURE_R8G8_UNORM    , TEXTURE_R16G16_SFLOAT      },
	{TEXTURE_R8G8B8_UNORM  , TEXTURE_R16G16B16_SFLOAT   },
	{TEXTURE_R8G8B8A8_UNORM, TEXTURE_R16G16B16A16_SFLOAT},
};

int get_comparison_functions(uint32_t v) {
	if (v == GL_NEVER   ) return FUNC_NEVER;
	if (v == GL_LESS    ) return FUNC_LESS;
	if (v == GL_EQUAL   ) return FUNC_EQUAL;
	if (v == GL_LEQUAL  ) return FUNC_LEQUAL;
	if (v == GL_GREATER ) return FUNC_GREATER;
	if (v == GL_NOTEQUAL) return FUNC_NOTEQUAL;
	if (v == GL_GEQUAL  ) return FUNC_GEQUAL;
	/*   ... GL_ALWAYS */ return FUNC_ALWAYS;
}

int get_stencil_operations(uint32_t v) {
	if (v == GL_ZERO     ) return STENCIL_ZERO;
	if (v == GL_KEEP     ) return STENCIL_KEEP;
	if (v == GL_REPLACE  ) return STENCIL_REPLACE;
	if (v == GL_INCR     ) return STENCIL_INCR;
	if (v == GL_DECR     ) return STENCIL_DECR;
	if (v == GL_INCR_WRAP) return STENCIL_INCR_WRAP;
	if (v == GL_DECR_WRAP) return STENCIL_DECR_WRAP;
	/*   ... GL_ZERO    */ return STENCIL_INVERT;
}

int get_blend_equations(uint32_t v) {
	if (v == GL_FUNC_ADD             ) return BLEND_ADD;
	if (v == GL_FUNC_SUBTRACT        ) return BLEND_SUBTRACT;
	if (v == GL_FUNC_REVERSE_SUBTRACT) return BLEND_REVERSE_SUBTRACT;
	if (v == GL_MIN                  ) return BLEND_MIN;
	/*   ... GL_MAX                 */ return BLEND_MAX;
}

int get_blend_functions(uint32_t v) {
	if (v == GL_ZERO               ) return FACTOR_ZERO;
	if (v == GL_ONE                ) return FACTOR_ONE;
	if (v == GL_SRC_COLOR          ) return FACTOR_SRC_COLOR;
	if (v == GL_ONE_MINUS_SRC_COLOR) return FACTOR_ONE_MINUS_SRC_COLOR;
	if (v == GL_DST_COLOR          ) return FACTOR_DST_COLOR;
	if (v == GL_ONE_MINUS_DST_COLOR) return FACTOR_ONE_MINUS_DST_COLOR;
	if (v == GL_SRC_ALPHA          ) return FACTOR_SRC_ALPHA;
	if (v == GL_ONE_MINUS_SRC_ALPHA) return FACTOR_ONE_MINUS_SRC_ALPHA;
	if (v == GL_DST_ALPHA          ) return FACTOR_DST_ALPHA;
	/* ... GL_ONE_MINUS_DST_ALPHA */ return FACTOR_ONE_MINUS_DST_ALPHA;
}

Rect::Rect(int w, int h) : width(w), height(h) {}

Rect::Rect(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}

void finish() {
	glFinish();
}

void flush() {
	glFlush();
}

void clear(bool c, bool d, bool s) {
	GLbitfield flag = 0;
	flag |= GL_COLOR_BUFFER_BIT * c;
	flag |= GL_DEPTH_BUFFER_BIT * d;
	flag |= GL_STENCIL_BUFFER_BIT * s;
	glClear(flag);
}

Vec4 get_clear_color() {
	Vec4 clear_color;
	glGetFloatv(GL_COLOR_CLEAR_VALUE, &clear_color.x);
	return clear_color;
}

void set_clear_color(const Vec3& c, float a) {
	glClearColor(c.x, c.y, c.z, a);
}

void set_clear_color(const Vec4& c) {
	glClearColor(c.x, c.y, c.z, c.w);
}

void enable_depth_test() {
	glEnable(GL_DEPTH_TEST);
}

void disable_depth_test() {
	glDisable(GL_DEPTH_TEST);
}

int get_depth_func() {
	int depth_func = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
	return get_comparison_functions(depth_func);
}

void set_depth_func(int f) {
	glDepthFunc(GL_COMPARISON_FUNCTIONS[f]);
}

void enable_stencil_test() {
	glEnable(GL_STENCIL_TEST);
}

void disable_stencil_test() {
	glDisable(GL_STENCIL_TEST);
}

int get_stencil_writemask() {
	int stencil_writemask = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &stencil_writemask);
	return stencil_writemask;
}

void set_stencil_writemask(int m) {
	glStencilMask(m);
}

int get_stencil_func() {
	int stencil_func = 0;
	glGetIntegerv(GL_STENCIL_FUNC, &stencil_func);
	return get_comparison_functions(stencil_func);
}

int get_stencil_ref() {
	int stencil_ref = 0;
	glGetIntegerv(GL_STENCIL_REF, &stencil_ref);
	return stencil_ref;
}

int get_stencil_mask() {
	int stencil_mask = 0;
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencil_mask);
	return stencil_mask;
}

void set_stencil_func(int f, int r, int m) {
	glStencilFunc(GL_COMPARISON_FUNCTIONS[f], r, m);
}

int get_stencil_fail() {
	int stencil_fail = 0;
	glGetIntegerv(GL_STENCIL_FAIL, &stencil_fail);
	return get_stencil_operations(stencil_fail);
}

int get_stencil_zfail() {
	int stencil_zfail = 0;
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &stencil_zfail);
	return get_stencil_operations(stencil_zfail);
}

int get_stencil_zpass() {
	int stencil_zpass = 0;
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &stencil_zpass);
	return get_stencil_operations(stencil_zpass);
}

void set_stencil_op(int f, int zf, int zp) {
	glStencilOp(GL_STENCIL_OPERATIONS[f],
				GL_STENCIL_OPERATIONS[zf],
				GL_STENCIL_OPERATIONS[zp]);
}

void enable_blending() {
	glEnable(GL_BLEND);
}

void disable_blending() {
	glDisable(GL_BLEND);
}

int get_blend_op() {
	int blend_op = 0;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend_op);
	return get_blend_equations(blend_op);
}

int get_blend_op_alpha() {
	int blend_op = 0;
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_op);
	return get_blend_equations(blend_op);
}

void set_blend_op(int rgb, int a) {
	glBlendEquationSeparate(GL_BLEND_OPERATIONS[rgb],
							GL_BLEND_OPERATIONS[a]);
}

int get_blend_src() {
	int blend_func = 0;
	glGetIntegerv(GL_BLEND_SRC_RGB, &blend_func);
	return get_blend_functions(blend_func);
}

int get_blend_src_alpha() {
	int blend_func = 0;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_func);
	return get_blend_functions(blend_func);
}

int get_blend_dst() {
	int blend_func = 0;
	glGetIntegerv(GL_BLEND_DST_RGB, &blend_func);
	return get_blend_functions(blend_func);
}

int get_blend_dst_alpha() {
	int blend_func = 0;
	glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_func);
	return get_blend_functions(blend_func);
}

void set_blend_factor(int srgb, int drgb, int sa, int da) {
	glBlendFuncSeparate(GL_BLEND_FACTORS[srgb],
						GL_BLEND_FACTORS[drgb],
						GL_BLEND_FACTORS[sa],
						GL_BLEND_FACTORS[da]);
}

void enable_scissor_test() {
	glEnable(GL_SCISSOR_TEST);
}

void disable_scissor_test() {
	glDisable(GL_SCISSOR_TEST);
}

Rect get_scissor() {
	Rect scissor_rect;
	glGetIntegerv(GL_SCISSOR_BOX, &scissor_rect.x);
	return scissor_rect;
}

void set_scissor(const Rect& s) {
	glScissor(s.x, s.y, s.width, s.height);
}

void enable_wireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void disable_wireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void enable_cull_face() {
	glEnable(GL_CULL_FACE);
}

void disable_cull_face() {
	glDisable(GL_CULL_FACE);
}

int get_cull_face() {
	int cull_face = 0;
	glGetIntegerv(GL_CULL_FACE, &cull_face);
	if (cull_face == GL_FRONT) return FRONT_SIDE;
	if (cull_face == GL_BACK ) return BACK_SIDE;
	return DOUBLE_SIDE;
}

void set_cull_face(int s) {
	if (s == FRONT_SIDE) {
		glCullFace(GL_FRONT);
	} else if (s == BACK_SIDE) {
		glCullFace(GL_BACK);
	} else if (s == DOUBLE_SIDE) {
		glCullFace(GL_FRONT_AND_BACK);
	}
}

Rect get_viewport() {
	Rect viewport_rect;
	glGetIntegerv(GL_VIEWPORT, &viewport_rect.x);
	return viewport_rect;
}

void set_viewport(const Rect& v) {
	glViewport(v.x, v.y, v.width, v.height);
}

void enable_multisample() {
	glEnable(GL_MULTISAMPLE);
}

void disable_multisample() {
	glDisable(GL_MULTISAMPLE);
}

void enable_texture_cube_seamless() {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void disable_texture_cube_seamless() {
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void apply_material_depth(const Material& m) {
	if (!m.depth_test) return disable_depth_test();
	enable_depth_test();
	set_depth_func(m.depth_func);
}

void apply_material_stencil(const Material& m) {
	if (!m.stencil_test) return disable_stencil_test();
	enable_stencil_test();
	set_stencil_writemask(m.stencil_writemask);
	set_stencil_func(m.stencil_func, m.stencil_ref, m.stencil_mask);
	set_stencil_op(m.stencil_fail, m.stencil_zfail, m.stencil_zpass);
}

void apply_material_blending(const Material& m) {
	if (!m.blending) return disable_blending();
	enable_blending();
	set_blend_op(m.blend_op, m.blend_op_alpha);
	set_blend_factor(m.blend_src, m.blend_dst, m.blend_src_alpha, m.blend_dst_alpha);
}

void apply_material_wireframe(const Material& m) {
	if (!m.wireframe) return disable_wireframe();
	enable_wireframe();
}

void apply_material_side(const Material& m) {
	if (m.side == FRONT_SIDE) {
		enable_cull_face();
		set_cull_face(BACK_SIDE);
	} else if (m.side == BACK_SIDE) {
		enable_cull_face();
		set_cull_face(FRONT_SIDE);
	} else if (m.side == DOUBLE_SIDE) {
		disable_cull_face();
	}
}

void apply_material_shadow_side(const Material& m) {
	if (m.shadow_side == FRONT_SIDE) {
		enable_cull_face();
		set_cull_face(BACK_SIDE);
	} else if (m.shadow_side == BACK_SIDE) {
		enable_cull_face();
		set_cull_face(FRONT_SIDE);
	} else if (m.shadow_side == DOUBLE_SIDE) {
		disable_cull_face();
	}
}

std::string get_error() {
	std::string info;
	uint32_t error = glGetError();
	if (error == GL_NO_ERROR) return "";
	while (error != GL_NO_ERROR) {
		info += "OpenGL: " + std::to_string(error) + ": ";
		if (error == GL_INVALID_ENUM) {
			info += "An unacceptable value is specified for an enumerated argument.\n";
		} else if (error == GL_INVALID_VALUE) {
			info += "A numeric argument is out of range.\n";
		} else if (error == GL_INVALID_OPERATION) {
			info += "The specified operation is not allowed in the current state.\n";
		} else if (error == GL_OUT_OF_MEMORY) {
			info += "There is not enough memory left to execute the command.\n";
		} else {
			info += "An unknown error has occurred.\n";
		}
		error = glGetError();
	}
	return info;
}

Shader::Shader() {
	program = glCreateProgram();
}

Shader::~Shader() {
	glDeleteProgram(program);
}

void Shader::load_vert(const char* s) {
	vert_shader = s;
	resolve_include(vert_shader);
}

void Shader::load_vert(const std::string& s) {
	vert_shader = s;
	resolve_include(vert_shader);
}

void Shader::load_geom(const char* s) {
	geom_shader = s;
	resolve_include(geom_shader);
}

void Shader::load_geom(const std::string& s) {
	geom_shader = s;
	resolve_include(geom_shader);
}

void Shader::load_frag(const char* s) {
	frag_shader = s;
	resolve_include(frag_shader);
}

void Shader::load_frag(const std::string& s) {
	frag_shader = s;
	resolve_include(frag_shader);
}

void Shader::load(const char* v, const char* f) {
	load_vert(v);
	load_frag(f);
}

void Shader::load(const char* v, const char* f, const char* g) {
	load_vert(v);
	load_frag(f);
	load_geom(g);
}

void Shader::load(const std::string& v, const std::string& f) {
	load_vert(v);
	load_frag(f);
}

void Shader::load(const std::string& v, const std::string& f, const std::string& g) {
	load_vert(v);
	load_frag(f);
	load_geom(g);
}

int Shader::load_vert_file(const std::string& s) {
	int return_value = read_file(s, vert_shader);
	resolve_include(vert_shader);
	return return_value;
}

int Shader::load_geom_file(const std::string& s) {
	int return_value = read_file(s, geom_shader);
	resolve_include(geom_shader);
	return return_value;
}

int Shader::load_frag_file(const std::string& s) {
	int return_value = read_file(s, frag_shader);
	resolve_include(frag_shader);
	return return_value;
}

int Shader::load_files(const std::string& v, const std::string& f) {
	int return_value = 0;
	return_value |= load_vert_file(v);
	return_value |= load_frag_file(f);
	return return_value;
}

int Shader::load_files(const std::string& v, const std::string& f, const std::string& g) {
	int return_value = 0;
	return_value |= load_vert_file(v);
	return_value |= load_frag_file(f);
	return_value |= load_geom_file(g);
	return return_value;
}

void Shader::compile() {
	if (defines == recent_defines) return;
	recent_defines = defines;
	compile_shaders();
}

void Shader::use_program() const {
	glUseProgram(program);
}

void Shader::set_define(const Defines& d) {
	defines = d.get();
}

void Shader::set_uniform_i(const std::string& n, int v) const {
	glUniform1i(glGetUniformLocation(program, n.c_str()), v);
}

void Shader::set_uniform_u(const std::string& n, unsigned int v) const {
	glUniform1ui(glGetUniformLocation(program, n.c_str()), v);
}

void Shader::set_uniform_f(const std::string& n, float v) const {
	glUniform1f(glGetUniformLocation(program, n.c_str()), v);
}

void Shader::set_uniform_v2(const std::string& n, const Vec2& v) const {
	glUniform2fv(glGetUniformLocation(program, n.c_str()), 1, &v.x);
}

void Shader::set_uniform_v3(const std::string& n, const Vec3& v) const {
	glUniform3fv(glGetUniformLocation(program, n.c_str()), 1, &v.x);
}

void Shader::set_uniform_v4(const std::string& n, const Vec4& v) const {
	glUniform4fv(glGetUniformLocation(program, n.c_str()), 1, &v.x);
}

void Shader::set_uniform_m2(const std::string& n, const Mat2& v) const {
	glUniformMatrix2fv(glGetUniformLocation(program, n.c_str()), 1, GL_TRUE, v[0]);
}

void Shader::set_uniform_m3(const std::string& n, const Mat3& v) const {
	glUniformMatrix3fv(glGetUniformLocation(program, n.c_str()), 1, GL_TRUE, v[0]);
}

void Shader::set_uniform_m4(const std::string& n, const Mat4& v) const {
	glUniformMatrix4fv(glGetUniformLocation(program, n.c_str()), 1, GL_TRUE, v[0]);
}

void Shader::set_uniforms(const Uniforms& u) const {
	auto uniforms_begin = u.vars.begin();
	auto uniforms_end = u.vars.end();
	for (auto i = uniforms_begin; i != uniforms_end; ++i) {
		
		/* analyse the name and suffix of variable */
		const std::string& name_with_suffix = i->first;
		const void* value = i->second;
		size_t last_index = name_with_suffix.rfind('_');
		if (last_index == -1) {
			set_error("Shader: Variable suffix not found");
			continue;
		}
		std::string name = name_with_suffix.substr(0, last_index);
		std::string suffix = name_with_suffix.substr(last_index + 1);
		
		/* convert variable into int data type */
		if (suffix == "i") {
			set_uniform_i(name, *static_cast<const int*>(value));
		}
		
		/* convert variable into unsigned int data type */
		else if (suffix == "u") {
			set_uniform_u(name, *static_cast<const unsigned int*>(value));
		}
		
		/* convert variable into float data type */
		else if (suffix == "f") {
			set_uniform_f(name, *static_cast<const float*>(value));
		}
		
		/* convert variable into Vec2 data type */
		else if (suffix == "v2") {
			set_uniform_v2(name, *static_cast<const Vec2*>(value));
		}
		
		/* convert variable into Vec3 data type */
		else if (suffix == "v3") {
			set_uniform_v3(name, *static_cast<const Vec3*>(value));
		}
		
		/* convert variable into Vec4 data type */
		else if (suffix == "v4") {
			set_uniform_v4(name, *static_cast<const Vec4*>(value));
		}
		
		/* convert variable into Mat2 data type */
		else if (suffix == "m2") {
			set_uniform_m2(name, *static_cast<const Mat2*>(value));
		}
		
		/* convert variable into Mat3 data type */
		else if (suffix == "m3") {
			set_uniform_m3(name, *static_cast<const Mat3*>(value));
		}
		
		/* convert variable into Mat4 data type */
		else if (suffix == "m4") {
			set_uniform_m4(name, *static_cast<const Mat4*>(value));
		}
		
		/* unknown variable suffix */
		else {
			set_error("Shader: Unknown variable suffix");
		}
	}
}

uint32_t Shader::compile_shader(const std::string& s, int32_t t) const {
	std::string shader_string = s;
	resolve_defines(shader_string);
	resolve_version(shader_string);
	uint32_t shader_id = glCreateShader(t);
	const char* shader_str = shader_string.c_str();
	glShaderSource(shader_id, 1, &shader_str, nullptr);
	glCompileShader(shader_id);
	std::string info = get_compile_info(shader_id, t);
	if (!info.empty()) {
		std::cerr << get_error_info(info, shader_string);
	}
	glAttachShader(program, shader_id);
	return shader_id;
}

void Shader::compile_shaders() const {
	/* determines whether shaders is provided */
	bool has_vert = !vert_shader.empty();
	bool has_geom = !geom_shader.empty();
	bool has_frag = !frag_shader.empty();
	if (!has_vert) {
		return void(set_error("Shader: Vertex shader is missing"));
	}
	if (!has_frag) {
		return void(set_error("Shader: Fragment shader is missing"));
	}
	
	/* compile shaders */
	uint32_t vert_id = 0;
	/* has_vert */ {
		vert_id = compile_shader(vert_shader, GL_VERTEX_SHADER);
	}
	uint32_t geom_id = 0;
	if (has_geom) {
		geom_id = compile_shader(geom_shader, GL_GEOMETRY_SHADER);
	}
	uint32_t frag_id = 0;
	/* has_frag */ {
		frag_id = compile_shader(frag_shader, GL_FRAGMENT_SHADER);
	}
	
	/* link shaders to program */
	glLinkProgram(program);
	std::string info = get_link_info();
	if (!info.empty()) std::cerr << info;
	
	/* delete shaders */
	/* has_vert */ glDeleteShader(vert_id);
	if (has_geom)  glDeleteShader(geom_id);
	/* has_frag */ glDeleteShader(frag_id);
}

std::string Shader::get_compile_info(uint32_t s, uint32_t t) const {
	int32_t success;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (success) return "";
	std::string info(1024, '\0');
	glGetShaderInfoLog(s, 1024, nullptr, info.data());
	if (t == GL_VERTEX_SHADER) {
		info = "Shader: Vertex shader compile error\n" + info;
	} else if (t == GL_GEOMETRY_SHADER) {
		info = "Shader: Geometry shader compile error\n" + info;
	} else if (t == GL_FRAGMENT_SHADER) {
		info = "Shader: Fragment shader compile error\n" + info;
	}
	return info;
}

std::string Shader::get_link_info() const {
	int32_t success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success) return "";
	std::string info(1024, '\0');
	glGetProgramInfoLog(program, 1024, nullptr, info.data());
	return "Shader: Link error\n" + info;
}

std::string Shader::get_error_info(const std::string& c, const std::string& s) {
	std::string info;
	int error_sum = 0;
	size_t line_begin = 0;
	size_t line_end = -1;
	
	/* search include directives by line */
	while (true) {
		/* get the string of every line */
		line_begin = line_end + 1;
		line_end = c.find('\n', line_begin);
		if (line_end == -1) break;
		std::string line = c.substr(line_begin, line_end - line_begin);
		info += line + '\n';
		if (line.substr(0, 6) != "ERROR:") continue;
		
		/* get the line number from error information */
		size_t number_begin = line.find(':', 7) + 1;
		size_t number_end = c.find(':', number_begin);
		int line_number = std::stoi(line.substr(number_begin, number_end - number_begin));
		
		/* search the code where the error occurred */
		size_t error_begin = 0;
		size_t error_end = -1;
		for (int i = 0; i < line_number; ++i) {
			error_begin = error_end + 1;
			error_end = s.find('\n', error_begin);
			if (error_end == -1) break;
		}
		std::string error_line = s.substr(error_begin, error_end - error_begin);
		
		/* add code line to error information */
		info += error_line + '\n';
		info += '\n';
		++error_sum;
	}
	
	/* count how many the errors occurred */
	info += std::to_string(error_sum);
	info += error_sum == 1 ? " error generated.\n" : " errors generated.\n";
	
	/* return error infomation */
	return info;
}

void Shader::resolve_include(std::string& s) {
	int include_times = 0;
	size_t line_begin = 0;
	size_t line_end = -1;
	
	/* search include directives by line */
	while (true) {
		/* get the string of every line */
		line_begin = line_end + 1;
		line_end = s.find('\n', line_begin);
		if (line_end == -1) break;
		std::string line = s.substr(line_begin, line_end - line_begin);
		
		/* search for include name */
		size_t char_1 = line.find_first_not_of(" \t");
		if (char_1 == -1 || line[char_1] != '#') continue;
		size_t char_2 = line.find_first_not_of(" \t", char_1 + 1);
		if (char_2 == -1 || line.substr(char_2, 7) != "include") continue;
		size_t char_3 = line.find_first_not_of(" \t", char_2 + 7);
		if (char_3 == -1 || line[char_3] != '<') continue;
		size_t char_4 = line.find('>', char_3 + 1);
		if (char_4 == -1) continue;
		std::string include = line.substr(char_3 + 1, char_4 - char_3 - 1);
		
		/* read included file into content */
		std::string content;
		int success = read_file(include_path + include + ".glsl", content);
		if (success == -1) {
			return void(set_error("Shader: Included file does not exist"));
		}
		
		/* replace content */
		s.replace(line_begin, line_end - line_begin, content);
		line_end = line_begin - 1;
		
		/* check if there is circular include */
		if (include_times++ == /* max include times */ 1024) {
			return void(set_error("Shader: Circular include"));
		}
	}
}

void Shader::resolve_defines(std::string& s) const {
	s = defines + s;
}

void Shader::resolve_version(std::string& s) {
	s = "#version " + glsl_version + "\n" + s;
}

void Shader::set_include_path(const std::string& p) {
	include_path = p;
}

std::string Shader::include_path = "shaders/inc/";
std::string Shader::glsl_version = "410";

BufferObject::BufferObject() {
	glGenBuffers(1, &id);
}
   
BufferObject::~BufferObject() {
	glDeleteBuffers(1, &id);
}

VertexObject::VertexObject() {
	glGenVertexArrays(1, &id);
}
   
VertexObject::~VertexObject() {
	glDeleteVertexArrays(1, &id);
}

void VertexObject::load(const Mesh& m, unsigned int g) {
	auto& vertex = m.vertex;
	auto& normal = m.normal;
	auto& uv = m.uv;
	auto& tangent = m.tangent;
	bool has_normal = !normal.empty();
	bool has_uv = !uv.empty();
	bool has_tangent = !tangent.empty();
	auto& material_reference = m.groups[g];
	std::vector<float> data;
	length = material_reference.length;
	/* has_vertex */ {
		names = {"vertex"};
		sizes = {3};
		locations = {0, 3};
	}
	if (has_normal) {
		names.emplace_back("normal");
		sizes.emplace_back(3);
		locations.emplace_back(locations.back() + 3);
	}
	if (has_uv) {
		names.emplace_back("uv");
		sizes.emplace_back(2);
		locations.emplace_back(locations.back() + 2);
	}
	if (has_tangent) {
		names.emplace_back("tangent");
		sizes.emplace_back(4);
		locations.emplace_back(locations.back() + 4);
	}
	int stride = locations.back();
	data.resize(length * stride);
	auto* data_ptr = data.data();
	for (int i = 0; i < length; ++i) {
		int position = material_reference.position + i;
		/* has_vertex */ {
			std::copy_n(&vertex[position].x, 3, data_ptr);
			data_ptr += 3;
		}
		if (has_normal) {
			std::copy_n(&normal[position].x, 3, data_ptr);
			data_ptr += 3;
		}
		if (has_uv) {
			std::copy_n(&uv[position].x, 2, data_ptr);
			data_ptr += 2;
		}
		if (has_tangent) {
			std::copy_n(&tangent[position].x, 4, data_ptr);
			data_ptr += 4;
		}
	}
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
	for (int i = 0; i < sizes.size(); ++i) {
		GLvoid* pointer = reinterpret_cast<void*>(sizeof(float) * locations[i]);
		glVertexAttribPointer(i, sizes[i], GL_FLOAT, GL_FALSE, sizeof(float) * stride, pointer);
		glEnableVertexAttribArray(i);
	}
}

void VertexObject::attach(const Shader& s) const {
	int stride = locations.back();
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
	for (int i = 0; i < sizes.size(); ++i) {
		int32_t attrib = glGetAttribLocation(s.program, names[i].c_str());
		if (attrib == -1) continue;
		GLvoid* pointer = reinterpret_cast<void*>(sizeof(float) * locations[i]);
		glVertexAttribPointer(attrib, sizes[i], GL_FLOAT, GL_FALSE, sizeof(float) * stride, pointer);
		glEnableVertexAttribArray(attrib);
	}
}

void VertexObject::draw() const {
	glBindVertexArray(id);
	glDrawArrays(GL_TRIANGLES, 0, length);
}

Texture::Texture() {
	glGenTextures(1, &id);
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void Texture::init_1d(int w, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_1D, id);
	glTexImage1D(GL_TEXTURE_1D, 0, internal, w, 0, external, data, nullptr);
	set_parameters(TEXTURE_1D, f, w, 0, 0);
}

void Texture::init_2d(int w, int h, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, external, data, nullptr);
	set_parameters(TEXTURE_2D, f, w, h, 0);
}

void Texture::init_2d(const Image& i, int f) {
	int type = i.bytes == 1 ? IMAGE_UBYTE : IMAGE_FLOAT;
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[type];
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, i.width, i.height, 0, external, data, i.data.data());
	set_parameters(TEXTURE_2D, f, i.width, i.height, 0);
}

void Texture::init_3d(int w, int h, int d, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_3D, id);
	glTexImage3D(GL_TEXTURE_3D, 0, internal, w, h, d, 0, external, data, nullptr);
	set_parameters(TEXTURE_3D, f, w, h, d);
}

void Texture::init_cube(int w, int h, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (int f = 0; f < 6; ++f) {
		uint32_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + f;
		glTexImage2D(target, 0, internal, w, h, 0, external, data, nullptr);
	}
	set_parameters(TEXTURE_CUBE, f, w, h, 0);
}

void Texture::init_cube(const Image* i, int f) {
	int type = i[0].bytes == 1 ? IMAGE_UBYTE : IMAGE_FLOAT;
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[type];
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (int f = 0; f < 6; ++f) {
		uint32_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + f;
		glTexImage2D(target, 0, internal, i[f].width, i[f].height, 0, external, data, i[f].data.data());
	}
	set_parameters(TEXTURE_CUBE, f, width, height, 0);
}

void Texture::init_1d_array(int w, int l, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_1D_ARRAY, id);
	glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, internal, w, l, 0, external, data, nullptr);
	set_parameters(TEXTURE_1D_ARRAY, f, w, l, 0);
}

void Texture::init_2d_array(int w, int h, int l, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internal, w, h, l, 0, external, data, nullptr);
	set_parameters(TEXTURE_2D_ARRAY, f, w, h, l);
}

void Texture::init_cube_array(int w, int h, int l, int f, int t) {
	uint32_t external = GL_TEXTURE_FORMATS[f].first;
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	uint32_t data = GL_IMAGE_TYPES[t];
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, id);
	for (int i = 0; i < l * 6; ++i) {
		glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, internal, w, h, i, 0, external, data, nullptr);
	}
	set_parameters(TEXTURE_CUBE_ARRAY, f, w, h, l);
}

int Texture::get_type() const {
	return type;
}

int Texture::get_format() const {
	return format;
}

int Texture::get_width() const {
	return width;
}

int Texture::get_height() const {
	return height;
}

int Texture::get_depth() const {
	return depth;
}

int Texture::get_layer() const {
	/* texture 1D (width) */
	if (type == TEXTURE_1D_ARRAY) {
		return height;
	}
	
	/* texture 2D (width x height) */
	if (type == TEXTURE_2D_ARRAY) {
		return depth;
	}
	
	/* texture cube (width x height) */
	if (type == TEXTURE_CUBE_ARRAY) {
		return depth;
	}
	
	/* other textures */
	return set_error("Texture: Cannot get layer from an non-array texture");
}

Image Texture::get_image() const {
	/* check whether the texture is 2D */
	if (type != TEXTURE_2D) {
		set_error("Texture: Cannot get image from an non-2D texture");
		return Image();
	}
	
	/* get the external format of texture */
	uint32_t external = GL_TEXTURE_FORMATS[format].first;
	
	/* create a new image */
	Image image;
	if (external == GL_RED) {
		image = Image(width, height, 1);
	} else if (external == GL_RG) {
		image = Image(width, height, 2);
	} else if (external == GL_RGB) {
		image = Image(width, height, 3);
	} else if (external == GL_RGBA) {
		image = Image(width, height, 4);
	} else {
		image = Image(width, height, 1, 4);
	}
	
	/* get image data from GPU */
	glGetTexImage(GL_TEXTURE_2D, 0, external, GL_UNSIGNED_BYTE, image.data.data());
	
	/* return the image */
	return image;
}

void Texture::set_wrap_s(int m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_wrap_t(int m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_wrap_r(int m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_wrap_all(int m) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAPPINGS[m]);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAPPINGS[m]);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, GL_TEXTURE_WRAPPINGS[m]);
}

void Texture::set_filters(int mag, int min) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glBindTexture(gl_type, id);
	glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_FILTERS[mag]);
	glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_FILTERS[min]);
}

void Texture::generate_mipmap() const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glGenerateMipmap(gl_type);
}

void Texture::set_border_color(const Vec4& c) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glTexParameterfv(gl_type, GL_TEXTURE_BORDER_COLOR, &c.x);
}

int Texture::activate(int l) const {
	uint32_t gl_type = GL_TEXTURE_TYPES[type];
	glActiveTexture(GL_TEXTURE0 + l);
	glBindTexture(gl_type, id);
	return l;
}

void Texture::set_parameters(int t, int f, int w, int h, int d) {
	type = t;
	format = f;
	width = w;
	height = h;
	depth = d;
}

int Texture::default_format(const Image& i) {
	return TEXTURE_DEFAULT_FORMATS[i.channel - 1][i.bytes == 4];
}

int Texture::default_format(int c, int b) {
	return TEXTURE_DEFAULT_FORMATS[c - 1][b == 4];
}

RenderBuffer::RenderBuffer() {
	glGenRenderbuffers(1, &id);
}

RenderBuffer::~RenderBuffer() {
	glDeleteRenderbuffers(1, &id);
}

void RenderBuffer::init(int w, int h, int f) const {
	int32_t internal = GL_TEXTURE_FORMATS[f].second;
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, internal, w, h);
}

FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &id);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &id);
}

void FrameBuffer::activate() const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		set_error("FrameBuffer: FrameBuffer is not complete");
	}
}

void FrameBuffer::activate(const FrameBuffer* f) {
	uint32_t id = f == nullptr ? 0 : f->id;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		set_error("FrameBuffer: FrameBuffer is not complete");
	}
}

void FrameBuffer::draw_attachments(const std::initializer_list<int>& l) const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	std::vector<uint32_t> buffers;
	for (auto& i : l) {
		buffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(static_cast<int>(buffers.size()), buffers.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::disable_draw() const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::set_attachment(const Texture& t, unsigned int i, int l, int p) const {
	set_texture_attachment(t, GL_COLOR_ATTACHMENT0 + i, l, p);
}

void FrameBuffer::set_attachment(const RenderBuffer& r, unsigned int i) const {
	set_render_buffer_attachment(r, GL_COLOR_ATTACHMENT0 + i);
}

void FrameBuffer::set_depth_attachment(const Texture& t, int l, int p) const {
	set_texture_attachment(t, GL_DEPTH_ATTACHMENT, l, p);
}

void FrameBuffer::set_depth_attachment(const RenderBuffer& r) const {
	set_render_buffer_attachment(r, GL_DEPTH_ATTACHMENT);
}

void FrameBuffer::set_depth_stencil_attachment(const Texture& t, int l, int p) const {
	set_texture_attachment(t, GL_DEPTH_STENCIL_ATTACHMENT, l, p);
}

void FrameBuffer::set_depth_stencil_attachment(const RenderBuffer& r) const {
	set_render_buffer_attachment(r, GL_DEPTH_STENCIL_ATTACHMENT);
}

void FrameBuffer::set_texture_attachment(const Texture& t, uint32_t a, int l, int p) const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (t.type == TEXTURE_2D) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, a, GL_TEXTURE_2D, t.id, l);
	} else if (t.type == TEXTURE_CUBE) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, a, GL_TEXTURE_CUBE_MAP_POSITIVE_X + p, t.id, l);
	} else if (t.type == TEXTURE_3D) {
		glFramebufferTexture3D(GL_FRAMEBUFFER, a, GL_TEXTURE_3D, t.id, l, p);
	} else if (t.type == TEXTURE_2D_ARRAY) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER, a, t.id, l, p);
	} else if (t.type == TEXTURE_CUBE_ARRAY) {
		glFramebufferTextureLayer(GL_FRAMEBUFFER, a, t.id, l, p);
	} else {
		set_error("FrameBuffer: Texture type is not supported");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::set_render_buffer_attachment(const RenderBuffer& r, uint32_t a) const {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, a, GL_RENDERBUFFER, r.id);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
