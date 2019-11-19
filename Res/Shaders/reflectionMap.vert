
#version 330

uniform mat4 modelview;
uniform mat4 projection;

in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 out_Normal;
out vec3 out_Position;

smooth out vec3 cubeTexCoord;

// multiply each vertex position by the modelview projection
void main(void) {
	// vertex into eye coordinates
	vec4 vertexPosition = modelview * vec4(in_Position,1.0);

	// calculate normal
	mat3 normalmatrix = transpose(inverse(mat3(modelview)));
	out_Normal = normalize(normalmatrix * in_Normal);
	// calculate position
	out_Position = vec3(modelview * vec4(in_Position,1.0));

    gl_Position = projection * vertexPosition;
	cubeTexCoord = normalize(in_Position);
}