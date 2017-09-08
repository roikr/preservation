#version 150
in vec2 v_texcoord;
uniform sampler2D tex;
out vec4 fragColor;

void main()
{
   vec3 c=vec3(texture(tex, v_texcoord).a);
   fragColor = vec4(c,1);
}