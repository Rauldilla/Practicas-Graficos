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
    mat4 S = mat4(1.0); // Matriz de escalado
    S[0].x = 1.0;
    S[1].y = 1.0;
    S[2].z = 1.0;
    mat4 R = mat4(1.0); // Rotación de theta grados en torno al eje y
    float theta = 0.0;
    R[0].x =  cos(theta*M_PI/180.0);
    R[0].z = -sin(theta*M_PI/180.0);
    R[2].x =  sin(theta*M_PI/180.0);
    R[2].z =  cos(theta*M_PI/180.0);
    mat4 T = mat4(1.0); // Matriz de traslacion
    T[3].x =  0.0;
    T[3].y =  0.0;
    T[3].z = -5.0;
    mat4 M = T*R*S;

    gl_Position = P*V*M * vec4(inPosition,1);

    vColor = uColor;
	
}
