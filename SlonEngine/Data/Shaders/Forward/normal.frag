#if defined(ENABLE_NORMAL_HEIGHT_MAP) || defined(ENABLE_DIFFUSE_SPECULAR_MAP)
#define ENABLE_TEXTURING
#endif

// uniforms
uniform float   opacity;

// varyings
varying vec3  	fp_position;
#ifndef ENABLE_NORMAL_HEIGHT_MAP
varying vec3	fp_normal;
#endif
#ifdef ENABLE_TEXTURING
varying	vec2	fp_texcoord;
#endif

vec3 contributeDirectionalLight(  int  index,
								#ifdef ENABLE_TEXTURING
								  vec2 texel, 
								#endif
								#ifndef ENABLE_NORMAL_HEIGHT_MAP
								  vec3 normal,
								#endif
								  vec3 eyeDir
								);
								
vec3 contributePointLight(  int  index,
						#ifdef ENABLE_TEXTURING
							vec2 texel, 
						#endif
						#ifndef ENABLE_NORMAL_HEIGHT_MAP
							vec3 normal,
						#endif
							vec3 position
						  );
						  
void main()
{
#ifndef ENABLE_NORMAL_HEIGHT_MAP
    vec3 normalNorm   = normalize(fp_normal);
#else

#endif // ENABLE_NORMAL_HEIGHT_MAP

	vec3 color = vec3(0.0);
#ifdef DIRECTIONAL_LIGHTING
    vec3 eyeDirNorm = normalize(fp_position);
	for (int i = 0; i<NUM_LIGHTS; ++i) 
	{
		color += contributeDirectionalLight( i,
										#ifdef ENABLE_TEXTURING
											 fp_texcoord,
										#endif
										#ifndef ENABLE_NORMAL_HEIGHT_MAP
											 normalNorm,
										#endif
											 eyeDirNorm );										 
	}
#elif defined(POINT_LIGHTING)
	for (int i = 0; i<NUM_LIGHTS; ++i) 
	{
		color += contributePointLight( 	i,
									#ifdef ENABLE_TEXTURING
										fp_texcoord,
									#endif
									#ifndef ENABLE_NORMAL_HEIGHT_MAP
										normalNorm,
									#endif
										fp_position );										 
	}
#endif

	gl_FragColor = vec4(color, opacity);
}
