//"uniform vec4 fpar[4];"

void main(void)
{
	vec2 c=vec2(-2,-1)+gl_TexCoord[0].xy*vec2(1.33,1.0);
	vec2 z=vec2(0.0);
	float h=0.0;
	float m;
  float dmin = 1000;
	for(int i=0;i<30;i++)
	{
		z=vec2(z.x*z.x-z.y*z.y,2.0*z.x*z.y)+c;
		m=dot(z,z);
		if(m>100.0) break;
		dmin=min(dmin,m);
	}
	//~ h=h+1.0-log2(.5*log(m));
	gl_FragColor=vec4(sqrt(dmin));
};