const vec3 blue = vec3(0.0, 0.5, 1.0);

void main()
{
	// Unpack
	float lighting = gl_TexCoord[0].x;
	
	// Apply lighting
	vec3 color = blue * lighting;
	
	// Output fragment color
	gl_FragColor = vec4(color, 1.0);
}
