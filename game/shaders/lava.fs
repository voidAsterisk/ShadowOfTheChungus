uniform float     time;
uniform vec2      iResolution;
uniform vec2		viewport;
float iTime = time * 0.0005;

// random2 function by Patricio Gonzalez
vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

// Value Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ), 
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ), 
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

vec3 magmaFunc(vec3 color, vec2 uv, float detail, float power,
              float colorMul, float glowRate, bool animate, float noiseAmount)
{
    vec3 rockColor = vec3(0.09 + abs(sin(iTime * .75)) * .03, 0.05, .05);
    float minDistance = 1.;
    uv *= detail;
    
    vec2 cell = floor(uv);
    vec2 frac = fract(uv);
    
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
        	vec2 cellDir = vec2(float(i), float(j));
            vec2 randPoint = random2(cell + cellDir);
            randPoint += noise(uv) * noiseAmount;
            randPoint = animate ? 0.5 + 0.5 * sin(iTime * .35 + 6.2831 * randPoint) : randPoint;
            minDistance = min(minDistance, length(cellDir + randPoint - frac));
        }
    }
    	
    float powAdd = sin(uv.x * 2. + iTime * glowRate) + sin(uv.y * 2. + iTime * glowRate);
	vec3 outColor = vec3(color * pow(minDistance, power + powAdd * .95) * colorMul);
    outColor.rgb = mix(rockColor, outColor.rgb, minDistance);
    return outColor;
}

float floor_n (float value, float multiple) {
    return floor(value/multiple)*multiple;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{    
	vec2 coord = fragCoord;
	coord.x -= viewport.x;
	coord.y += viewport.y;
	coord.x = floor_n(coord.x, 15);
	coord.y = floor_n(coord.y, 15);		
	vec2 uv = coord / iResolution.xy;
    uv.x *= iResolution.x / iResolution.y;
    uv.x += iTime * .01;
	
    fragColor = vec4(0.);
    fragColor.rgb += magmaFunc(vec3(1.5, .45, 0.), uv, 3.,  2.5, 1.15, 1.5, false, 1.5);
    fragColor.rgb += magmaFunc(vec3(1.5, 0., 0.), uv, 6., 3., .4, 1., false, 0.);
    fragColor.rgb += magmaFunc(vec3(1.2, .4, 0.), uv, 8., 4., .2, 1.9, true, 0.5);
}
void main(void)
{
	mainImage( gl_FragColor, gl_FragCoord.xy );
}