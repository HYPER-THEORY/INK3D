#include <common>

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model_view;
uniform mat4 model_view_proj;
uniform mat3 normal_mat;
uniform vec3 camera_pos;

#ifdef USE_DISPLACEMENT_MAP
uniform sampler2D displacement_map;
uniform float displacement_scale;
#endif

in vec3 vertex;
in vec3 normal;
in vec2 uv;

#ifdef USE_TANGENT
in vec4 tangent;
#endif

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_world_pos;

#ifdef USE_TANGENT
out vec3 v_tangent;
out vec3 v_bitangent;
#endif

void main() {
	vec3 t_vertex = vertex;
	vec3 t_normal = normal;
	vec2 t_uv = uv;
	#ifdef USE_TANGENT
		vec3 t_tangent = tangent.xyz;
		vec3 t_bitangent;
	#endif
	
	#ifdef USE_DISPLACEMENT_MAP
		/* reposition the vertex */
		t_vertex += t_normal * texture(displacement_map, t_uv).x * displacement_scale;
	#endif
	
	/* transform normal from object space to world space */
	t_normal = normalize(normal_mat * t_normal);
	#ifdef USE_TANGENT
		/* transform tangent from object space to world space */
		t_tangent = normalize((model * vec4(t_tangent, 0.)).xyz);
		
		/* calculate bitangent with normal and tangent */
		t_bitangent = normalize(cross(t_normal, t_tangent) * tangent.w);
	#endif
	
	/* pass to fragment shader */
	v_normal = t_normal;
	v_uv = t_uv;
	v_world_pos = (model * vec4(t_vertex, 1.)).xyz;
	#ifdef USE_TANGENT
		v_tangent = t_tangent;
		v_bitangent = t_bitangent;
	#endif
	gl_Position = model_view_proj * vec4(t_vertex, 1.);
}