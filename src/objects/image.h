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

#pragma once

#include <algorithm>
#include <string>
#include <vector>

class image {
public:
	int width;						/* width of image */
	int height;						/* height of image */
	int bytes;						/* bytes of image */
	std::vector<uint8_t> data;		/* data of image */
	
	/**
	 * Create a new image.
	 */
	image() = default;
	
	/**
	 * Returns a new image sliced from the original image.
	 *
	 * \param i new image
	 * \param x x-coordinate of original image
	 * \param y y-coordinate of original image
	 * \param w width of new image
	 * \param h height of new image
	 */
	void slice(image& i, int x, int y, int w, int h);
	
	/**
	 * Filp image vertically.
	 */
	void flip_vertical();
	
	/**
	 * Filp image horizontally.
	 */
	void flip_horizontal();
	
	/**
	 * Swap red and blue color.
	 */
	void swap_rb();
};
