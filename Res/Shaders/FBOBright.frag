#version 330

// Some drivers require the following
precision highp float;

 
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

//out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;

uniform vec3 edgeColor;
uniform float herp1;
uniform float herp2;

// sobelx
mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);

// sobely
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

void main(void) 
{   
	// calulate a diffuse value
    vec3 diffuse = texture(screenTexture, TexCoords.st).rgb;
	// create a matrix to fill with texels
    mat3 I;
	// fill it with texels to perform the comparison of the Sobel operation
    for (int i=0; i<3; i++) 
	{
        for (int j=0; j<3; j++) 
		{
            vec3 sample  = texelFetch(screenTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(sample); 
		}
	}

	// gradient X and Y
	// perform Sobel comparison
	float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
	float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

	// calculate the gradient magnitude
	float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));

	// lerp the gradient values
	g = smoothstep(herp1, herp2, g);

	// first do normal lighting calculations and output results
	FragColor = vec4(mix(diffuse, edgeColor, g), 1.0);

	// check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
	{
        BrightColor = vec4(FragColor.rgb, 1.0);
	}
    else
	{
    	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
    
}