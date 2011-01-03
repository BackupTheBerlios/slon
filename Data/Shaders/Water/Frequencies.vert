//#extension GL_EXT_gpu_shader4 : enable

// attributes 
attribute vec2 position;

uniform vec2 gridSize;
uniform vec2 surfaceSize;

//noperspective 
varying vec2 K;
varying vec2 texcoord;

#define PI     3.141592653
#define PI_2   (2.0 * PI)

void main()
{
    texcoord     = position;
    
    vec2 shifted = position - vec2(0.5);
    K            = PI_2 * gridSize * shifted / surfaceSize;
    gl_Position  = vec4( 2.0 * shifted, 0.0, 1.0 );
}
