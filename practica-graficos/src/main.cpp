#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Model.h"

#define I glm::mat4(1.0)

#define NUM_ARMS 5

void funInit();
void funDisplay();

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
    Shaders shaders;

// Modelos
    Model modelPlane;
    Model modelCylinder;
    Model modelCone;
    Model modelSphere;

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
    modelPlane.initModel("resources/models/plane.obj");
    modelCylinder.initModel("resources/models/cylinder.obj");
    modelCone.initModel("resources/models/cone.obj");
    modelSphere.initModel("resources/models/sphere.obj");
}
 
void funDisplay() {
    
    // Borramos el buffer de color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Shaders
    shaders.useShaders();
    
    // Matriz de Proyeccion P (Perspectiva)
    float fovy   = 30.0;
    float nplane =  0.1;
    float fplane = 25.0;
    float aspect = (float)w/(float)h;
    glm::mat4 P  = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);
    
    // Matriz de Vista V (Camara)
    glm::vec3 pos   (4.0, 4.0, 4.0);
    glm::vec3 lookat(0.0, 0.0,  0.0);
    glm::vec3 up    (0.0, 1.0,  0.0);
    glm::mat4 V = glm::lookAt(pos, lookat, up);
    
    // Dibujamos escena
    drawSuelo(P, V, I);
    
    // Cuerpo
    //glm::mat4 SSphere = glm::scale(I, glm::vec3(0.5, 0.2, 0.5));
    //drawObject(modelSphere, glm::vec3(0.0, 1.0, 0.0), P, V, I*SSphere);
    
    // Brazos
    float rotZ = 90.0;
    float rotYCilindro = 72.0;
    glm::mat4 RCylinder = glm::rotate   (I, glm::radians(rotZ), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 R72 = glm::rotate   (I, glm::radians(rotYCilindro), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 SCylinder = glm::scale(I, glm::vec3(0.05, 0.5, 0.05));
    glm::mat4 TCylinder = glm::translate(I, glm::vec3(0.5, 0.0, 0.0));
    
    // TODO Refactor display arms
    /*for(int i = 0; i < NUM_ARMS; i++) {
        glm::mat4 I2 = glm::scale(I, glm::vec3(i, i, i));
        
        drawObject(modelCylinder, glm::vec3(0.0, 0.0, 1.0), P, V,
                I2*R72*TCylinder*RCylinder*SCylinder);
    }*/
    /*drawObject(modelCylinder, glm::vec3(0.0, 0.0, 1.0), P, V, I*TCylinder*RCylinder*SCylinder);
    drawObject(modelCylinder, glm::vec3(0.0, 0.0, 1.0), P, V, I*R72*TCylinder*RCylinder*SCylinder);
    drawObject(modelCylinder, glm::vec3(0.0, 0.0, 1.0), P, V, I*R72*R72*TCylinder*RCylinder*SCylinder);
    drawObject(modelCylinder, glm::vec3(0.0, 0.0, 1.0), P, V, I*R72*R72*R72*TCylinder*RCylinder*SCylinder);
    drawObject(modelCylinder, glm::vec3(0.0, 0.0, 1.0), P, V, I*R72*R72*R72*R72*TCylinder*RCylinder*SCylinder);*/
    
    // Blades
    // TODO Change scale to the correct one
    glm::mat4 SCone = glm::scale(I, glm::vec3(0.005, 0.06, 0.035));
    glm::mat4 RCone = glm::rotate   (I, glm::radians(rotZ), glm::vec3(0.0, 0.0, 1.0));
    drawObject(modelCone, glm::vec3(1.0, 0.0, 0.0), P, V, I*RCone*SCone);
    
    // Intercambiamos los buffers
    glutSwapBuffers();
}

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
   
    shaders.setMat4("uPVM",P*V*M);
    glEnable(GL_POLYGON_OFFSET_FILL);
    shaders.setVec3("uColor",color);
    model.renderModel(GL_FILL);
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    shaders.setVec3("uColor",glm::vec3(color.r*0.75,
                                       color.g*0.75,
                                       color.b*0.75));
    model.renderModel(GL_LINE);
}

void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    // Escala
    glm::mat4 SPlane = glm::scale(I, glm::vec3(2.0, 1.0, 2.0));
    // Posición
    // TODO Checkear que solo la y = 0
    glm::mat4 TPlane = glm::translate(I, glm::vec3(0.0, 0.0, 0.0));
    
    drawObject(modelPlane, glm::vec3(0.40, 0.40, 0.40), P, V, M*SPlane*TPlane);
}