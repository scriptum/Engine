/**
 *    Introduction to GPU Programming with GLSL
 *
 *  Fragment Shader -- Cartoon shader
 *
 *  Maximo, Andre -- Mar, 2009
 *
 **/

varying vec3 normal;

void main(void) {

	vec4 color = gl_Color;

	vec3 light_dir = normalize( vec3( gl_LightSource[0].position ) );
	float intensity = dot( light_dir, normal );

	if (intensity > 0.95)      color = 1.00 * gl_Color;
	else if (intensity > 0.5)  color = 0.50 * gl_Color;
	else if (intensity > 0.1)  color = 0.25 * gl_Color;
	else                       color = 0.10 * gl_Color;

	gl_FragColor = color;

}
