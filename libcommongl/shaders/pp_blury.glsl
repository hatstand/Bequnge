uniform sampler2D source;

uniform vec2 pixelStep;

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	color.rgb += 0.006 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*(-3.0))).rgb;
	color.rgb += 0.061 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*(-2.0))).rgb;
	color.rgb += 0.242 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*(-1.0))).rgb;
	color.rgb += 0.383 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*( 0.0))).rgb;
	color.rgb += 0.242 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*(+1.0))).rgb;
	color.rgb += 0.060 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*(+2.0))).rgb;
	color.rgb += 0.006 * texture2D(source, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + pixelStep.y*(+3.0))).rgb;
	
	// Output fragment color
	gl_FragColor = vec4(color.rgb, 1.0);
}
