/*
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

#include "material.h"

material::material(const std::string& n) {
	name = n;
}

int material::type() const {
	return MATERIAL_DEFAULT;
}

const void* material::get_shader() const {
	return linked_shader;
}

void material::set_shader(const void* s) {
	linked_shader = s;
}

size_t material::image_size() const {
	return linked_images.size();
}

void material::clear_image() {
	linked_images.clear();
}

const image** material::get_images() {
	return linked_images.data();
}

const image* const* material::get_images() const {
	return linked_images.data();
}

void material::add_image(const image* i) {
	linked_images.push_back(i);
}

void material::add_images(const std::initializer_list<const image*>& i) {
	linked_images.insert(linked_images.end(), i);
}
