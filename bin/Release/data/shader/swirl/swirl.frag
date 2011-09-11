//~ uniform float c;void main(){gl_FragColor = vec4(c,1,1,1);}
// Scene buffer
uniform sampler2D tex0; 


// GeeXLab built-in uniform, width of
// the current render target
const float rt_w = 800;
// GeeXLab built-in uniform, height of
// the current render target
const float rt_h = 600; 

// Swirl effect parameters
const float radius = 300.0;
const float angle = 3.1415/4;
const vec2 center = vec2(400, 300.0);

vec4 PostFX(sampler2D tex, vec2 uv)
{
  vec2 texSize = vec2(rt_w, rt_h);
  vec2 tc = uv * texSize;
  tc -= center;
  float dist = length(tc);
  if (dist < radius)
  {
    float percent = (radius - dist) / radius;
    float theta = percent * percent * angle * 8.0;
    float s = sin(theta);
    float c = cos(theta);
    tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
  }
  tc += center;
  vec3 color = texture2D(tex0, tc / texSize).rgb;
  return vec4(color, 1.0);
}

void main (void)
{
  gl_FragColor = PostFX(tex0, gl_TexCoord[0].st);
}