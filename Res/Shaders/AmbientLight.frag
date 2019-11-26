// cubeMap fragment shader
#version 330

// Some drivers require the following
precision highp float;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


uniform vec4 ambientLight;
 
void main(void) 
{   
	FragColor = vec4(0.5, 0.5, 0.5, 1.0) * ambientLight;
    //FragColor = vec4(lighting, 1.0);
    // check whether fragment output is higher than threshold, if so output as brightness color
    //float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	float brightness = dot(FragColor.rgb, vec3(1.0, 1.0, 1.0));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}