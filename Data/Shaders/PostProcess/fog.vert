// attributes
attribute vec2	position;

// uniforms
uniform mat4	projectionMatrixInverse;
uniform vec4	eyePosition;
uniform float 	fogHeightFalloff;
uniform float   fogLevel;

varying vec3	fp_position;
varying vec2	fp_texcoord;

void main()
{
    fp_texcoord = position;
	
	vec4 pos    = projectionMatrixInverse * vec4(2.0 * position - 1.0, 1.0, 1.0);
	fp_position = vec3(pos.xy, 1.0);
    gl_Position = vec4(2.0 * position - 1.0, 0.0, 1.0);
}
