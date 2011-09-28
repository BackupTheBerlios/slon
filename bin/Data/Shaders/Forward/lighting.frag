#if defined(ENABLE_NORMAL_HEIGHT_MAP) || defined(ENABLE_DIFFUSE_SPECULAR_MAP)
#define ENABLE_TEXTURING
#endif

// uniforms
uniform sampler2D diffuseSpecularMap;
uniform sampler2D normalHeightMap;

uniform vec4  	materialDiffuseSpecular;
uniform float 	materialShininess;
uniform vec4	lightDirectionAmbient[NUM_LIGHTS];
uniform vec4	lightPositionRadius[NUM_LIGHTS];
uniform vec4  	lightColorIntensity[NUM_LIGHTS];

vec3 contributeDirectionalLight(  int  index,
								#ifdef ENABLE_TEXTURING
								  vec2 texel, 
								#endif
								#ifndef ENABLE_NORMAL_HEIGHT_MAP
								  vec3 normal,
								#endif	
								  vec3 eyeDir
								)
{
#ifdef ENABLE_NORMAL_HEIGHT_MAP
	vec3 normal 		 = 2.0 * texture2D(normalHeightMap, texel).rgb - 1.0;
#endif

	// lighting
    float dotValue 		 = -dot(lightDirectionAmbient[index].xyz, normal);
    vec3  ambient        = materialDiffuseSpecular.rgb;   
    vec3  diffuse 		 = materialDiffuseSpecular.rgb * max(dotValue, 0.0);
    float specular 		 = materialDiffuseSpecular.a * pow( max( -dot(lightDirectionAmbient[index].xyz, reflect(eyeDir, normal)), 0.0), materialShininess);

#ifdef ENABLE_DIFFUSE_SPECULAR_MAP
	vec4 diffuseSpecular = texture2D(diffuseSpecularMap, texel);
    ambient             *= diffuseSpecular.rgb;
	diffuse  			*= diffuseSpecular.rgb;
	specular 			*= diffuseSpecular.a;
#endif

	return ambient * lightDirectionAmbient[index].a
           + (diffuse * lightColorIntensity[index].rgb + specular * lightColorIntensity[index].rgb) * lightColorIntensity[index].a;
}

vec3 contributePointLight(  int  index,
						#ifdef ENABLE_TEXTURING
							vec2 texel, 
						#endif
						#ifndef ENABLE_NORMAL_HEIGHT_MAP
							vec3 normal,
						#endif	
							vec3 position
						  )
{
#ifdef ENABLE_NORMAL_HEIGHT_MAP
	vec3 normal 		 = 2.0 * texture2D(normalHeightMap, texel).rgb - 1.0;
#endif
	vec3 lightDir		 = normalize(lightPositionRadius[index].xyz - position);
	vec3 eyeDir			 = normalize(position);

	// lighting
    float dotValue 		 = dot(lightDir, normal);
    vec3  diffuse 		 = materialDiffuseSpecular.rgb * max(dotValue, 0.0);
    float specular 		 = materialDiffuseSpecular.a * pow( max( dot(lightDir, reflect(eyeDir, normal)), 0.0), materialShininess);

#ifdef ENABLE_DIFFUSE_SPECULAR_MAP
	vec4 diffuseSpecular = texture2D(diffuseSpecularMap, texel);
	diffuse  			*= diffuseSpecular.rgb;
	specular 			*= diffuseSpecular.a;
#endif

	return (diffuse * lightColorIntensity[index].rgb + specular * lightColorIntensity[index].rgb) * lightColorIntensity[index].a;
}
