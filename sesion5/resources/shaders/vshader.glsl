#version 330 core

layout (location = 0) in vec3 inPosition;

uniform vec3 uColor;
uniform mat4 uPVM;

out vec3 vColor;

void main() {

    gl_Position = uPVM * vec4(inPosition,1);

    vColor = uColor;
	
}
