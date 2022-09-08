#version 410 core

#pragma once

#define PI			3.14159265358979323846264338327950288
#define HALF_PI		1.57079632679489661923132169163975144
#define ONE_BY_PI	0.318309886183790671537767526745028724
#define EPS			0.000001

/* Returns a pseudo-random value in the range from 0 to 1. */
highp float rand(const in vec2 s) {
	const highp float a = 12.9898;
	const highp float b = 78.233;
	const highp float c = 43758.5453;
	highp float dt = dot(s, vec2(a, b));
	highp float sn = mod(dt, PI);
	return fract(sin(sn) * c);
}

/* Returns a pseudo-random value in the range from 0 to 1. */
highp float rand(const in vec3 s) {
	const highp float a = 12.9898;
	const highp float b = 78.233;
	const highp float c = 45.164;
	const highp float d = 43758.5453;
	highp float dt = dot(s, vec3(a, b, c));
	highp float sn = mod(dt, PI);
	return fract(sin(sn) * d);
}

/* Transform dirction vector. */
vec3 transform_dir(const in vec3 dir, const in mat4 mat) {
	return normalize((mat * vec4(dir, 0.)).xyz);
}

/* Transform dirction vector inversely. */
vec3 inv_transform_dir(const in vec3 dir, const in mat4 inv_mat) {
	return normalize((vec4(dir, 0.) * inv_mat).xyz);
}

/* Return linearized depth. */
float linearize_depth(const in mat4 proj, const in float depth) {
	return proj[3][2] / (proj[2][2] + depth * 2. - 1.);
}

/* Convert color to relative luminance. */
float relative_luminance(const in vec3 c) {
	const vec3 weights = vec3(0.2126, 0.7152, 0.0722);
	return dot(weights, c);
}

bool out_of(const in vec2 v) {
	return any(lessThan(v, vec2(0.))) || any(greaterThan(v, vec2(1.)));
}
