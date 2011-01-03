uniform sampler2D heightMap;

uniform mat4  worldViewMatrix;

uniform float sharpness;
uniform float distanceSmoothness;
uniform vec2  patchSize;
uniform vec3  surfaceCorners[4];

float pack4(float value)
{
    return 0.5 * floor(value * 10000.0) * 0.0001 + 0.5;
}

void main()
{
    // calculate vertex position
    vec3 a = (surfaceCorners[2] - surfaceCorners[3]) * gl_TexCoord[0].y;
    vec3 b = (surfaceCorners[1] - surfaceCorners[0]) * gl_TexCoord[0].y;
    vec3 c = mix(b, a, gl_TexCoord[0].x);
    vec3 d = (surfaceCorners[3] - surfaceCorners[0]) * gl_TexCoord[0].x;
    vec4 waterVertex = vec4(surfaceCorners[0] + c + d, 1.0);

    // smooth distant water 
    float distance            = length( (worldViewMatrix * waterVertex).xyz );
    float distanceAttenuation = exp(-distance * distanceSmoothness);
    float attenuation         = distanceAttenuation * min( pow(1.0 - gl_TexCoord[0].y, 0.2), pow(1.0 - abs(gl_TexCoord[0].x - 0.5) * 2.0, 0.2) );
    
    // displace water vertex using height map with edge & distance attenuation    
    vec4 texel      = texture2D(heightMap, waterVertex.xz / patchSize); 
    vec2 offset     = clamp(sharpness * texel.zw * attenuation, -1.0, 1.0);
    waterVertex.y   = texel.x * attenuation;

    float packedOffset = pack4(offset.x) + pack4(offset.y) * 0.0001;
    gl_FragData[0] = vec4(waterVertex.xyz, packedOffset);
}