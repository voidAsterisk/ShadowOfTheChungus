uniform float     time;
uniform vec2      iResolution;
uniform vec2		viewport;
float iTime = time * 0.0005;

float pi = 3.1416;

float floor_n (float value, float multiple) {
    return floor(value/multiple)*multiple;
}


void mainImage( out vec4 col, in vec2 coo )
{
	vec2 crd = coo;
	crd.x -= viewport.x;
	crd.y += viewport.y;
	crd.x = floor_n(crd.x, 15);
	crd.y = floor_n(crd.y, 15);		
	vec2 uv = (crd.xy - iResolution.xy * 0.5) / iResolution.y;
    vec2 p = vec2(sin(iTime / 3.1), cos(iTime / 3.1));
    float v = (
        sin(length(uv - p) * 20.0) + 
        sin(iTime + 16.0 * (uv.x + uv.y)) + 
        sin(iTime / 8.0 + uv.x*6.0*pi) + 
        sin(iTime / 4.8 + uv.y*4.0*pi) + 
        4.0) / 8.0;
    float i = pow(cos(3.5 * pi * (v + iTime / 10.0)) + 1.0, 10.0) / 1000.0;
    vec3 c = mix(vec3(0.0), vec3(0.6,0.0,1.0), i);
	col = vec4(c,1.0);
}

void main(void)
{
	mainImage( gl_FragColor, gl_FragCoord.xy );
}