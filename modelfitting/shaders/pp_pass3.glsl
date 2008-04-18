uniform sampler2D source;

void main()
{
	vec3 sample = texture2D(source, gl_TexCoord[0].xy).rgb;
	
	gl_FragColor = vec4(sample, 1.0);
}
