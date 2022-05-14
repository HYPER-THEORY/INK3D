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

#include "viewer.h"

viewer::viewer(const perspcamera& c, int m, float s) :
	view_camera(c), mode(m), speed(s) {};
	
void viewer::key_mapping(int u, int d, int l, int r) {
	key_up = u;
	key_down = d;
	key_left = l;
	key_right = r;
}

perspcamera& viewer::update() {
	/* keyborad event */
	vec3 move;
	if (window::is_down(key_up)) ++move.z;
	if (window::is_down(key_down)) --move.z;
	if (window::is_down(key_left)) --move.x;
	if (window::is_down(key_right)) ++move.x;
	if (move.magnitude() != 0) move = move.normalize() * speed;
	/* camera perspective */
	axisy -= (window::get_cursor_x() - window::get_width() / 2) * sensitivity;
	axisz -= (window::get_cursor_y() - window::get_height() / 2) * sensitivity;
	if (mode == VIEWER_WALK) {
		if (axisz > M_PI_2) axisz = M_PI_2;
		if (axisz < -M_PI_2) axisz = -M_PI_2;
	}
	view_camera.direction = {sinf(axisy) * cosf(axisz), sinf(axisz), cosf(axisy) * cosf(axisz)};
	view_camera.up = {-sinf(axisy) * sinf(axisz), cosf(axisz), -cosf(axisy) * sinf(axisz)};
	/* camera movement */
	if (mode == VIEWER_WALK) {
		view_camera.position.x += move.z * sinf(axisy) - move.x * cosf(axisy);
		view_camera.position.z += move.z * cosf(axisy) + move.x * sinf(axisy);
	} else if (mode == VIEWER_FLY) {
		view_camera.position += move.z * view_camera.direction;
		view_camera.position += move.x * (view_camera.direction ^ view_camera.up);
	}
	view_camera.lookat(view_camera.position, -view_camera.direction, view_camera.up);
	return view_camera;
}
