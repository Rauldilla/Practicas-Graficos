#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Model.h"

#define I glm::mat4(1.0)
#define UPPER_FOVY 60.0
#define LOWER_FOVY 10.0
#define X_PLANE 2.0
#define Z_PLANE 2.0
#define X_VIEWPORT 600
#define Y_VIEWPORT 600


void funInit();
void funReshape(int w, int h);
void funDisplay();
void funTimer(int value);
void funKeyboard(unsigned char key, int x, int y);
void funSpecial(int key, int x, int y);
void funMouse(int button, int state, int x, int y);
void funMotion(int x, int y);

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawDron(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazos(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelices(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSoporte(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawArticulacion(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawAspas(glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
Shaders shaders;

// Modelos
Model modelPlano;
Model modelCilindro;
Model modelCono;
Model modelEsfera;

// Viewport
int w = X_VIEWPORT;
int h = Y_VIEWPORT;

GLint speed = 20;
float animRotAspa = 0;
float animRotDron = 0;
float animRotSoportes = 0;
float animYdron = 0.2;
float animXdron = 0.0;
float animZdron = 0.0;
float fovyModifier = 60.0;
float alphaX = 0.0;
float alphaY = 0.174533;

int main(int argc, char** argv) {
    // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Practica-1");

    // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

    // Inicializaciones específicas
    funInit();

    // Callbacks
    glutDisplayFunc(funDisplay);
    glutReshapeFunc(funReshape);
    glutTimerFunc(speed, funTimer, 0);
    glutKeyboardFunc(funKeyboard);
    glutSpecialFunc(funSpecial);
    glutMouseFunc(funMouse);
    glutMotionFunc(funMotion);

    // Bucle principal
    glutMainLoop();

    return 0;
}

void funInit() {

    // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0, 1.0);

    // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl", "resources/shaders/fshader.glsl");

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
    float fovy = fovyModifier;
    float nplane = 0.1;
    float fplane = 25.0;
    float aspect = (float) w / (float) h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

    // Matriz de Vista V (Camara)
    float x = 5.0f * glm::cos(alphaY) * glm::sin(alphaX);
    float y = 5.0f * glm::sin(alphaY);
    float z = 5.0f * glm::cos(alphaY) * glm::cos(alphaX);

    glm::vec3 pos(x, y, z);
    glm::vec3 lookat(0.0, 0.0, 0.0);
    glm::vec3 up(0.0, 1.0, 0.0);
    glm::mat4 V = glm::lookAt(pos, lookat, up);

    // Dibujamos escena
    drawSuelo(P, V, I);
    drawVentana(P, V, I);

    glm::mat4 T_iniPos = glm::translate(I, glm::vec3(animXdron, animYdron, animZdron));
    drawDron(P, V, I * T_iniPos);

    // Intercambiamos los buffers
    glutSwapBuffers();
}

void funReshape(int wnew, int hnew) {
    
    // Configuración del Viewport
    glViewport(0, 0, wnew, hnew);
    
    // Captura de w y h
    w = wnew;
    h = hnew;
}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    /* Configuración del suelo */
    glm::mat4 SPlane = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TPlane = glm::translate(I, glm::vec3(0.0, 0.0, 0.0));

    /* Dibuja el suelo */
    drawObject(modelPlano, glm::vec3(0.40, 0.40, 0.40), P, V, M * SPlane * TPlane);
}

void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotWindow = 90.0;
    /* Configuración del suelo */
    glm::mat4 RWindow = glm::rotate(I, glm::radians(rotWindow), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 SWindow = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TWindow = glm::translate(I, glm::vec3(0.0, 2.0, 0.0));

    /* Dibuja el suelo */
    drawObject(modelPlano, glm::vec3(0.40, 0.40, 0.40), P, V, M * RWindow * SWindow * TWindow);
}

void drawObject(Model model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uPVM", P * V * M);
    glEnable(GL_POLYGON_OFFSET_FILL);
    shaders.setVec3("uColor", color);
    model.renderModel(GL_FILL);
    glDisable(GL_POLYGON_OFFSET_FILL);

    shaders.setVec3("uColor", glm::vec3(color.r * 0.75,
            color.g * 0.75,
            color.b * 0.75));
    model.renderModel(GL_LINE);
}

void drawDron(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    // Añadir animación al dron
    glm::mat4 R_dron = glm::rotate(I, glm::radians(animRotDron), glm::vec3(0.0, 1.0, 0.0));
    M = M*R_dron;

    /* Dibuja el cuerpo */
    drawCuerpo(P, V, M);

    /* Dibuja los brazos */
    drawBrazos(P, V, M);

    /* Dibuja las hélices */
    drawHelices(P, V, M);
    drawHelices(P, V, M);
    drawHelices(P, V, M);
    drawHelices(P, V, M);
    drawHelices(P, V, M);
}

void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    /* Configuración del cuerpo */
    glm::mat4 S_esfera = glm::scale(I, glm::vec3(0.5, 0.2, 0.5));

    /* Dibuja el cuerpo */
    drawObject(modelEsfera, glm::vec3(0.0, 1.0, 0.0), P, V, M * S_esfera);
}

void drawBrazos(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotZ = 90.0;
    float rotY = 72.0;

    /* Configuración de los cilindros */
    glm::mat4 R_cilindro = glm::rotate(I, glm::radians(rotZ), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 R72 = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 S_cilindro = glm::scale(I, glm::vec3(0.05, 0.5, 0.05));
    // TODO mover T_cilindro fuera del método
    glm::mat4 T_cilindro = glm::translate(I, glm::vec3(0.5, 0.0, 0.0));

    /* Dibuja cinco brazos */
    // TODO Refactorizar
    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M * T_cilindro * R_cilindro * S_cilindro);

    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M * R72 * T_cilindro * R_cilindro * S_cilindro);

    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M * R72 * R72 * T_cilindro * R_cilindro * S_cilindro);

    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M * R72 * R72 * R72 * T_cilindro * R_cilindro * S_cilindro);

    drawObject(modelCilindro, glm::vec3(0.0, 0.0, 1.0), P, V,
            M * R72 * R72 * R72 * R72 * T_cilindro * R_cilindro * S_cilindro);
}

void drawHelices(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    // Variables para situar brazos, articulaciones y soportes en el lado
    // y con el ángulo correctos
    float rotY = 72.0;
    glm::mat4 R72 = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 T = glm::translate(I, glm::vec3(1.0, 0.0, 0.0));

    glm::mat4 R = glm::rotate(I, glm::radians(animRotSoportes),
            glm::vec3(0.0, 0.0, 1.0));

    /* Dibuja las articulaciones */
    drawArticulacion(P, V, M * T);
    drawArticulacion(P, V, M * R72 * T);
    drawArticulacion(P, V, M * R72 * R72 * T);
    drawArticulacion(P, V, M * R72 * R72 * R72 * T);
    drawArticulacion(P, V, M * R72 * R72 * R72 * R72 * T);

    /* Dibuja los soportes */
    drawSoporte(P, V, M * T * R);
    drawSoporte(P, V, M * R72 * T * R);
    drawSoporte(P, V, M * R72 * R72 * T * R);
    drawSoporte(P, V, M * R72 * R72 * R72 * T * R);
    drawSoporte(P, V, M * R72 * R72 * R72 * R72 * T * R);

    /* Configuración de las aspas */
    float rotInicial = 45.0;

    // Colocación inicial de las aspas
    glm::mat4 T_helice = glm::translate(I, glm::vec3(0.0, 0.20, 0.0));
    glm::mat4 R_rotInicial = glm::rotate(I, glm::radians(rotInicial), glm::vec3(0.0, 1.0, 0.0));

    /* Dibuja las aspas */
    drawAspas(P, V, M * T * R * T_helice * R_rotInicial);
    drawAspas(P, V, M * R72 * T * R * T_helice * R_rotInicial);
    drawAspas(P, V, M * R72 * R72 * T * R * T_helice * R_rotInicial);
    drawAspas(P, V, M * R72 * R72 * R72 * T * R * T_helice * R_rotInicial);
    drawAspas(P, V, M * R72 * R72 * R72 * R72 * T * R * T_helice * R_rotInicial);
}

void drawSoporte(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S_cilindro = glm::scale(I, glm::vec3(0.025, 0.1, 0.025));
    glm::mat4 T_cilindro = glm::translate(I, glm::vec3(0.0, 1.0, 0.0));

    drawObject(modelCilindro, glm::vec3(1.0, 1.0, 0.0), P, V,
            M * S_cilindro * T_cilindro);
}

void drawArticulacion(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S_esfera = glm::scale(I, glm::vec3(0.075, 0.075, 0.075));

    /* Dibuja el cuerpo */
    drawObject(modelEsfera, glm::vec3(1.0, 0.0, 1.0), P, V, M * S_esfera);
}

void drawAspas(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotY = 90.0;

    /* Configuración de aspas */
    glm::mat4 S_cono = glm::scale(I, glm::vec3(0.01116484395, 0.06, 0.03721614651));
    glm::mat4 R_cono = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 R_anim = glm::rotate(I, glm::radians(animRotAspa), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 R90 = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 T_cono = glm::translate(I, glm::vec3(0.16, 0.0, 0.0));

    /* Dibuja cuatro aspas */
    // TODO Refactorizar
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M * R_anim * T_cono * R_cono * S_cono);
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M * R_anim * R90 * T_cono * R_cono * S_cono);
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M * R_anim * R90 * R90 * T_cono * R_cono * S_cono);
    drawObject(modelCono, glm::vec3(1.0, 0.0, 0.0), P, V, M * R_anim * R90 * R90 * R90 * T_cono * R_cono * S_cono);
}

