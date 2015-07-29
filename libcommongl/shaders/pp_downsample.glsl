uniform sampler2D source;

uniform vec2 pixelStep;

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	// Box filter
	/*color += 0.25 * texture2D(source, gl_TexCoord[0].xy + vec2(-1.0, -1.0) * 0.25 * pixelStep);
	color += 0.25 * texture2D(source, gl_TexCoord[0].xy + vec2(1.0, -1.0) * 0.25 * pixelStep);
	color += 0.25 * texture2D(source, gl_TexCoord[0].xy + vec2(1.0, 1.0) * 0.25 * pixelStep);
	color += 0.25 * texture2D(source, gl_TexCoord[0].xy + vec2(-1.0, 1.0) * 0.25 * pixelStep);*/
	
	//gl_FragColor = color;
	
	gl_FragColor = texture2D(source, gl_TexCoord[0].xy);
}
