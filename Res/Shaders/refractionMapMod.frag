// cubeMap fragment shader
#version 330

// Some drivers require the following
precision highp float;

out vec4 outColour;

uniform vec3 cameraPos;

uniform float ratioDiv;
uniform float ratioNum;

in vec3 out_Normal;
in vec3 out_Position;

smooth in vec3 cubeTexCoord;
uniform samplerCube cubeMap;
 
void main(void) 
{   
	// Fragment colour

	float ratio = ratioNum / ratioDiv;
	vec3 viewVec = normalize(out_Position - cameraPos);
    vec3 refractionVec = refract(viewVec, normalize(out_Normal), ratio);

	outColour = vec4(texture(cubeMap, refractionVec).rgb, 1.0);
}