void funTimer(int ignore) {

    animRotAspa += 5;

    glutPostRedisplay();
    glutTimerFunc(speed, funTimer, 0);

}

void funKeyboard(unsigned char key, int x, int y) {

    switch (key) {
        case 'r':
            animRotDron -= 5.0;
            break;
        case 'R':
            animRotDron += 5.0;
            break;
        case 'a':
            if (animRotSoportes <= 0 && animRotSoportes > -180) {
                animRotSoportes -= 5;
            }
            break;
        case 'A':
            if (animRotSoportes < 0 && animRotSoportes >= -180) {
                animRotSoportes += 5;
            }
            break;
        case 'y':
            if (animYdron < 1.95) {
                animYdron += 0.05;
            }
            break;
        case 'Y':
            // TODO check this
            if (animYdron >= 0.25) {
                animYdron -= 0.05;
            }
            break;
        case 'p':
            std::cout << animYdron << "\n";
    }
    glutPostRedisplay();

}

void funSpecial(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_UP:
            if (animZdron < (Z_PLANE - 0.5)) {
                animZdron += 0.1;
            }
            break;
        case GLUT_KEY_DOWN:
            if (animZdron > (-Z_PLANE + 0.5)) {
                animZdron -= 0.1;
            }
            break;
        case GLUT_KEY_LEFT:
            if (animXdron > (-X_PLANE + 0.5)) {
                animXdron -= 0.1;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (animXdron < (X_PLANE - 0.5)) {
                animXdron += 0.1;
            }
            break;
        default:
            animZdron = 0.0;
            animXdron = 0.0;
            break;
    }
    glutPostRedisplay();
}

