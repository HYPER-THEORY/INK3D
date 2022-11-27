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

#include "Software.h"

namespace WR {

namespace Soft {

Vec3 nearest_map(const Image& t, float u, float v) {
	int x = u * t.width;
	x = x < 0 ? 0 : x;
	x = x >= t.width ? t.width - 1 : x;
	int y = v * t.height;
	y = y < 0 ? 0 : y;
	y = y >= t.height ? t.height - 1 : y;
	const uint8_t* pointer = t.data.data() + (x + y * t.width) * t.bytes;
	return {pointer[0] / 255.f, pointer[1] / 255.f, pointer[2] / 255.f};
}

Vec3 nearest_map(const Image& t, const Vec2& uv) {
	return nearest_map(t, uv.x, uv.y);
}

Vec3 linear_map(const Image& t, float u, float v) {
	int x0 = u * t.width;
	x0 = x0 < 0 ? 0 : x0;
	x0 = x0 >= t.width ? t.width - 1 : x0;
	int x1 = x0 + 1;
	x1 = x1 >= t.width ? t.width - 1 : x1;
	int y0 = v * t.height;
	y0 = y0 < 0 ? 0 : y0;
	y0 = y0 >= t.height ? t.height - 1 : y0;
	int y1 = y0 + 1;
	y1 = y1 >= t.height ? t.height - 1 : y1;
	const uint8_t* pointer00 = t.data.data() + (x0 + y0 * t.width) * t.bytes;
	const uint8_t* pointer01 = t.data.data() + (x0 + y1 * t.width) * t.bytes;
	const uint8_t* pointer10 = t.data.data() + (x1 + y0 * t.width) * t.bytes;
	const uint8_t* pointer11 = t.data.data() + (x1 + y1 * t.width) * t.bytes;
	Vec3 color00 = {pointer00[0] / 255.f, pointer00[1] / 255.f, pointer00[2] / 255.f};
	Vec3 color01 = {pointer01[0] / 255.f, pointer01[1] / 255.f, pointer01[2] / 255.f};
	Vec3 color10 = {pointer10[0] / 255.f, pointer10[1] / 255.f, pointer10[2] / 255.f};
	Vec3 color11 = {pointer11[0] / 255.f, pointer11[1] / 255.f, pointer11[2] / 255.f};
	return (color00 * (y1 - v) + color01 * (v - y0)) * (x1 - u) + (color10 * (y1 - v) + color11 * (v - y0)) * (u - x0);
}

Vec3 linear_map(const Image& t, const Vec2& uv) {
	return linear_map(t, uv.x, uv.y);
}

void set_viewport(int x, int y, int w, int h) {
	viewport_x = x;
	viewport_y = y;
	viewport_width = w;
	viewport_height = h;
}

void add_pointlist(const Vec4& v, const Vec3& b, PointList& l) {
	l.vertices[l.size] = v;
	l.barycenters[l.size] = b;
	++l.size;
}

void znear_clip(const PointList& i, float zn, PointList& o) {
	for (int l = 0; l < i.size; ++l) {
		Vec4& vertex1 = i.vertices[l];
		Vec4& vertex2 = i.vertices[(l + 1) % i.size];
		/* both outside */
		if (vertex1.w < zn && vertex2.w < zn) continue;
		/* both inside */
		const Vec3& barycenter1 = i.barycenters[l];
		const Vec3& barycenter2 = i.barycenters[(l + 1) % i.size];
		if (vertex1.w > zn && vertex2.w > zn) {
			add_pointlist(vertex2, barycenter2, o);
			continue;
		}
		/* from inside to outside */
		float weight1 = fabsf(vertex1.w - zn);
		float weight2 = fabsf(vertex2.w - zn);
		float inverse = 1 / (weight1 + weight2);
		if (vertex1.w > zn && vertex2.w < zn) {
			add_pointlist((vertex1 * weight2 + vertex2 * weight1) * inverse,
						  (barycenter1 * weight2 + barycenter2 * weight1) * inverse, o);
			continue;
		}
		/* from outside to inside */
		add_pointlist((vertex1 * weight2 + vertex2 * weight1) * inverse,
					  (barycenter1 * weight2 + barycenter2 * weight1) * inverse, o);
		add_pointlist(vertex2, barycenter2, o);
	}
}

void zfar_clip(const PointList& i, float zf, PointList& o) {
	for (int l = 0; l < i.size; ++l) {
		Vec4& vertex1 = i.vertices[l];
		Vec4& vertex2 = i.vertices[(l + 1) % i.size];
		/* both outside */
		if (vertex1.w > zf && vertex2.w > zf) continue;
		/* both inside */
		const Vec3& barycenter1 = i.barycenters[l];
		const Vec3& barycenter2 = i.barycenters[(l + 1) % i.size];
		if (vertex1.w < zf && vertex2.w < zf) {
			add_pointlist(vertex2, barycenter2, o);
			continue;
		}
		/* from inside to outside */
		float weight1 = fabsf(vertex1.w - zf);
		float weight2 = fabsf(vertex2.w - zf);
		float inverse = 1 / (weight1 + weight2);
		if (vertex1.w > zf && vertex2.w < zf) {
			add_pointlist((vertex1 * weight2 + vertex2 * weight1) * inverse,
						  (barycenter1 * weight2 + barycenter2 * weight1) * inverse, o);
			continue;
		}
		/* from outside to inside */
		add_pointlist((vertex1 * weight2 + vertex2 * weight1) * inverse,
					  (barycenter1 * weight2 + barycenter2 * weight1) * inverse, o);
		add_pointlist(vertex2, barycenter2, o);
	}
}

void rasterize(const PointList& p, const Vec3* dvs, Shader& s, Vec3* canvas, double* zb, GInfo* gb) {
	for (int i = 2; i < p.size; ++i) {
		const Vec3& vertex_a = dvs[0];
		const Vec3& vertex_b = dvs[i - 1];
		const Vec3& vertex_c = dvs[i];
		Vec3 fixed_a = {p.barycenters[0].x, p.barycenters[0].y, p.barycenters[0].z};
		Vec3 fixed_b = {p.barycenters[i - 1].x, p.barycenters[i - 1].y, p.barycenters[i - 1].z};
		Vec3 fixed_c = {p.barycenters[i].x, p.barycenters[i].y, p.barycenters[i].z};
		Vec3 fixed_barycenter = {1 / p.vertices[0].w, 1 / p.vertices[i - 1].w, 1 / p.vertices[i].w};
		Vec2d v0 = {vertex_c.x - vertex_a.x, vertex_c.y - vertex_a.y};
		Vec2d v1 = {vertex_b.x - vertex_a.x, vertex_b.y - vertex_a.y};
		Vec2d v2 = {};
		double dot00 = v0 * v0;
		double dot01 = v0 * v1;
		double dot11 = v1 * v1;
		double inverse = 1 / (dot00 * dot11 - dot01 * dot01);
		Vec3 barycenter;
		Vec4 color;
		Vec3 vertex_l = vertex_a;
		Vec3 vertex_m = vertex_b;
		Vec3 vertex_u = vertex_c;
		if (vertex_l.y > vertex_m.y) std::swap(vertex_l, vertex_m);
		if (vertex_m.y > vertex_u.y) std::swap(vertex_m, vertex_u);
		if (vertex_l.y > vertex_m.y) std::swap(vertex_l, vertex_m);
		float lower = fmaxf(floorf(vertex_l.y) + 1, 0);
		float upper = fminf(floorf(vertex_u.y) + 1, viewport_height);
		float median = vertex_m.y;
		float inverseml = 1 / (vertex_m.y - vertex_l.y);
		float inverseum = 1 / (vertex_u.y - vertex_m.y);
		float inverseul = 1 / (vertex_u.y - vertex_l.y);
		for (float y = lower; y < upper; ++y) {
			float left = y < median ?
				(vertex_l.x * (vertex_m.y - y) + vertex_m.x * (y - vertex_l.y)) * inverseml :
				(vertex_m.x * (vertex_u.y - y) + vertex_u.x * (y - vertex_m.y)) * inverseum;
			float right = (vertex_l.x * (vertex_u.y - y) + vertex_u.x * (y - vertex_l.y)) * inverseul;
			if (left > right) std::swap(left, right);
			left = fmaxf(floorf(left) + 1, 0);
			right = fminf(floorf(right) + 1, viewport_width);
			for (float x = left; x < right; ++x) {
				v2.x = x - vertex_a.x;
				v2.y = y - vertex_a.y;
				double dot02 = v0 * v2;
				double dot12 = v1 * v2;
				double u = (dot11 * dot02 - dot01 * dot12) * inverse;
				double v = (dot00 * dot12 - dot01 * dot02) * inverse;
				if (UNLIKELY(u < -eps || v < -eps || u + v > 1 + eps)) continue;
				double z = vertex_a.z * (1 - u - v) + vertex_b.z * v + vertex_c.z * u;
				int location = x + y * viewport_width;
				if (z > -1 && z < 1 && z < zb[location] + eps) {
					zb[location] = z;
					barycenter.x = (1 - u - v) * fixed_barycenter.x;
					barycenter.y = v * fixed_barycenter.y;
					barycenter.z = u * fixed_barycenter.z;
					barycenter /= barycenter.x + barycenter.y + barycenter.z;
					barycenter = fixed_a * barycenter.x + fixed_b * barycenter.y + fixed_c * barycenter.z;
					s.fragment_shader(barycenter, {x / viewport_width, y / viewport_height}, gb[location], color);
					canvas[location].x = canvas[location].x * (1 - color.w) + color.x * color.w;
					canvas[location].y = canvas[location].y * (1 - color.w) + color.y * color.w;
					canvas[location].z = canvas[location].z * (1 - color.w) + color.z * color.w;
				}
			}
		}
	}
}

template <bool write>
bool rasterize(const PointList& p, const Vec3* dvs, double* zb) {
	for (int i = 2; i < p.size; ++i) {
		const Vec3& vertex_a = dvs[0];
		const Vec3& vertex_b = dvs[i - 1];
		const Vec3& vertex_c = dvs[i];
		Vec2d v0 = {vertex_c.x - vertex_a.x, vertex_c.y - vertex_a.y};
		Vec2d v1 = {vertex_b.x - vertex_a.x, vertex_b.y - vertex_a.y};
		Vec2d v2 = {};
		double dot00 = v0 * v0;
		double dot01 = v0 * v1;
		double dot11 = v1 * v1;
		double inverse = 1 / (dot00 * dot11 - dot01 * dot01);
		Vec3 vertex_l = vertex_a;
		Vec3 vertex_m = vertex_b;
		Vec3 vertex_u = vertex_c;
		if (vertex_l.y > vertex_m.y) std::swap(vertex_l, vertex_m);
		if (vertex_m.y > vertex_u.y) std::swap(vertex_m, vertex_u);
		if (vertex_l.y > vertex_m.y) std::swap(vertex_l, vertex_m);
		float lower = fmaxf(floorf(vertex_l.y) + 1, 0);
		float upper = fminf(floorf(vertex_u.y) + 1, viewport_height);
		float median = vertex_m.y;
		float inverseml = 1 / (vertex_m.y - vertex_l.y);
		float inverseum = 1 / (vertex_u.y - vertex_m.y);
		float inverseul = 1 / (vertex_u.y - vertex_l.y);
		for (float y = lower; y < upper; ++y) {
			float left = y < median ?
				(vertex_l.x * (vertex_m.y - y) + vertex_m.x * (y - vertex_l.y)) * inverseml :
				(vertex_m.x * (vertex_u.y - y) + vertex_u.x * (y - vertex_m.y)) * inverseum;
			float right = (vertex_l.x * (vertex_u.y - y) + vertex_u.x * (y - vertex_l.y)) * inverseul;
			if (left > right) std::swap(left, right);
			left = fmaxf(floorf(left) + 1, 0);
			right = fminf(floorf(right) + 1, viewport_width);
			for (float x = left; x < right; ++x) {
				v2.x = x - vertex_a.x;
				v2.y = y - vertex_a.y;
				double dot02 = v0 * v2;
				double dot12 = v1 * v2;
				double u = (dot11 * dot02 - dot01 * dot12) * inverse;
				double v = (dot00 * dot12 - dot01 * dot02) * inverse;
				if (UNLIKELY(u < -eps || v < -eps || u + v > 1 + eps)) continue;
				double z = vertex_a.z * (1 - u - v) + vertex_b.z * v + vertex_c.z * u;
				int location = x + y * viewport_width;
				if (z > -1 && z < 1 && z < zb[location] + eps) {
					if (!write) return true;
					zb[location] = z;
				}
			}
		}
	}
	return false;
}

void draw(const Camera& c, Shader& s, const Mesh& m, Vec3* canvas, double* zb, GInfo* gb) {
	Vec3 barycenters[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	size_t length = m.vertex.size();
	for (int i = 0; i < length; i += 3) {
		Vec4 vertices[3];
		/* vertex shader */
		for (int j = 0; j < 3; ++j) {
			s.vextex_shader(m, i + j, j, vertices[j]);
		}
		/* geometry shader */
		s.geometry_shader(vertices);
		/* clipping */
		Vec4 clip_vertices[4];
		Vec3 clip_barycenters[4];
		PointList clip_primitive = {0, clip_vertices, clip_barycenters};
		znear_clip({3, vertices, barycenters}, c.znear, clip_primitive);
		Vec4 primitive_vertices[5];
		Vec3 primitive_barycenters[5];
		PointList primitive = {0, primitive_vertices, primitive_barycenters};
		zfar_clip(clip_primitive, c.zfar, primitive);
		/* perspective division */
		Vec3 device_vertices[5];
		for (int i = 0; i < primitive.size; ++i) {
			device_vertices[i].x = primitive.vertices[i].x / primitive.vertices[i].w;
			device_vertices[i].y = primitive.vertices[i].y / primitive.vertices[i].w;
			device_vertices[i].z = primitive.vertices[i].z / primitive.vertices[i].w;
		}
		/* viewport transform */
		for (int i = 0; i < primitive.size; ++i) {
			device_vertices[i].x = device_vertices[i].x * viewport_width / 2 + viewport_width / 2;
			device_vertices[i].y = -device_vertices[i].y * viewport_height / 2 + viewport_height / 2;
		}
		/* rasterization */
		rasterize(primitive, device_vertices, s, canvas, zb, gb);
	}
}

void ztest(const Camera& c, Shader& s, const Mesh& m, double* zb) {
	Vec3 barycenters[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	size_t length = m.vertex.size();
	for (int i = 0; i < length; i += 3) {
		Vec4 vertices[3];
		/* vertex shader */
		for (int j = 0; j < 3; ++j) {
			s.vextex_shader(m, i + j, j, vertices[j]);
		}
		/* geometry shader */
		s.geometry_shader(vertices);
		/* clipping */
		Vec4 clip_vertices[4];
		Vec3 clip_barycenters[4];
		PointList clip_primitive = {0, clip_vertices, clip_barycenters};
		znear_clip({3, vertices, barycenters}, c.znear, clip_primitive);
		Vec4 primitive_vertices[5];
		Vec3 primitive_barycenters[5];
		PointList primitive = {0, primitive_vertices, primitive_barycenters};
		zfar_clip(clip_primitive, c.zfar, primitive);
		/* perspective division */
		Vec3 device_vertices[5];
		for (int i = 0; i < primitive.size; ++i) {
			device_vertices[i].x = primitive.vertices[i].x / primitive.vertices[i].w;
			device_vertices[i].y = primitive.vertices[i].y / primitive.vertices[i].w;
			device_vertices[i].z = primitive.vertices[i].z / primitive.vertices[i].w;
		}
		/* viewport transform */
		for (int i = 0; i < primitive.size; ++i) {
			device_vertices[i].x = device_vertices[i].x * viewport_width / 2 + viewport_width / 2;
			device_vertices[i].y = -device_vertices[i].y * viewport_height / 2 + viewport_height / 2;
		}
		/* rasterization */
		rasterize(primitive, device_vertices, zb);
	}
}

bool zquery(const Camera& c, Shader& s, const Mesh& m, double* zb) {
	Vec3 barycenters[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	size_t length = m.vertex.size();
	for (int i = 0; i < length; i += 3) {
		Vec4 vertices[3];
		/* vertex shader */
		for (int j = 0; j < 3; ++j) {
			s.vextex_shader(m, i + j, j, vertices[j]);
		}
		/* geometry shader */
		s.geometry_shader(vertices);
		/* clipping */
		Vec4 clip_vertices[4];
		Vec3 clip_barycenters[4];
		PointList clip_primitive = {0, clip_vertices, clip_barycenters};
		znear_clip({3, vertices, barycenters}, c.znear, clip_primitive);
		Vec4 primitive_vertices[5];
		Vec3 primitive_barycenters[5];
		PointList primitive = {0, primitive_vertices, primitive_barycenters};
		zfar_clip(clip_primitive, c.zfar, primitive);
		/* perspective division */
		Vec3 device_vertices[5];
		for (int i = 0; i < primitive.size; ++i) {
			device_vertices[i].x = primitive.vertices[i].x / primitive.vertices[i].w;
			device_vertices[i].y = primitive.vertices[i].y / primitive.vertices[i].w;
			device_vertices[i].z = primitive.vertices[i].z / primitive.vertices[i].w;
		}
		/* viewport transform */
		for (int i = 0; i < primitive.size; ++i) {
			device_vertices[i].x = device_vertices[i].x * viewport_width / 2 + viewport_width / 2;
			device_vertices[i].y = -device_vertices[i].y * viewport_height / 2 + viewport_height / 2;
		}
		/* rasterization */
		if (rasterize<false>(primitive, device_vertices, zb)) return true;
	}
	return false;
}

void draw_instances(const Camera& c, Shader& s, const Instance* const* is, size_t size, Vec3* canvas) {
	size_t buffer_size = viewport_width * viewport_height;
	if (zbuffer.size() != buffer_size) zbuffer.resize(buffer_size);
	if (gbuffer.size() != buffer_size) gbuffer.resize(buffer_size);
	std::fill(zbuffer.begin(), zbuffer.end(), 1);
	Mat<4, 4> view_proj = c.projection * c.viewing;
	for (int i = 0; i < size; ++i) {
		s.model = Camera::model_transform(*is[i]);
		s.view = c.viewing;
		s.proj = c.projection;
		s.model_view_proj = view_proj * s.model;
		for (auto& g : is[i]->mesh_ptr->groups) {
			s.textures = is[i]->mesh_ptr->get_material(g.name)->get_images();
			draw(c, s, *is[i]->mesh_ptr, canvas, zbuffer.data(), gbuffer.data());
		}
	}
}

bool Light::intersect(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& o, const Vec3& d, float l) {
	Vec3 ab = b - a;
	Vec3 ac = c - a;
	Vec3 ao = o - a;
	Vec3 p = d ^ac;
	Vec3 q = ao ^ab;
	float inverse = 1 / (ab * p);
	float u = d * q * inverse;
	float v = ao * p * inverse;
	float t = ac * q * inverse;
	return t > eps && t < l && u > -eps && v > -eps && u + v < 1 + eps;
}
	
bool Light::intersect(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& o, const Vec3& d, float l, Vec3& i) {
	Vec3 ab = b - a;
	Vec3 ac = c - a;
	Vec3 ao = o - a;
	Vec3 p = d ^ac;
	Vec3 q = ao ^ab;
	float inverse = 1 / (ab * p);
	float u = d * q * inverse;
	float v = ao * p * inverse;
	float t = ac * q * inverse;
	if (t < eps || t > l || u < -eps || v < -eps || u + v > 1 + eps) return false;
	i.x = 1 - u - v;
	i.y = v;
	i.z = u;
	return true;
}

PointLight::PointLight(const Vec3& p, float i, float d) : position(p), intensity(i), decay(d) {}
	
Vec3 PointLight::lighting(const Vec3& p, const Vec3& n) const {
	float distance = position.distance(p);
	Vec3 direction = (position - p).normalize();
	return color * fmax(direction * n * intensity / (distance * distance * decay), 0);
}
	
Vec3 PointLight::cast(const Vec3& p, const Vec3& n, const Vec3* w, size_t ws) const {
	float distance = position.distance(p);
	Vec3 direction = (position - p).normalize();
	for (int f = 0; f < ws; f += 3) {
		if (intersect(w[f], w[f + 1], w[f + 2], p, direction, distance)) return {};
	}
	return color * fmax(direction * n * intensity / (distance * distance * decay), 0);
}

DirectLight::DirectLight(const Vec3& d, float i) : direction(-d), intensity(i) {}
	
Vec3 DirectLight::lighting(const Vec3& p, const Vec3& n) const {
	return color * fmax(direction * n * intensity, 0);
}

Vec3 DirectLight::cast(const Vec3& p, const Vec3& n, const Vec3* w, size_t ws) const {
	for (int f = 0; f < ws; f += 3) {
		if (intersect(w[f], w[f + 1], w[f + 2], p, direction, limit)) return {};
	}
	return color * fmax(direction * n * intensity, 0);
}

float DirectLight::limit = 1000;

AreaLight::AreaLight(const Vec3& p, const Vec3& tr, const Vec3& tl, float i) : position(p),
                                                                               toright(tr), tolower(tl), normal((tl ^ tr).normalize()), area((tl ^ tr).magnitude()), intensity(i) {}
	
bool AreaLight::emit(const Vec3& o, const Vec3& d, const Vec3* w, size_t w_size , Vec3& p, Vec3& n) {
	float minimum = limit;
	Vec3 barycenter;
	bool collided = false;
	for (int f = 0; f < w_size; f += 3) {
		if (intersect(w[f], w[f + 1], w[f + 2], o, d, limit, barycenter)) {
			Vec3 position = w[f] * barycenter.x + w[f + 1] * barycenter.y + w[f + 2] * barycenter.z;
			if (o.distance(position) < minimum) {
				minimum = o.distance(position);
				p = position;
				n = (w[f + 1] - w[f]) ^ (w[f + 2] - w[f]);
			}
			collided = true;
		}
	}
	if (collided) n = n.normalize();
	return collided;
}
	
Vec3 AreaLight::cast(const Vec3& p, const Vec3& n, const Vec3* w, size_t ws) const {
	Vec3 origin = position + toright * randomf() + tolower * randomf();
	float distance = origin.distance(p);
	Vec3 direction = (origin - p).normalize();
	for (int f = 0; f < ws; f += 3) {
		if (intersect(w[f], w[f + 1], w[f + 2], p, direction, distance)) return {};
	}
	return color * fmax(intensity * (direction * n) * -(direction * normal) * area / (distance * distance), 0);
}

float AreaLight::limit = 1000;

void lighting(const Vec3& r, Vec3& c) {
	c.x *= fminf(r.x, 1);
	c.y *= fminf(r.y, 1);
	c.z *= fminf(r.z, 1);
}

void classic_raytracing(const Light* const* ls, size_t size, const Vec3* w, size_t ws, Vec3* canvas) {
	for (int i = viewport_width * viewport_height; i --> 0;) {
		if (zbuffer[i] == 1) continue;
		Vec3 radiance;
		for (int l = 0; l < size; ++l) {
			radiance += ls[l]->cast(gbuffer[i].world_coord, gbuffer[i].normal, w, ws);
		}
		lighting(radiance, canvas[i]);
	}
}

void path_tracing(const Light* const* ls, size_t size, const Vec3* w, size_t ws, Vec3* canvas) {
	static float brdf = .2f;
	std::vector<const AreaLight*> lights(size);
	for (size_t i = size; i --> 0;) {
		lights[i] = static_cast<const AreaLight*>(ls[i]);
	}
	for (int i = viewport_width * viewport_height; i --> 0;) {
		if (zbuffer[i] == 1) continue;
		float probability = .6f;
		float coefficient = 1;
		Vec3 radiance;
		Vec3 position = gbuffer[i].world_coord;
		Vec3 normal = gbuffer[i].normal;
		while (true) {
			for (auto& l : lights) {
				radiance += l->cast(position, normal, w, ws) * coefficient;
			}
			if (probability <= randomf()) break;
			Vec3 direction = Vec3::random();
			if (direction * normal < 0) direction = -direction;
			coefficient *= direction * normal * brdf / probability;
			if (!AreaLight::emit(position + direction * eps, direction, w, ws, position, normal)) break;
		}
		lighting(radiance, canvas[i]);
	}
}

}

}
