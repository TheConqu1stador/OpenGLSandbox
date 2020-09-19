#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec4 newColor;
void main()
{
   gl_Position = vec4(-aPos.x, -aPos.y, -aPos.z, 1.0);
   newColor = vec4(aColor, 1.0);
}