void funMouse(int button, int state, int x, int y) {

    switch (button) {
        case 3: // SCROLL UP
            if ((fovyModifier + 1.0 >= LOWER_FOVY) && (fovyModifier + 1.0 <= UPPER_FOVY)) {
                fovyModifier += 1.0;
            }
            break;
        case 4: // SCROLL DOWN
            if ((fovyModifier >= LOWER_FOVY) && (fovyModifier <= UPPER_FOVY)) {
                fovyModifier -= 1.0;
            }
            break;
    }
    glutPostRedisplay();
}

void funMotion(int x, int y) {

    /* Para esta parte hay que tener en cuenta que la x y la y que llegan al 
     método son partiendo de la esquina superior izquierda del viewport,
     mientras que a nosotros nos interesa que sean en base al CENTRO
     del viewport. */

    float xViewport;
    float yViewport;
    float mitadX = w / 2;
    float mitadY = h / 2;

    float calculoAlphaX;
    float calculoAlphaY;

    if (x < mitadX) {
        xViewport = -(mitadX - x); // Resultado en PIXELES
    } else {
        xViewport = x - mitadX; // Resultado en PIXELES
    }

    if (y < mitadY) {
        yViewport = mitadY - y; // Resultado en PIXELES
    } else {
        yViewport = -(y - mitadY); // Resultado en PIXELES
    }

    // Regla de tres para pasar de pixeles a grados
    calculoAlphaX = (xViewport * 90) / mitadX; // Resultado en GRADOS
    calculoAlphaY = (yViewport * 90) / mitadY; // Resultado en GRADOS

    if ((calculoAlphaX >= (-179)) && (calculoAlphaX <= 179)) {
        alphaX = ((calculoAlphaX * M_PI) / 180); // Resultado en RADIANES
    }
    if ((calculoAlphaY >= (-89)) && (calculoAlphaY <= 89)) {
        alphaY = ((calculoAlphaY * M_PI) / 180); // Resultado en RADIANES
    }
}
