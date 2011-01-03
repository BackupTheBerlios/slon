#extension GL_EXT_gpu_shader4 : enable

uniform vec2  rowDirection;  // rows or columns
uniform float N;             // number elements in the initial FFT sum 

#ifdef GL_EXT_gpu_shader4
noperspective varying vec2  index;
noperspective varying float linearIndex;
#else
varying vec2    index;
varying float   linearIndex;
#endif

void main()
{
    vec2 gridVertex = N * gl_Vertex.xy;

    linearIndex = gridVertex.x;
    index       = gridVertex.x * rowDirection.xy + gridVertex.y * rowDirection.yx;
    gl_Position = vec4( 2.0 * index.xy / N - vec2(1.0), 0.0, 1.0 );
}
