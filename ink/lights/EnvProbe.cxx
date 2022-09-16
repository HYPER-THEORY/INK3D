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

#include "EnvProbe.h"

namespace Ink {

EnvProbe::EnvProbe(float i, int r) :
intensity(i), resolution(r) {}

void EnvProbe::load_cubemap(const Image& px, const Image& nx, const Image& py,
							const Image& ny, const Image& pz, const Image& nz) {
	env_map = std::make_unique<Gpu::Texture>();
	IBLPrefilter::load_cubemap(px, nx, py, ny, pz, nz, *env_map, resolution);
}

void EnvProbe::load_equirect(const Image& i) {
	env_map = std::make_unique<Gpu::Texture>();
	IBLPrefilter::load_equirect(i, *env_map, resolution);
}

void EnvProbe::load_texture(const Gpu::Texture& t) {
	env_map = std::make_unique<Gpu::Texture>();
	IBLPrefilter::load_texture(t, *env_map, resolution);
}

int EnvProbe::activate(int l) const {
	return env_map->activate(l);
}

}
