#version 400 core
in vec4 newColor;
out vec4 Frag_Color;
void main()
{
    	Frag_Color = newColor;
}