uniform vec3 texOffset;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	vec4 texCoord = vec4(
		texOffset.x + texOffset.z * gl_MultiTexCoord0.x,
		texOffset.y + texOffset.z * gl_MultiTexCoord0.y,
		0.0, 0.0);
	
	gl_TexCoord[0] = texCoord;
	
	gl_FrontColor = gl_Color;
}
