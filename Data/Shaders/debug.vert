// uniforms
uniform mat4  	worldViewProjMatrix;

// attributes
attribute vec4 	position;

void main()
{
	gl_Position = worldViewProjMatrix * position;
}
