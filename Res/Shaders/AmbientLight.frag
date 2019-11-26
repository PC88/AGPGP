// cubeMap fragment shader
#version 330

// Some drivers require the following
precision highp float;

layout(location = 0) out vec4 color;

uniform vec4 ambientLight;
 
void main(void) 
{   
	color = vec4(0.5, 0.5, 0.5, 1.0) * ambientLight;
}