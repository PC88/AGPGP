// cubeMap fragment shader
#version 330

// Some drivers require the following
precision highp float;

out vec4 outColour;

uniform vec3 cameraPos;
in vec3 out_Normal;
in vec3 out_Position;

smooth in vec3 cubeTexCoord;
uniform samplerCube cubeMap;
 
void main(void) 
{   
	// Fragment colour

	float ratio = 1.00f / 1.52f;
	vec3 viewVec = normalize(out_Position - cameraPos);
    vec3 refractionVec = refract(viewVec, normalize(out_Normal), ratio);
    // FragColor = vec4(texture(skybox, R).rgb, 1.0);

	//outColour = texture(cubeMap, cubeTexCoord);
	outColour = vec4(texture(cubeMap, refractionVec).rgb, 1.0);
}