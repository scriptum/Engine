//~ uniform float c;void main(){gl_FragColor = vec4(c,1,1,1);}
uniform int time;
void main(void)
{
	vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / vec2(800,600);
	vec2 cc = vec2( cos(.25*time/100), sin(.25*time/100*1.423) );

	float dmin = 1000.0;
	vec2 z  = p*vec2(1.33,1.0);
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