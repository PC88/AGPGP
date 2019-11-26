#version 330

// standard vertex data
uniform mat4 modelview;
uniform mat4 projection;

// the input values
in vec3 in_Normal;
in vec3 in_Position;
in vec4 aVertex;

// pass through texcoords
in vec2 in_TexCoord;
out vec2 ex_TexCoord;

// pass out color
out vec4 ex_Color;

void main(void)
{
	// pass through texcoords
	ex_TexCoord = in_TexCoord;

	ex_Color = vec4(0.3f, 0.5f, 0.1f, 1.0f);

	// old way of doing this without direct MVP
	vec4 vertexPosition = modelview * vec4(in_Position,1.0);
    gl_Position = projection * vertexPosition;
}