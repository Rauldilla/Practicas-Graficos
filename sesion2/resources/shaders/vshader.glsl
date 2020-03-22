#version 330 core

#define M_PI 3.1416	

layout (location = 0) in vec3 inPosition;

uniform vec3 uColor;

out vec3 vColor;

void main() {

 // Matriz P
    float fovy   = 60.0;
    float nplane =  0.1;
    float fplane = 25.0;
    mat4 P =  mat4(1.0);
    P[0].x =  1.0/tan(fovy*M_PI/360.0);
    P[1].y =  1.0/tan(fovy*M_PI/360.0);
    P[2].z = -(fplane + nplane) / (fplane - nplane);
    P[2].w = -1.0;
    P[3].z = -2.0 * fplane * nplane / (fplane - nplane);
    P[3].w =  0.0;

 // Matriz V
    mat4 V = mat4(1.0);

 // Matriz M
    mat4 M = mat4(1.0);
    M[3].x =   0.0;
    M[3].y =   5.0;
    M[3].z = -10.0;

    gl_Position = P*V*M * vec4(inPosition,1);

    vColor = uColor;
	
}
