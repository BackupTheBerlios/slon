//uniform sampler2D   heightMap;
uniform sampler2D   normalMap;
uniform sampler2D   reflectMap;
uniform sampler2D   refractMap;
uniform samplerCube environmentMap;

uniform float distanceSmoothness;
uniform float waterTransparency;

// lighting
uniform vec4  lightDirectionAmbient;
uniform vec4  lightPositionRadius;
uniform vec4  lightColorIntensity;

varying vec2  normalTexCoord;
varying vec3  eyeDir;
varying vec3  fragmentPosition;

#ifdef ENABLE_REFLECTIONS
varying vec3  reflectionTexCoord;
#endif

float get_fragment_depth(vec2 texCoord);

void main()
{
    const vec3 waterMinColor = vec3(0.0, 0.05, 0.15);
    const vec3 waterMaxColor = vec3(0.0, 0.1,  0.15);

    vec3 eyeDirNorm = normalize(eyeDir);
    vec3 normalNorm = 2.0 * (texture2D(normalMap, normalTexCoord).rgb - 0.5);

    // distance attenuation of light and fogginess
    float distVal = exp(-fragmentPosition.z * distanceSmoothness * 5.0);
    normalNorm    = mix( vec3(0.0, 1.0, 0.0), normalNorm, distVal );

    float dotValue   = dot(eyeDirNorm, normalNorm);
    vec3  waterColor = mix( waterMinColor, waterMaxColor, abs(dotValue) );

    // texture coordinate of the fragment
    vec2  fragTexCoord = 0.5 * fragmentPosition.xy / fragmentPosition.z + 0.5; 
            
    #ifdef ENABLE_REFRACTIONS
    {     
        vec2  distortTexCoord = fragTexCoord + normalNorm.xz * 0.02;
        
        #ifdef ENABLE_DEPTH_MAP
        float nonDistortDepth = get_fragment_depth(fragTexCoord);
        float distortDepth    = get_fragment_depth(distortTexCoord);

        // calculate distorsion
        float deltaDepth;
        if (distortDepth > fragmentPosition.z) {
            deltaDepth = distortDepth - fragmentPosition.z;
        }
        else 
        {
            distortTexCoord = fragTexCoord;
            deltaDepth      = nonDistortDepth - fragmentPosition.z;
        }
        
        // calculate attenuation
        float depthAttenuation = 1.0 / pow(1.0 + deltaDepth, 1.0 / waterTransparency - 1.0);

        // mix water with reflections
        vec4 refractColor = texture2D(refractMap, distortTexCoord).rgba;
        waterColor        = mix(waterColor, refractColor.rgb, depthAttenuation * refractColor.a); 
        #else
        vec4 refractColor = texture2D(refractMap, distortTexCoord).rgba;
        waterColor        = mix(waterColor, refractColor.rgb, waterTransparency * refractColor.a); 
        #endif
    }
    #endif

    float fresnel      = clamp( pow(1.0 + dotValue, 4.0), 0.05, 0.5 );
	vec3  reflectColor = textureCube( environmentMap, reflect(eyeDirNorm, normalNorm) ).rgb;

    #ifdef ENABLE_REFLECTIONS
    {
        vec2 distortTexCoord   = reflectionTexCoord.xy / reflectionTexCoord.z + normalNorm.xz * 0.05;
        vec4 localReflectColor = texture2D(reflectMap, distortTexCoord);
        reflectColor           = mix(reflectColor, localReflectColor.rgb, localReflectColor.a);
    }
    #endif
	
    waterColor = mix(waterColor, reflectColor, fresnel);

#ifdef DIRECTIONAL_LIGHTING
    vec3 halfVecNorm  = normalize(eyeDirNorm + lightDirectionAmbient.xyz);
    vec3 light        = lightColorIntensity.w * lightColorIntensity.rgb * pow( abs( dot(halfVecNorm, normalNorm) ), 32.0);

    waterColor 	  	 += light * exp(-fragmentPosition.z * distanceSmoothness);
#endif

    gl_FragColor = vec4(waterColor, 1.0);
}
