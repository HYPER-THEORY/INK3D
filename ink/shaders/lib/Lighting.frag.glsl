#include <common>
#include <transform>
#include <lights>
#include <brdf>
#include <shadow>
#include <packing>
#include <tonemapping>

#ifdef LINEAR_TONE_MAPPING
#define TONE_MAPPING linear_tone_mapping
#endif

#ifdef REINHARD_TONE_MAPPING
#define TONE_MAPPING reinhard_tone_mapping
#endif

#ifdef OPTIMIZED_TONE_MAPPING
#define TONE_MAPPING optimized_tone_mapping
#endif

#ifdef ACES_FILMIC_TONE_MAPPING
#define TONE_MAPPING aces_filmic_tone_mapping
#endif

uniform sampler2D gbuffer_c;    /* G-Buffer color */
uniform sampler2D gbuffer_n;    /* G-Buffer normal */
uniform sampler2D gbuffer_m;    /* G-Buffer material */
uniform sampler2D gbuffer_e;    /* G-Buffer emissive color */
uniform sampler2D gbuffer_i;    /* G-Buffer indirect light */
uniform sampler2D gbuffer_d;    /* G-Buffer depth */

uniform vec3 camera_pos;
uniform mat4 inv_view_proj;
uniform float exposure;

#if NUM_POINT_LIGHT > 0
#define USE_POINT_LIGHT
uniform PointLight point_lights[NUM_POINT_LIGHT];
#endif

#if NUM_SPOT_LIGHT > 0
#define USE_SPOT_LIGHT
uniform SpotLight spot_lights[NUM_SPOT_LIGHT];
uniform Shadow spot_shadows[NUM_SPOT_LIGHT];
#endif

#if NUM_DIRECTIONAL_LIGHT > 0
#define USE_DIRECTIONAL_LIGHT
uniform DirectionalLight directional_lights[NUM_DIRECTIONAL_LIGHT];
uniform Shadow directional_shadows[NUM_DIRECTIONAL_LIGHT];
#endif

#if NUM_AMBIENT_LIGHT > 0
#define USE_AMBIENT_LIGHT
uniform AmbientLight ambient_lights[NUM_AMBIENT_LIGHT];
#endif

#if NUM_HEMISPHERE_LIGHT > 0
#define USE_HEMISPHERE_LIGHT
uniform HemisphereLight hemisphere_lights[NUM_HEMISPHERE_LIGHT];
#endif

in vec2 v_uv;

layout(location = 0) out vec4 out_color;

struct Material {
	vec3 color;
	vec3 f0;
	float roughness;
};

struct Geometry {
	vec3 position;
	vec3 view_dir;
	vec3 normal;
};

float attenuate(float light_distance, float max_distance, float decay) {
	if (max_distance > 0. && decay > 0.) {
		return pow(saturate(1. - light_distance / max_distance), decay);
	}
	return 1.;
}

float spot_attenuate(float angle_cos, float max_angle_cos, float penumbra_cos) {
	return smoothstep(max_angle_cos, penumbra_cos, angle_cos);
}

vec3 lighting_direct(Material m, vec3 light_dir, vec3 view_dir, vec3 normal) {
	float nol = saturate(dot(normal, light_dir));
	vec3 specular = brdf_ggx(light_dir, view_dir, normal, m.f0, m.roughness);
	vec3 diffuse = brdf_lambert(m.color);
	return nol * (specular + diffuse);
}

#ifdef USE_POINT_LIGHT
vec3 apply_point_lights(Material m, Geometry g) {
	/* initialize total light */
	vec3 total_light = vec3(0.);
	
	/* apply all the point light */
	for (int i = 0; i < NUM_POINT_LIGHT; ++i) {
		
		/* get point light object */
		PointLight light = point_lights[i];
		
		/* check whether the light is visible */
		if (!light.visible) continue;
		
		/* calculate the vector form world to light */
		vec3 light_vec = light.position - g.position;
		vec3 light_dir = normalize(light_vec);
		float light_distance = length(light_vec);
		
		/* calculate distance attenuation */
		float attenuation = attenuate(light_distance, light.distance, light.decay);
		if (attenuation <= 0.) continue;
		
		/* calculate the color of light */
		vec3 light_color = light.color * attenuation;
		
		/* calculate lighting model */
		light_color *= lighting_direct(m, light_dir, g.view_dir, g.normal);
		
		/* accumulate to total light */
		total_light += light_color;
	}
	
	/* return the value of total light */
	return total_light;
}
#endif

#ifdef USE_SPOT_LIGHT
vec3 apply_spot_lights(Material m, Geometry g) {
	/* initialize total light */
	vec3 total_light = vec3(0.);
	
	/* apply all the spot light */
	for (int i = 0; i < NUM_SPOT_LIGHT; ++i) {
		
		/* get spot light object */
		SpotLight light = spot_lights[i];
		
		/* check whether the light is visible */
		if (!light.visible) continue;
		
		/* calculate the vector form world to light */
		vec3 light_vec = light.position - g.position;
		vec3 light_dir = normalize(light_vec);
		float light_distance = length(light_vec);
		
		/* calculate spot & distance attenuation */
		float attenuation = 1;
		float angle_cos = dot(light_dir, light.direction);
		attenuation *= spot_attenuate(angle_cos, light.angle, light.penumbra);
		attenuation *= attenuate(light_distance, light.distance, light.decay);
		if (attenuation <= 0.) continue;
		
		/* calculate the color of light */
		vec3 light_color = light.color * attenuation;
		
		/* apply spot light shadow */
		light_color *= light.shadow ?
			shadowing(spot_shadows[i], g.position, g.normal) : 1.;
		
		/* calculate lighting model */
		light_color *= lighting_direct(m, light_dir, g.view_dir, g.normal);
		
		/* accumulate to total light */
		total_light += light_color;
	}
	
	/* return the value of total light */
	return total_light;
}
#endif

