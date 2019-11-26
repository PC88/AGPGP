// cubeMap.vert
// Vertex shader for cubemap for e.g. sky box, with no lights
#version 330

in vec3 in_Position;

uniform mat4 modelview;
uniform mat4 projection;

void main(void) 
{
	vec4 vertexPosition = modelview * vec4(in_Position,1.0);
    gl_Position = projection * vertexPosition;
}