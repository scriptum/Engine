uniform sampler2D tex0; // 0
const vec2 center=vec2(0.5,0.5); // Mouse position
uniform int tim; // effect elapsed time
const vec3 shockParams = vec3(10.0, 0.8, 0.1); // 10.0, 0.8, 0.1
void main()
{
  float time = float(tim)/1000.0;
  vec2 uv = gl_TexCoord[0].xy;
  vec2 texCoord = uv;
  float distance = distance(uv, center);
  //~ if ( (distance <= (time + shockParams.z)) &&
       //~ (distance >= (time - shockParams.z)) )
  //~ {
    float diff = (distance - time);
    float powDiff = 1.0 - pow(abs(diff*shockParams.x),
                                shockParams.y);
    float diffTime = diff  * powDiff;
    vec2 diffUV = normalize(uv - center);
    texCoord = uv + (diffUV * diff);
  //~ }
  
  gl_FragColor = texture2D(tex0, texCoord);
}
