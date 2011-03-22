// attributes 
attribute vec2 position;

void main()
{
    gl_Position       = vec4( 2.0 * ( position - vec2(0.5) ), 0.0, 1.0 );
    gl_TexCoord[0].xy = position;
}
