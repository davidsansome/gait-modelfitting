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
		
		vec3 sample = texture2D(source, coord).rgb;
		float glow = pow((sample.r + sample.g + sample.b) / 3.0, 2.0);
		sample *= glow;
		
		color.rgb += KERNEL[i] * sample;
	}
	
	// Output fragment color
	gl_FragColor = vec4(color, 1.0);
}
