//~ uniform float c;void main(){gl_FragColor = vec4(c,1,1,1);}
uniform float time;
uniform vec2 resolution;
void main(void)
{
	vec2 p = -1.5 + 3 * gl_FragCoord.xy / resolution;
	vec2 cc = vec2( cos(.25*time), sin(.25*time*1.423) );

	float dmin = 1000.0;
	vec2 z  = p;
	for( int i=0; i<5; i++ )
	{
		z = cc + vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );
		float m2 = dot(z,z);
		if( m2>100.0 ) break;
		dmin=min(dmin,m2);
		z = cc + vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );
		m2 = dot(z,z);
		if( m2>100.0 ) break;
		dmin=min(dmin,m2);
		z = cc + vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );
		m2 = dot(z,z);
		if( m2>100.0 ) break;
		dmin=min(dmin,m2);
	}
	gl_FragColor = sqrt(sqrt(dmin))*0.7;
}