varying vec2 fp_texcoord;

void main()
{
    fp_texcoord = 0.5 * gl_Vertex.xy + 0.5
    gl_Position = gl_Vertex;
}
