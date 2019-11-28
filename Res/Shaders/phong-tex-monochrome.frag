// Phong fragment shader phong-tex.frag matched with phong-tex.vert
#version 330

// Some drivers require the following
precision highp float;

uniform vec4 lightPosition;

struct lightStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float cutOff;
};

struct materialStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	//vec3 color;
	float shininess;
};

uniform lightStruct light;
uniform materialStruct material;
uniform sampler2D textureUnit0;

uniform float attConst;
uniform float attLinear;
uniform float attQuadratic;

uniform vec4 spotDir;

/*uniform float phoDiffuse=1.0f;
uniform float phoSpecular=1.0f;
uniform float phoShininess=1.0f;*/

in float in_dist;

in vec3 ex_N;
in vec3 ex_V;
in vec3 ex_L;
in vec2 ex_TexCoord;
layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Ambient intensity
	vec4 ambientI = light.ambient * material.ambient;

	// Diffuse intensity
	vec4 diffuseI = light.diffuse * material.diffuse;
	diffuseI = diffuseI * max(dot(normalize(ex_N),normalize(ex_L)),0)/**phoDiffuse*/;

	// Specular intensity
	// Calculate R - reflection of light
	vec3 R = normalize(reflect(normalize(-ex_L),normalize(ex_N)));

	vec4 specularI = light.specular * material.specular;
	specularI = specularI * pow(max(dot(R,ex_V),0), material.shininess/**phoShininess*/)/**phoSpecular*/;

	float attentuation = 1.0/(attConst + attLinear*in_dist + attQuadratic*in_dist*in_dist);

	vec4 tempColor = attentuation*(diffuseI + specularI);
	tempColor = tempColor + ambientI;
	tempColor = vec4(tempColor.rgb, 1.0);
	
	
	float theta = dot(ex_L, normalize(-spotDir.xyz));
	if(theta > light.cutOff) //If within area of light
	{       
		//Do lighting calculations
		out_Color = tempColor;
	}
	else  // else, use ambient light so scene isn't completely dark outside the spotlight.
	{
		//Make everything outside of the spotlight greyscale
		tempColor = vec4((tempColor.r+tempColor.g+tempColor.b)/3, (tempColor.r+tempColor.g+tempColor.b)/3, (tempColor.r+tempColor.g+tempColor.b)/3, 1.0);
		out_Color = tempColor * texture(textureUnit0, ex_TexCoord);
	}

}