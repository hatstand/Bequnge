uniform sampler2D tex;

void main()
{
	vec4 alpha = texture2D(tex, gl_TexCoord[0].xy).a;
	
	vec3 color = gl_Color.rgb * alpha;
	
	gl_FragColor = vec4(color, alpha);
}
