#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D fftInput;

uniform vec2  rowDirection;  // rows or columns
uniform vec2  shift;         // index shift of the fft
uniform float direction;     // 1 - FFT, -1 - IFFT
uniform float numPartitions; // N / 2^i, where i is number of the pass
uniform float N;             // number elements in the initial FFT sum 

#ifdef GL_EXT_gpu_shader4
noperspective varying vec2  index;
noperspective varying float linearIndex;
#else
varying vec2    index;
varying float   linearIndex;
#endif

#define PI     3.141592653
#define PI_2   (2.0 * PI)

vec2 complex_mult(vec2 a, vec2 b)
{
    return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

vec2 W(float partitionSize, float X)
{
    float angle = PI_2 * X / partitionSize;
    return vec2( cos(angle), -direction * sin(angle) );
}

void main()
{
    float k = floor(linearIndex / numPartitions);
    
    // fftIndex = ( iIndex + k * n ) % N
    vec2 fftIndex = index + rowDirection * k * numPartitions;
    fftIndex     -= floor(fftIndex / N) * N;

    // fetch fft coefficents
#ifdef GL_EXT_gpu_shader4
    vec4 He = texelFetch(fftInput, ivec2(fftIndex), 0);
    vec4 Ho = texelFetch(fftInput, ivec2(fftIndex + rowDirection * numPartitions), 0);
#else
    vec4 He = texture2DLod(fftInput, fftIndex / N, 0.0);
    vec4 Ho = texture2DLod(fftInput, (fftIndex + rowDirection * numPartitions) / N, 0.0);
#endif

    // compute next fft coefficent
    vec2 Wn = W(N / numPartitions, k);
    vec4 rhs = vec4( complex_mult(Wn, Ho.xy), complex_mult(Wn, Ho.zw) ); // vec4(H*W, H*W)
    vec4 result = He + rhs;

    // shift the FFT
    if ( numPartitions == 1.0 ) 
    {
        Wn = W( N, linearIndex * dot(shift, rowDirection) );
        result.xy = complex_mult(Wn, result.xy); 
        result.zw = complex_mult(Wn, result.zw);
        //result.xy += (result.xy + vec2(1.0)) / 2.0;
    }

    gl_FragColor = result; // H = He + W * Ho
}