#ifdef USE_DIRECTIONAL_LIGHT
vec3 apply_directional_lights(Material m, Geometry g) {
	/* initialize total light */
	vec3 total_light = vec3(0.);
	
	/* apply all the directional light */
	for (int i = 0; i < NUM_DIRECTIONAL_LIGHT; ++i) {
		/* get directional light object */
		DirectionalLight light = directional_lights[i];
		
		/* check whether the light is visible */
		if (!light.visible) continue;
		
		/* calculate the color of light */
		vec3 light_color = light.color;
		
		/* apply directional light shadow */
		light_color *= light.shadow ?
			shadowing(directional_shadows[i], g.position, g.normal) : 1.;
		
		/* calculate lighting model */
		light_color *= lighting_direct(m, light.direction, g.view_dir, g.normal);
		
		/* accumulate to total light */
		total_light += light_color;
	}
	
	/* return the value of total light */
	return total_light;
}
#endif

#ifdef USE_AMBIENT_LIGHT
vec3 apply_ambient_lights(Material m, Geometry g) {
	/* initialize total light */
	vec3 total_light = vec3(0.);
	
	/* apply all the ambient light */
	for (int i = 0; i < NUM_AMBIENT_LIGHT; ++i) {
		/* get ambient light object */
		AmbientLight light = ambient_lights[i];
		
		/* check whether the light is visible */
		if (!light.visible) continue;
		
		/* calculate the color of light */
		vec3 light_color = light.color;
		
		/* accumulate to total light */
		total_light += light_color;
	}
	
	/* return the value of total light */
	return total_light;
}
#endif

#ifdef USE_HEMISPHERE_LIGHT
vec3 apply_hemisphere_lights(Material m, Geometry g) {
	/* initialize total light */
	vec3 total_light = vec3(0.);
	
	/* apply all the hemisphere light */
	for (int i = 0; i < NUM_HEMISPHERE_LIGHT; ++i) {
		/* get hemisphere light object */
		HemisphereLight light = hemisphere_lights[i];
		
		/* check whether the light is visible */
		if (!light.visible) continue;
		
		/* calculate the color of light */
		float weight = dot(g.normal, light.direction) * 0.5 + 0.5;
		vec3 light_color = mix(light.ground_color, light.sky_color, weight);
		
		/* accumulate to total light */
		total_light += light_color;
	}
	
	/* return the value of total light */
	return total_light;
}
#endif

void main() {
	/* get diffuse color and depth */
	vec3 diffuse = textureLod(gbuffer_c, v_uv, 0).xyz;
	float depth = textureLod(gbuffer_d, v_uv, 0).x;
	
	/* ignore pixels on skybox */
	out_color = vec4(diffuse, 0.);
	if (depth == 1.) return;
	
	/* get world normal */
	vec3 normal = textureLod(gbuffer_n, v_uv, 0).xyz;
	
	/* normalize normal to avoid artifacts */
	normal = normalize(unpack_normal(normal));
	
	/* get specular and roughness */
	vec3 specular = textureLod(gbuffer_m, v_uv, 0).xyz;
	float roughness = textureLod(gbuffer_m, v_uv, 0).w;
	
	/* get indirect light */
	vec3 in_light = textureLod(gbuffer_i, v_uv, 0).xyz;
	
	/* setup material object */
	Material material;
	material.color = diffuse;
	material.f0 = specular;
	material.roughness = roughness;
	
	/* calculate world coordinates */
	vec4 ndc = vec4(v_uv.xy, depth, 1.) * 2. - 1.;
	vec4 world_pos = inv_view_proj * ndc;
	world_pos /= world_pos.w;
	
	/* from world position to camera position */
	vec3 view_dir = normalize(camera_pos - world_pos.xyz);
	
	/* setup geometry object */
	Geometry geometry;
	geometry.position = world_pos.xyz;
	geometry.view_dir = view_dir;
	geometry.normal = normal;
	
	/* initialize total light */
	vec3 total_light = vec3(0.);
	
	/* apply point lights on total light */
	#ifdef USE_POINT_LIGHT
		total_light += apply_point_lights(material, geometry);
	#endif
	
	/* apply spot lights on total light */
	#ifdef USE_SPOT_LIGHT
		total_light += apply_spot_lights(material, geometry);
	#endif
	
	/* apply directional lights on total light */
	#ifdef USE_DIRECTIONAL_LIGHT
		total_light += apply_directional_lights(material, geometry);
	#endif
	
	/* apply ambient lights on total light */
	#ifdef USE_AMBIENT_LIGHT
		total_light += apply_ambient_lights(material, geometry);
	#endif
	
	/* apply hemisphere lights on total light */
	#ifdef USE_HEMISPHERE_LIGHT
		total_light += apply_hemisphere_lights(material, geometry);
	#endif
	
	/* apply indirect lights on total light */
	total_light += in_light;
	
	/* output the color of total light */
	out_color = vec4(total_light, 0);
	
	/* apply tone mapping on output color */
	out_color.xyz = TONE_MAPPING(out_color.xyz, exposure);
}
