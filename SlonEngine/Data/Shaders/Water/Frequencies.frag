//#extension GL_EXT_gpu_shader4 : enable

uniform vec2        gridSize;
uniform sampler2D   frequenciesMap;
uniform float       time;

//noperspective 
varying vec2 K;
varying vec2 texcoord;

vec2 complex_mult(vec2 a, vec2 b)
{
    return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

vec2 complex_exp(float imag)
{
    return vec2( cos(imag), sin(imag) );
}

vec2 conjugate(vec2 z)
{
    return vec2(z.x, -z.y);
}

void main()
{
    vec2 h0 = texture2DLod(frequenciesMap, texcoord, 0.0).xy;
    vec2 h1 = texture2DLod(frequenciesMap, vec2(1.0) - vec2(1.0)/gridSize - texcoord, 0.0).xy;
    vec2 w  = complex_exp( sqrt( length(K) * 9.8 ) * time );
    vec2 h  = complex_mult(h0, w) + conjugate( complex_mult(h1, conjugate(w)) );
    gl_FragColor = vec4( h, complex_mult( vec2(K.y, -K.x) / length(K), h ) );
}