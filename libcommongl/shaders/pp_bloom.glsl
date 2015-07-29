uniform sampler2D source;

uniform vec2 pixelStep;

void main()
{
	vec3 sample = texture2D(source, gl_TexCoord[0].xy).rgb;
	
	float glow = pow(max(max(sample.r, sample.g), sample.b), 2.0);
	sample *= glow;
	
	// Output fragment color
	gl_FragColor = vec4(sample, 1.0);
}
