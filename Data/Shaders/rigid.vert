#if defined(ENABLE_NORMAL_HEIGHT_MAP) || defined(ENABLE_DIFFUSE_SPECULAR_MAP)
#define ENABLE_TEXTURING
#endif

// attributes
attribute vec4 	position;
attribute vec3 	normal;
attribute vec2	texcoord;

// uniforms
uniform mat4  worldViewMatrix;
uniform mat4  projectionMatrix;
uniform mat4  worldViewProjMatrix;
uniform mat3  normalMatrix;

// varyings
#ifndef DEPTH_ONLY
varying vec3  	fp_position;
#ifndef ENABLE_NORMAL_HEIGHT_MAP
varying vec3	fp_normal;
#endif
#ifdef ENABLE_TEXTURING
varying	vec2	fp_texcoord;
#endif
#endif // DEPTH_ONLY

void main()
{
#ifdef DEPTH_ONLY
	gl_Position = worldViewProjMatrix * position;
	
#else // DEPTH_ONLY
	vec4 pos	= worldViewMatrix * position;
    fp_position = pos.xyz;

#ifndef ENABLE_NORMAL_HEIGHT_MAP
    fp_normal   = normalMatrix * normal;
#endif

#ifdef ENABLE_TEXTURING
    fp_texcoord = texcoord;
#endif

    gl_Position = worldViewProjMatrix * position;
#endif // DEPTH_ONLY
}
