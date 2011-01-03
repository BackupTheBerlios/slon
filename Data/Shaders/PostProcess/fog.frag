uniform sampler2D inputMap;

// uniforms
uniform vec3    fogDirection;
uniform vec3    fogColor;
uniform float   fogHeightFalloff;
uniform float   fogDensity;
uniform float   fogLevel;

// varyings
varying vec3    fp_position;
varying vec2    fp_texcoord;

// extern functions
float get_fragment_depth(vec2 texcoord);

float get_fog_density(in vec3 observePoint)
{
    float worldViewY = dot(observePoint, fogDirection);
    float fogInt     = length(observePoint) * exp( -fogHeightFalloff * (worldViewY - fogLevel) );
    if(abs(worldViewY) > 0.01)
    {
        float t = fogHeightFalloff * worldViewY;
        fogInt *= ( 1.0 - exp(-t) ) / t;
    }

    return exp(-fogDensity * fogInt);
}

void main()
{
    vec3  pos       = fp_position * get_fragment_depth(fp_texcoord);
    float fogVal    = get_fog_density(pos);
#ifdef USE_ALPHA_BLEND
    gl_FragData[0] = vec4(fogColor, fogVal);
#else
    gl_FragData[0] = vec4(mix(fogColor, texture2D(inputMap, fp_texcoord).rgb, fogVal), 1.0);
#endif
}