uniform sampler2D source;

uniform vec2 pixelStep;

float KERNEL[7];

void main()
{
	const int KERNEL_SIZE = 7;
	KERNEL[0] = 0.006;
	KERNEL[1] = 0.061;
	KERNEL[2] = 0.242;
	KERNEL[3] = 0.383;
	KERNEL[4] = 0.242;
	KERNEL[5] = 0.061;
	KERNEL[6] = 0.006;
	
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
