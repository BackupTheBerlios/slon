uniform sampler2D heightMap;

// attributes
attribute vec4 position;

uniform float sharpness;
uniform vec2  squeezeParams;
uniform vec2  surfaceSize; // size of the waterSurface
uniform vec3  surfaceCorners[4];
uniform vec4  eyePosition;

uniform float distanceSmoothness;
uniform mat4  projectionMatrix;
uniform mat4  worldViewMatrix;
uniform mat4  reflectionMatrix;

varying vec2  normalTexCoord;
varying vec3  eyeDir;
varying vec3  fragmentPosition;

#ifdef ENABLE_REFLECTIONS
varying vec3  reflectionTexCoord;
#endif

void main()
{
    float position_y = squeezeParams.x * (exp(squeezeParams.y * position.y) - 1.0);

    // calculate vertex position
    vec3 a = (surfaceCorners[2] - surfaceCorners[3]) * position_y;
    vec3 b = (surfaceCorners[1] - surfaceCorners[0]) * position_y;
    vec3 c = mix(b, a, position.x);
    vec3 d = (surfaceCorners[3] - surfaceCorners[0]) * position.x;
    vec4 waterVertex = vec4(surfaceCorners[0] + c + d, 1.0);
    
    // calculate texture coordinates
    normalTexCoord = waterVertex.xz / surfaceSize;

    // displace water vertex using height map with edge & distance attenuation    
    float distance            = length( (worldViewMatrix * waterVertex).xyz );
    float distanceAttenuation = exp(-distance * distanceSmoothness);
    float attenuation         = distanceAttenuation * min( pow(1.0 - position_y, 0.2), pow(1.0 - abs(position.x - 0.5) * 2.0, 0.2) );
    
    vec4 texel      = texture2D(heightMap, normalTexCoord); 
    waterVertex.y   = texel.x * attenuation;
    waterVertex.xz -= sharpness * texel.zw * attenuation;
      
    eyeDir   = waterVertex.xyz - eyePosition.xyz;

#ifdef ENABLE_REFLECTIONS
    vec4 projTexCoord  = reflectionMatrix * waterVertex; 
    reflectionTexCoord = projTexCoord.xyw;
#endif

    gl_Position      = projectionMatrix * worldViewMatrix * waterVertex;
    fragmentPosition = gl_Position.xyw;       
}
