// Phong fragment shader phong-tex.frag matched with phong-tex.vert
#version 330

// Some drivers require the following
precision highp float;

in vec4 ex_Color;
out vec4 out_Color;
 
in vec2 ex_TexCoords;

void main(void) 
{
	out_Color = ex_Color;
}