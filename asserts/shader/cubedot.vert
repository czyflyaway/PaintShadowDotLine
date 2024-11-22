#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uMvpMatrix;
out vec3 frag_pos;
flat out vec3 vert_pos;
void main()
{
    gl_Position = uMvpMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vert_pos = aPos;
    frag_pos = aPos;
}
