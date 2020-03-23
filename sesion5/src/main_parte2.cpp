#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Model.h"

#include <sstream>
std::string toString(const int &i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

#define I glm::mat4(1.0)

void funInit();
void funReshape(int w, int h);
void funDisplay();
void funSpecial(int key, int x, int y);

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawAspa  (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCilindro (glm::mat4 P, glm::mat4 V, glm::mat4 T, glm::mat4 M);
void drawMolino(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void funTimer(int value);

// Shaders
   Shaders shaders;
   
// Modelos
   Model modelPlane;
   Model modelTriangle;
   Model modelCilindro;
   
// Viewport
   int w = 500;
   int h = 500;
   
// Animaciones
   float desZ = 0.0;
   float rotZ = 0.0;
   float speed = 10;
      
int main(int argc, char** argv) {

 // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3,3);   
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); 
    glutInitContextProfile(GLUT_CORE_PROFILE); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w,h);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Sesion 5");
    
 // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout <<"This system supports OpenGL Version: " << oglVersion << std::endl;
    
 // Inicializaciones específicas
    funInit();
    
 // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutSpecialFunc(funSpecial);
    glutTimerFunc(speed,funTimer,0);
              
 // Bucle principal
    glutMainLoop();
    
    return 0;
}

void funInit() {
      
 // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0,1.0);
    
 // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");
    
 // Modelos
    modelPlane.initModel("resources/models/plane.obj");
    modelTriangle.initModel("resources/models/triangle.obj");
    modelCilindro.initModel("resources/models/cylinder.obj");
    
}

void funReshape(int wnew, int hnew) {
    
 // Configuración del Viewport
    glViewport(0, 0, wnew, hnew);

 // Captura de w y h
    w = wnew;
    h = hnew;
    
}

void funDisplay() {
    
 // Borramos el buffer de color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
 // Indicamos los shaders a utilizar
    shaders.useShaders();
    
 // Matriz de Proyección P (Perspectiva)
    float fovy   = 60.0;
    float nplane =  0.1;
    float fplane = 25.0;
    float aspect = (float)w/(float)h;
    glm::mat4 P  = glm::perspective(glm::radians(fovy), aspect, nplane, fplane); 

 // Matriz de Vista V (Cámara)
    glm::vec3 pos   (0.0, 0.0, 10.0);
    glm::vec3 lookat(0.0, 0.0,  0.0);
    glm::vec3 up    (0.0, 1.0,  0.0);
    glm::mat4 V = glm::lookAt(pos, lookat, up);
    
 // Dibujamos la escena
    drawSuelo(P,V,I);
    
    glm::mat4 R = glm::rotate   (I, glm::radians(rotZ), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, 2.0, desZ));

    drawMolino(P, V, T*R);
    
 // Intercambiamos los buffers
    glutSwapBuffers();
    
}

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
 
    shaders.setMat4("uPVM",P*V*M);
    glEnable(GL_POLYGON_OFFSET_FILL);
    shaders.setVec3("uColor",color);
    model.renderModel(GL_FILL);
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    shaders.setVec3("uColor",glm::vec3(1.0, 1.0, 1.0));
    model.renderModel(GL_LINE);
    
}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S = glm::scale(I, glm::vec3(5.0, 1.0, 5.0));
    drawObject(modelPlane,glm::vec3(0.0, 0.0, 1.0),P,V,M*S);

}

void drawCilindro (glm::mat4 P, glm::mat4 V, glm::mat4 Tc, glm::mat4 M) {
    
    glm::mat4 Sc = glm::scale(I, glm::vec3(0.1, 2.0, 1.0));
    drawObject(modelCilindro, glm::vec3(0.0, 1.0, 0.0), P, V, Tc*Sc);
    
}

void drawAspa(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 T = glm::translate(I, glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 SAspa = glm::scale(I, glm::vec3(0.4, 0.8, 0.0));
    drawObject(modelTriangle,glm::vec3(1.0, 0.0, 0.0),P,V,M*T*SAspa);

}

void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    //glm::mat4 R90 = glm::rotate(I, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 R120 = glm::rotate(I, glm::radians(120.0f), glm::vec3(0.0, 0.0, 1.0));
    drawAspa(P,V,M);
    drawAspa(P,V,M*R120);
    drawAspa(P,V,M*R120*R120);

}
    
void funSpecial(int key, int x, int y) {
   
    switch(key) {
        case GLUT_KEY_UP:    desZ -= 0.1;   break;
        case GLUT_KEY_DOWN:  desZ += 0.1;   break;
        case GLUT_KEY_LEFT:  rotZ += 5.0;   break;
        case GLUT_KEY_RIGHT: rotZ -= 5.0;   break;
        default:
           desZ = 0.0;   
           rotZ = 0.0;
           break;
    }
    glutPostRedisplay();
    
}

void funTimer(int value) {
    rotZ += 0.5;
    
    glutPostRedisplay();
    glutTimerFunc(speed,funTimer,0);
}

void drawMolino(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    drawHelice(P,V,M);
    
    glm::mat4 Tc = glm::translate(I, glm::vec3(0.0, 0.0, desZ));
    drawCilindro(P,V,Tc,M);
}