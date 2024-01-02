#version 330 core 

// Fragment shader - this code is executed for every pixel/fragment that belongs to a displayed shape
//
// Compute the color using Phong illumination (ambient, diffuse, specular) 
//  There is 3 possible input colors:
//    - fragment_data.color: the per-vertex color defined in the mesh
//    - material.color: the uniform color (constant for the whole shape)
//    - image_texture: color coming from the texture image
//  The color considered is the product of: fragment_data.color x material.color x image_texture
//  The alpha (/transparent) channel is obtained as the product of: material.alpha x image_texture.a
// 

// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;


// Uniform values that must be send from the C++ code
// ***************************************************** //

uniform sampler2D image_texture;   // Texture image identifiant

uniform mat4 view;       // View matrix (rigid transform) of the camera - to compute the camera position

uniform vec3 light; // position of the light


// Coefficients of phong illumination model
struct phong_structure {
	float ambient;      
	float diffuse;
	float specular;
	float specular_exponent;
};

// Settings for texture display
struct texture_settings_structure {
	bool use_texture;       // Switch the use of texture on/off
	bool texture_inverse_v; // Reverse the texture in the v component (1-v)
	bool two_sided;         // Display a two-sided illuminated surface (doesn't work on Mac)
};

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object
	float alpha; // alpha coefficient

	phong_structure phong;                       // Phong coefficients
	texture_settings_structure texture_settings; // Additional settings for the texture
}; 

uniform material_structure material;

uniform float time;


//Simplex noise functions
float snoise(in vec2 v, int octaves);
float snoise_abs(in vec2 v, int octaves);
float snoise(in vec3 v, int octaves);
float snoise_abs(in vec3 v, int octaves);

// A colormap varying from 0 to 1 with bleck-red-yellow-white
vec3 colormap(float a);



vec3 procedural_texture(float u, float v, float t)
{
	// Set (x,y) to vary in [0,1]
	float x = 2.0*(u-0.5);
	float y = 2.0*(v-0.5);

	// (r,theta) are the polar coordinates
	float r = length(vec2(x,y));
	float theta = atan(x,y);
	
	// q is the angle variation normalized in [0,1] and symetrized
	float q = abs(theta/3.14);

	// ************************************************************* //
	// TO DO: Adapt and change the code to get the expected behavior
	// The following line is an example of noise
	//   snoise_abs = \sum_octaves abs(perlin_noise)
	// ************************************************************* //
	float n = 0.8*snoise_abs(vec2(5.0*q,r-0.1*t),5);

	return colormap(n);
}


vec3 colormap(in float a) {
	vec3 white = vec3(1,1,0.9);
	vec3 yellow = vec3(1,1,0);
	vec3 red = vec3(1,0,0);
	vec3 black = vec3(0,0,0);
	if(a>0.8) {
		a = (a-0.8)/0.2;
		return mix(yellow, white, a);
	}
	if(a>0.5){
		a = (a-0.5)/(0.8-0.5);
		return mix(red, yellow, a);
	}
	else{
		a = 2.0*a;
		return mix(black, red, a);
	}
}

void main()
{
	// Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
	vec3 camera_position = -O*last_col;


	// Renormalize normal
	vec3 N = normalize(fragment.normal);

	// Inverse the normal if it is viewed from its back (two-sided surface)
	//  (note: gl_FrontFacing doesn't work on Mac)
	if (material.texture_settings.two_sided && gl_FrontFacing == false) {
		N = -N;
	}

	// Phong coefficient (diffuse, specular)
	// *************************************** //

	// Unit direction toward the light
	vec3 L = normalize(light-fragment.position);

	// Diffuse coefficient
	float diffuse_component = max(dot(N,L),0.0);

	// Specular coefficient
	float specular_component = 0.0;
	if(diffuse_component>0.0){
		vec3 R = reflect(-L,N); // reflection of light vector relative to the normal.
		vec3 V = normalize(camera_position-fragment.position);
		specular_component = pow( max(dot(R,V),0.0), material.phong.specular_exponent );
	}

	// Texture
	// *************************************** //

	// Current uv coordinates
	vec2 uv_image = vec2(fragment.uv.x, fragment.uv.y);
	if(material.texture_settings.texture_inverse_v) {
		uv_image.y = 1.0-uv_image.y;
	}

	// Get the current texture color
	vec4 color_image_texture = texture(image_texture, uv_image);
	if(material.texture_settings.use_texture == false) {
		color_image_texture=vec4(1.0,1.0,1.0,1.0);
	}
	
	// Compute Shading
	// *************************************** //

	// Compute the base color of the object based on: vertex color, uniform color, and texture
	vec3 color_object  = fragment.color * material.color * color_image_texture.rgb;
	color_object = color_object * procedural_texture(fragment.uv.x, fragment.uv.y, time);

	// Compute the final shaded color using Phong model
	float Ka = material.phong.ambient;
	float Kd = material.phong.diffuse;
	float Ks = material.phong.specular;
	vec3 color_shading = (Ka + Kd * diffuse_component) * color_object + Ks * specular_component * vec3(1.0, 1.0, 1.0);
	
	// Output color, with the alpha component
	FragColor = vec4(color_shading, material.alpha * color_image_texture.a);
}


// ************************************************ //
// GLSL Simplex Noise
// //
// Description : Array and textureless GLSL 2D/3D/4D simplex
// noise functions.
// Author : Ian McEwan, Ashima Arts.
// Maintainer : ijm
// Lastmod : 20110822 (ijm)
// License : Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise
//
// See: https://www.shadertoy.com/view/Mds3Wr
// ************************************************ //
float smooth_snoise(in vec2 v);
float smooth_snoise(in vec3 v);
float snoise(in vec2 v, int octaves)
{
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += smooth_snoise(v) * scale;
		v *= vec2(2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}
float snoise(in vec3 v, int octaves)
{
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += smooth_snoise(v) * scale;
		v *= vec3(2.0, 2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}
float snoise_abs(in vec2 v, int octaves)
{
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += abs(smooth_snoise(v)) * scale;
		v *= vec2(2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}
float snoise_abs(in vec3 v, int octaves)
{
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += abs(smooth_snoise(v)) * scale;
		v *= vec3(2.0, 2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}
vec3 mod289(in vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec2 mod289(in vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec3 permute(in vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}
float smooth_snoise(in vec2 v){
  const vec4 C = vec4(0.211324865405187,
                      0.366025403784439, 
                     -0.577350269189626,
                      0.024390243902439);
// First corner
  vec2 i = floor(v + dot(v, C.yy) );
  vec2 x0 = v - i + dot(i, C.xx);

// Other corners
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

vec4 mod289(in vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(in vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(in vec4 r){
  return 1.79284291400159 - 0.85373472095314 * r;
}

float smooth_snoise(in vec3 v){
  const vec2 C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i = floor(v + dot(v, C.yyy) );
  vec3 x0 = v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; 
  vec3 x3 = x0 - D.yyy;

// Permutations
  i = mod289(i);
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
  float n_ = 0.142857142857;
  vec3 ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}
// ************************************************ //