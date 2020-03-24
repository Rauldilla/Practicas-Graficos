#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Model.h"

#define I glm::mat4(0.0)


void funInit();
void funDisplay();

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
    Shaders shaders;

// Modelos
    Model plane;
    Model cylinder;
    Model cone;
    Model sphere;

// Viewport
   int w = 600;
   int h = 600;

int main(int argc, char** argv) {
    // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3,3);   
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); 
    glutInitContextProfile(GLUT_CORE_PROFILE); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w,h);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Practica-1");
    
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
    
    // Callbacks
    glutDisplayFunc(funDisplay);
    
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
    
    // Models
    plane.initModel("resources/models/plane.obj");
    cylinder.initModel("resources/models/cylinder.obj");
    cone.initModel("resources/models/cone.obj");
    sphere.initModel("resources/models/sphere.obj");
}
 
void funDisplay() {
    
}