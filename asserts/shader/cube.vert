#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uMvpMatrix;
void main()
{
    gl_Position = uMvpMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
