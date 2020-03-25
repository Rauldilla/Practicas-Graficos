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
void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawDron(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazos(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
    Shaders shaders;

// Modelos
    Model modelPlano;
    Model modelCilindro;
    Model modelCono;
    Model modelEsfera;

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
    modelPlano.initModel("resources/models/plane.obj");
    modelCilindro.initModel("resources/models/cylinder.obj");
    modelCono.initModel("resources/models/cone.obj");
    modelEsfera.initModel("resources/models/sphere.obj");
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
    
    // APARTADO 4
    //glm::vec3 pos   (4.0, 2.0, 4.0);
    
    /* 3a */
    /*float rotY = 27.0;
    glm::mat4 T_vuelo = glm::translate(I, glm::vec3(0.0, 0.5, 0.0));
    glm::mat4 R_dron = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 T_atras = glm::translate(I, glm::vec3(-2.0, 0.0, -2.0));
    
    drawDron(P, V, I*T_vuelo*T_atras*R_dron);*/
    
    /* 3b */
    
    
    // FIN APARTADO 4
    
    drawDron(P, V, I);
    
    // Intercambiamos los buffers
    glutSwapBuffers();
}

void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 SPlane = glm::scale(I, glm::vec3(2.0, 1.0, 2.0));
    // TODO Checkear que solo la y = 0
    glm::mat4 TPlane = glm::translate(I, glm::vec3(0.0, 0.0, 0.0));
    
    drawObject(modelPlano, glm::vec3(0.40, 0.40, 0.40), P, V, M*SPlane*TPlane);
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

void drawDron(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    drawCuerpo(P, V, M);
    
    drawBrazos(P, V, M);
    
    float rotYhelice = 72.0;
    float rotInicial = 45.0;
    
    glm::mat4 R72Helices = glm::rotate(I, glm::radians(rotYhelice),glm::vec3(0.0, 1.0, 0.0));
    // Situar hélices en la punta de los brazos
    glm::mat4 T_helicePos = glm::translate(I, glm::vec3(1.0, 0.0, 0.0));
    // Colocación inicial de las hélices
    glm::mat4 R_rotInicial = glm::rotate(I, glm::radians(rotInicial),glm::vec3(0.0, 1.0, 0.0));
    
    drawHelice(P, V, M*T_helicePos*R_rotInicial);
    drawHelice(P, V, M*R72Helices*T_helicePos*R_rotInicial);
    drawHelice(P, V, M*R72Helices*R72Helices*T_helicePos*R_rotInicial);
    drawHelice(P, V, M*R72Helices*R72Helices*R72Helices*T_helicePos*R_rotInicial);
    drawHelice(P, V, M*R72Helices*R72Helices*R72Helices*R72Helices*T_helicePos*R_rotInicial);
}

void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S_esfera = glm::scale(I, glm::vec3(0.5, 0.2, 0.5));
    drawObject(modelEsfera, glm::vec3(0.0, 1.0, 0.0), P, V, M*S_esfera);
}

void drawBrazos(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    float rotZ = 90.0;
    float rotY = 72.0;
    
    /* Configuración de los cilindros */
    glm::mat4 R_cilindro = glm::rotate(I, glm::radians(rotZ), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 R72 = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 S_cilindro = glm::scale(I, glm::vec3(0.05, 0.5, 0.05));
    // TODO moverlo fuera
    glm::mat4 T_cilindro = glm::translate(I, glm::vec3(0.5, 0.0, 0.0));
    
    // TODO Refactorizar
    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M*T_cilindro*R_cilindro*S_cilindro);
    
    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M*R72*T_cilindro*R_cilindro*S_cilindro);
    
    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M*R72*R72*T_cilindro*R_cilindro*S_cilindro);
    
    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M*R72*R72*R72*T_cilindro*R_cilindro*S_cilindro);
    
    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M*R72*R72*R72*R72*T_cilindro*R_cilindro*S_cilindro);
}

void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    float rotY = 90.0;
    
    /* Configuración de aspas */
    glm::mat4 S_cono = glm::scale(I, glm::vec3(0.005, 0.06, 0.035));
    glm::mat4 R_cono = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 R90 = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 T_cono = glm::translate(I, glm::vec3(0.16, 0.0, 0.0));
    
    // Dibuja cuatro aspas
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M*T_cono*R_cono*S_cono);
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M*R90*T_cono*R_cono*S_cono);
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M*R90*R90*T_cono*R_cono*S_cono);
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M*R90*R90*R90*T_cono*R_cono*S_cono);
}