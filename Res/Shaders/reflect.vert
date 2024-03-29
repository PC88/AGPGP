
#version 330



uniform mat4 modelview;
uniform mat4 projection;


uniform mat4 modelMatrix;
uniform vec3 cameraPos;


in  vec3 in_Position;
in  vec3 in_Normal;


out vec3 ex_WorldNorm;
out vec3 ex_WorldView;
																									
void main(void) 
{
	vec3 worldPos = (modelMatrix * vec4(in_Position,1.0)).xyz;		
	
	mat3 normalworldmatrix = transpose(inverse(mat3(modelMatrix))); 

	ex_WorldNorm = normalize(normalworldmatrix * in_Normal);
	ex_WorldView = cameraPos - worldPos;

	
	gl_Position = projection * modelview * vec4(in_Position,1.0);
}