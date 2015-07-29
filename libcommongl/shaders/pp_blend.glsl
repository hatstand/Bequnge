uniform sampler2D scene;
uniform sampler2D blur1;
uniform sampler2D blur2;
uniform sampler2D blur3;

/*uniform vec2 blur1Step;
uniform vec2 blur2Step;
uniform vec2 blur3Step;*/

uniform float bloomAmount;
uniform float desaturationAmount;

/*float p(float f)
{
	return max(0.0, f);
}

float r(float x)
{
	return (  pow(p(x+2.0), 3.0) - 4.0 * pow(p(x+1.0),3.0) + 6.0 * pow(p(x), 3.0) - 4.0* pow(p(x-1.0),3.0))/6.0;
}

vec4 sampleBicubic(sampler2D tex, vec2 pixelStep, vec2 coord)
{
	vec2 integerPos = floor(coord / pixelStep);
	vec2 d = coord / pixelStep - integerPos;
	
	vec4 ret = vec4(0.0, 0.0, 0.0, 0.0);
	
	for(int m = -1; m < 3; m++)
	{
		for(int n = -1; n < 3; n++)
		{
			vec4 sample = texture2D(tex, (integerPos + vec2(m, n)) * pixelStep);
			
			ret += sample * r(float(m) - d.x) * r(d.y - float(n));
		}
	}
	
	return ret;
}*/

void main()
{
	vec4 sceneSample = texture2D(scene, gl_TexCoord[0].xy);
	vec4 blur1Sample = bloomAmount * texture2D(blur1, gl_TexCoord[0].xy);
	vec4 blur2Sample = bloomAmount * texture2D(blur2, gl_TexCoord[0].xy);
	vec4 blur3Sample = 2.0 * bloomAmount * texture2D(blur3, gl_TexCoord[0].xy);
	
	gl_FragColor = sceneSample + blur1Sample + blur2Sample + blur3Sample;
}
