uniform sampler2D source;

uniform vec2 pixelStep;

float KERNEL[7];

void main()
{
	KERNEL[0] = 0.006;
	KERNEL[1] = 0.061;
	KERNEL[2] = 0.242;
	KERNEL[3] = 0.383;
	KERNEL[4] = 0.242;
	KERNEL[5] = 0.061;
	KERNEL[6] = 0.006;
	const int KERNEL_SIZE = 7;
	
	vec3 color = vec3(0.0, 0.0, 0.0);
	
	for (int i=0 ; i<KERNEL_SIZE ; ++i)
	{
		vec2 coord = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*float(i - KERNEL_SIZE/2));
		color.rgb += KERNEL[i] * texture2D(source, coord).rgb;
	}
	
	// Output fragment color
	gl_FragColor = vec4(color, 1.0);
}
