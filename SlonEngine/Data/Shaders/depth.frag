// z-buffer texture
uniform sampler2D depthMap;

// inverted projection matrix corner to restore depth from the z-buffer
uniform vec4 depthParams;

float get_fragment_depth(vec2 texCoord)
{
    float depth = 2.0 * texture2D(depthMap, texCoord).r - 1.0;
    return (depthParams.x * depth + depthParams.y) / (depthParams.z * depth + depthParams.w);
}
