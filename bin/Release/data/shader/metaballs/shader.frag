uniform vec2 resolution;
uniform vec2 mouse;
uniform float time;

float calcMetaBalls(vec2 pos){
    //first metaball with radius 0.3
    float val =  0.3 / distance(pos, vec2(0.0, 0.0));
  
    //second metaball with radius 0.1 at mousePos
    val += 0.1 / distance(pos, mouse.xy/resolution.xy*2.0-1.0);

    return val;
}

void main(void)
{
    vec2 screenPos = ((gl_FragCoord.xy / resolution.xy)*2.0-1.0);
    float val = 0.0;
    float temp = calcMetaBalls(screenPos);


    gl_FragColor = vec4(20*(temp-max_th));
}