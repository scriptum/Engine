uniform sampler2D textureImage;

void main( void )
{
	gl_FragColor = texture2D( textureImage, gl_TexCoord[0].st );
}