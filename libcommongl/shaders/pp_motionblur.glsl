uniform sampler2D previousFrame;

uniform float motionBlurAmount;

void main()
{
	vec3 sample = texture2D(previousFrame, gl_TexCoord[0].xy).rgb;
	
	// Output fragment color
	gl_FragColor = vec4(sample, motionBlurAmount);
}
