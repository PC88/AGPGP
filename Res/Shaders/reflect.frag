// Phong fragment shader phong-tex.frag matched with phong-tex.vert
#version 330

// Some drivers require the following
precision highp float;

uniform float ratioR;
uniform float ratioG;
uniform float ratioB;

layout(location = 0) out vec4 out_Color;

in vec3 ex_WorldNorm;
in vec3 ex_WorldView;
in vec3 ex_toCamera;

uniform samplerCube cubeMap;

void main(void) 
{

	vec4 refractColor;  
   refractColor.r = texture(cubeMap, refract(ex_WorldView, normalize(ex_WorldNorm),ratioR)).r;  
   refractColor.g = texture(cubeMap, refract(ex_WorldView, normalize(ex_WorldNorm),ratioG)).g; 
   refractColor.b = texture(cubeMap, refract(ex_WorldView, normalize(ex_WorldNorm),ratioB)).b; 
   refractColor.a = 1.0;


	vec3 reflectTexCoord = reflect(ex_WorldView, normalize(ex_WorldNorm));
	float ratio = 1.00 / 1.52;
	vec3 refract =  refract(ex_WorldView, normalize(ex_WorldNorm),ratio);
	vec4 refract1 =  texture(cubeMap, refract);
	vec4 reflectColor =  texture(cubeMap, reflectTexCoord);

	vec3 viewVector = normalize(ex_WorldView);
	float refractive =  dot(viewVector, normalize(ex_WorldNorm));
	refractive = pow(refractive, 5.0);

	
	//out_Color = reflectColor;
	//out_Color = refractColor;
	out_Color = mix(refractColor,reflectColor,refractive);

}