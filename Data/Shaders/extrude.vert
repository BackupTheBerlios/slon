uniform float 	lightRadius;
uniform vec4 	lightPosition;

uniform mat4    worldViewMatrix;
uniform mat4    projectionMatrix;

void main()
{
	vec4 position = worldViewMatrix * gl_Vertex;
	vec3 normal   = ( worldViewMatrix * vec4(gl_Normal, 0.0) ).xyz;
	vec3 lightDir = normalize(lightPosition.xyz - position.xyz);
	
	if ( dot(normal, lightDir) < 0.0 ) 
	{
		// extrude back face
		position.xyz -= lightDir * lightRadius;
	}
	//position.xyz -= normal * 0.1;
	position.xyz -= lightDir * 0.1;
	
	gl_Position = projectionMatrix * position;
}
