void main()
{
	// Convert things to eye space
	vec3 posEye = (gl_ModelViewMatrix * gl_Vertex).xyz;
	vec3 lightEye = gl_LightSource[0].position.xyz;
	vec3 normalEye = normalize(gl_NormalMatrix * gl_Normal);
	
	// Calculate vector from light to pos
	vec3 lightVector = normalize(lightEye - posEye);
	
	float ambient = 0.2;
	float diffuse = 0.7 * clamp(dot(lightVector, normalEye), 0.0, 1.0);
	float lighting = ambient + diffuse;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = vec4(lighting, 0.0, 0.0, 0.0);
	
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
}
