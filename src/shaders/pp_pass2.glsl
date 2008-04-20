uniform sampler2D source;

uniform vec2 pixelStep;

void main()
{
	const int KERNEL_SIZE = 7;
	float[7] KERNEL = float[7](0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006);
	
	vec3 color = vec3(0.0, 0.0, 0.0);
	
	for (int i=0 ; i<KERNEL_SIZE ; ++i)
	{
		vec2 coord = vec2(gl_TexCoord[0].x + pixelStep.x*float(i - KERNEL_SIZE/2), gl_TexCoord[0].y);
		color.rgb += KERNEL[i] * texture2D(source, coord).rgb;
	}
	
	// Output fragment color
	gl_FragColor = vec4(color, 1.0);
}
