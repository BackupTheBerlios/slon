// uniforms
uniform mat4  viewProjectionMatrix;

// attributes
attribute vec4	position;

// varyings
varying vec3  texcoord;

void main()
{
    texcoord    = position.xyz;
    gl_Position = (viewProjectionMatrix * position).xyww;
}
