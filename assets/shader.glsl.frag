#version 450 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
	int index = int(v_TexIndex);
	//o_Color = texture(u_Textures[index], v_TexCoord) * v_Color; //This is currently commented so I could see if I can just see something white
	//Even If I uncomment the code above, it doesn't work
	o_Color = vec4(1.f,1.f,1.f, 1.f);
}
