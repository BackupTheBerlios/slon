uniform sampler2D fftMap;

uniform float sharpness;
uniform float amplitude;
uniform vec2  gridSize;
uniform vec2  surfaceSize;

vec3 compute_position(sampler2D fftMap, vec2 texel)
{
    vec4 tex = texture2DLod(fftMap, texel, 0.0);
    vec3 pos = vec3( texel * surfaceSize, tex.x );
    pos.xy  -= sharpness * tex.zw;

    return pos;
}

vec3 compute_local_normal(sampler2D fftMap, vec2 texel, vec2 gridSize)
{
    vec2 dx  = vec2(1.0 / gridSize.x, 0.0);
    vec2 dy  = vec2(0.0, 1.0 / gridSize.y);
    vec3 pos = compute_position(fftMap, texel);

    vec3 norm0 = normalize( cross( compute_position(fftMap, texel + dy) - pos,
                                   compute_position(fftMap, texel + dx) - pos ) );
    vec3 norm1 = normalize( cross( compute_position(fftMap, texel - dy) - pos,
                                   compute_position(fftMap, texel - dx) - pos ) );

    return -0.5 * (norm0 + norm1);
}

void main()
{
     //vec3 wave = 0.5 * ( texel.xzw + vec3(amplitude) ) / amplitude;
    vec3 normal = 0.5 * compute_local_normal(fftMap, gl_TexCoord[0].xy, gridSize).xzy + vec3(0.5);

    //gl_FragData[0] = vec4(wave, 0.0);
    gl_FragData[0] = vec4(normal, 0.0);
}
