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

#include "convexhull.h"

void convexhull::add_vertex(const vec3& v) {
	vertexs.push_back(v);
}

void convexhull::add_vertices(const std::initializer_list<vec3>& v) {
	vertexs.insert(vertexs.end(), v);
}

void convexhull::compute() {
	insert_face(0, 1, 2);
	insert_face(2, 1, 0);
	size_t size = vertexs.size();
	for (int i = 3; i < size; ++i) {
		std::unordered_set<long long> new_faces;
		auto face_iter = faces.begin();
		auto normal_iter = normals.begin();
		while (face_iter != faces.end()) {
			vec3 dir = vertexs[i] - vertexs[(*face_iter)[0]];
			if (*normal_iter * dir <= 0) {
				++face_iter;
				++normal_iter;
				continue;
			}
			for (int k = 0; k < 3; ++k) {
				long long u = (*face_iter)[k];
				long long v = (*face_iter)[(k + 1) % 3];
				long long vu = v << 32 | u;
				if (new_faces.count(vu) != 0) {
				   new_faces.erase(vu);
				} else {
				   new_faces.insert(u << 32 | v);
				}
			}
			face_iter = faces.erase(face_iter);
			normal_iter = normals.erase(normal_iter);
		}
		for (auto& f : new_faces) {
			insert_face(i, f >> 32, f & 0xffffffffll);
		}
	}
}

void convexhull::insert_face(int a, int b, int c) {
	faces.push_back({a, b, c});
	normals.push_back((vertexs[b] - vertexs[a]) ^ (vertexs[c] - vertexs[a]));
}
