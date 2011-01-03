// uniforms
uniform samplerCube environmentMap;

// varyings
varying vec3  texcoord;

void main()
{
    gl_FragColor = textureCube(environmentMap, texcoord);
}