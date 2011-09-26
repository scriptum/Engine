void main (void)
{
  gl_FragColor = vec4(vec3(fract(sin(dot(vec2(gl_FragCoord.x/800,gl_FragCoord.y/600), vec2(12.9898, 78.233)))* 43758.5453)),1);
}