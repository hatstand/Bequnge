uniform sampler2D scene;
uniform sampler2D blur1;
uniform sampler2D blur2;
uniform sampler2D blur3;

void main()
{
	vec3 sceneSample = 1.0 * texture2D(scene, gl_TexCoord[0].xy).rgb;
	vec3 blur1Sample = 0.8 * texture2D(blur1, gl_TexCoord[1].xy).rgb;
	vec3 blur2Sample = 0.9 * texture2D(blur2, gl_TexCoord[1].xy).rgb;
	vec3 blur3Sample = 1.0 * texture2D(blur3, gl_TexCoord[1].xy).rgb;
	
	gl_FragColor = vec4(sceneSample + blur1Sample + blur2Sample + blur3Sample, 1.0);
}
