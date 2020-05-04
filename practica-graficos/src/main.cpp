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

#include <sstream>

std::string toString(const int &i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

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

void setLights(glm::mat4 P, glm::mat4 V);
void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawDron(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCupula(glm::mat4 P, glm::mat4 V, glm::mat4 M);
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

// Texturas
Texture textureDiscoDiffuse;
Texture textureDiscoNormal;
Texture textureEmissive;
Texture textureHexagon;
Texture textureNoEmissive;
Texture textureRocks;
Texture textureLuces;

// Viewport
int w = X_VIEWPORT;
int h = Y_VIEWPORT;

// Animaciones
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
bool aspasMoviendose = true;
float intensidadLD = 0.7; // Intensidad luz focal

// Luces
#define   NLD 1
#define   NLP 1
#define   NLF 1
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];

// Materiales
Material matRuby;
Material matGold;
Material matTransparentPolishedBronze;
Material matPolishedBronze;
Material matLuces;
Textures texPlano;
Textures texEsfera;

int main(int argc, char** argv) {
    // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Practica-3");

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
    glEnable(GL_CULL_FACE);

    // Transparencias
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl", "resources/shaders/fshader.glsl");

    // Models
    modelPlano.initModel("resources/models/plane.obj");
    modelCilindro.initModel("resources/models/cylinder.obj");
    modelCono.initModel("resources/models/cone.obj");
    modelEsfera.initModel("resources/models/sphere.obj");

    // Texturas
    textureNoEmissive.initTexture("resources/textures/img1.png");
    textureEmissive.initTexture("resources/textures/img2.png");
    textureHexagon.initTexture("resources/textures/img3.png");
    textureLuces.initTexture("resources/textures/img4.png");
    textureRocks.initTexture("resources/textures/img5.png");
    textureDiscoDiffuse.initTexture("resources/textures/img6.png");
    textureDiscoNormal.initTexture("resources/textures/img7.png");

    // Luz ambiental global
    lightG.ambient = glm::vec3(0.5, 0.5, 0.5);

    // Luces direccionales
    lightD[0].direction = glm::vec3(0.0, -1.0, 0.0);
    lightD[0].ambient = glm::vec3(0.1, 0.1, 0.1);
    lightD[0].diffuse = glm::vec3(0.7, 0.7, 0.7);
    lightD[0].specular = glm::vec3(0.7, 0.7, 0.7);

    // Luces posicionales
    lightP[0].position = glm::vec3(0.0, 0.2, -1.5);
    lightP[0].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightP[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].c0 = 1.00;
    lightP[0].c1 = 0.22;
    lightP[0].c2 = 0.20;

    // Luces focales
    // TODO Ver más tarde lo del innerCutOff y outerCutOff para que se vea como el del profe
    lightF[0].position = glm::vec3(3.0, 3.0, -3.0);
    lightF[0].direction = glm::vec3(-3.0, -3.0, 3.0);
    lightF[0].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightF[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].innerCutOff = 9.0;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 2.0;
    lightF[0].c0 = 1.000;
    lightF[0].c1 = 0.090;
    lightF[0].c2 = 0.032;

    // Materiales
    matRuby.ambient = glm::vec4(0.174500, 0.011750, 0.011750, 1.00);
    matRuby.diffuse = glm::vec4(0.614240, 0.041360, 0.041360, 1.00);
    matRuby.specular = glm::vec4(0.727811, 0.626959, 0.626959, 1.00);
    matRuby.emissive = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    matRuby.shininess = 76.8;

    matGold.ambient = glm::vec4(0.247250, 0.199500, 0.074500, 1.00);
    matGold.diffuse = glm::vec4(0.751640, 0.606480, 0.226480, 1.00);
    matGold.specular = glm::vec4(0.628281, 0.555802, 0.366065, 1.00);
    matGold.emissive = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    matGold.shininess = 51.2;

    matTransparentPolishedBronze.ambient = glm::vec4(0.25f, 0.148f, 0.06475f, 0.75);
    matTransparentPolishedBronze.diffuse = glm::vec4(0.4f, 0.2368f, 0.1036f, 0.75);
    matTransparentPolishedBronze.specular = glm::vec4(0.774597f, 0.458561f, 0.200621f, 0.75);
    matTransparentPolishedBronze.emissive = glm::vec4(0.000000, 0.000000, 0.000000, 0.75);
    matTransparentPolishedBronze.shininess = 76.8f;

    matPolishedBronze.ambient = glm::vec4(0.25f, 0.148f, 0.06475f, 1.00);
    matPolishedBronze.diffuse = glm::vec4(0.4f, 0.2368f, 0.1036f, 1.00);
    matPolishedBronze.specular = glm::vec4(0.774597f, 0.458561f, 0.200621f, 1.00);
    matPolishedBronze.emissive = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    matPolishedBronze.shininess = 76.8f;

    matLuces.ambient = glm::vec4(0.0, 0.0, 0.0, 1.0);
    matLuces.diffuse = glm::vec4(0.0, 0.0, 0.0, 1.0);
    matLuces.specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
    matLuces.emissive = glm::vec4(1.0, 1.0, 1.0, 1.0);
    matLuces.shininess = 1.0;

    texPlano.diffuse = textureRocks.getTexture();
    texPlano.specular = textureHexagon.getTexture();
    texPlano.emissive = textureEmissive.getTexture();
    texPlano.normal = 0;
    texPlano.shininess = 10.0;

    texEsfera.diffuse = textureDiscoDiffuse.getTexture();
    texEsfera.specular = textureDiscoDiffuse.getTexture();
    texEsfera.emissive = textureNoEmissive.getTexture();
    texEsfera.normal = textureDiscoNormal.getTexture();
    texEsfera.shininess = 10.0;
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
    shaders.setVec3("ucpos", pos);

    // Fijar luces
    setLights(P, V);

    // Dibujamos escena
    drawSuelo(P, V, I);
    

    glm::mat4 T_iniPos = glm::translate(I, glm::vec3(animXdron, animYdron, animZdron));
    drawDron(P, V, I * T_iniPos);
    
    // Dibujamos ventana al final por las transparencias
    drawVentana(P, V, I);

    // Intercambiamos los buffers
    glutSwapBuffers();
}

void setLights(glm::mat4 P, glm::mat4 V) {
    shaders.setLight("ulightG", lightG);
    for (int i = 0; i < NLD; i++) shaders.setLight("ulightD[" + toString(i) + "]", lightD[i]);
    for (int i = 0; i < NLP; i++) shaders.setLight("ulightP[" + toString(i) + "]", lightP[i]);
    for (int i = 0; i < NLF; i++) shaders.setLight("ulightF[" + toString(i) + "]", lightF[i]);

    for (int i = 0; i < NLP; i++) {
        glm::mat4 M = glm::scale(glm::translate(I, lightP[i].position), glm::vec3(0.025));
        drawObjectMat(modelEsfera, matLuces, P, V, M);
    }

    for (int i = 0; i < NLF; i++) {
        glm::mat4 M = glm::scale(glm::translate(I, lightF[i].position), glm::vec3(0.025));
        drawObjectMat(modelEsfera, matLuces, P, V, M);
    }
}

void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM", M);
    shaders.setMat4("uPVM", P * V * M);
    //glEnable(GL_POLYGON_OFFSET_FILL);
    //shaders.setVec3("uColor", color);
    shaders.setBool("uWithMaterials", true);
    shaders.setMaterial("umaterial", material);
    model.renderModel(GL_FILL);
    //glDisable(GL_POLYGON_OFFSET_FILL);

    /*shaders.setVec3("uColor", glm::vec3(color.r * 0.75,
            color.g * 0.75,
            color.b * 0.75));
    model.renderModel(GL_LINE);*/
}

void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM", M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uWithMaterials", false);
    if (textures.normal != 0) shaders.setBool("uWithNormals", true);
    else shaders.setBool("uWithNormals", false);
    shaders.setTextures("utextures", textures);
    model.renderModel(GL_FILL);
}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotPlane = 180;

    /* Configuración del suelo */
    glm::mat4 SPlane = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TPlane = glm::translate(I, glm::vec3(0.0, 0.0, 0.0));
    glm::mat4 RSecond_Plane = glm::rotate(I, glm::radians(rotPlane), glm::vec3(0.0, 0.0, 1.0));

    /* Dibuja el suelo */
    drawObjectTex(modelPlano, texPlano, P, V, M * SPlane * TPlane);
    drawObjectTex(modelPlano, texPlano, P, V, M * SPlane * TPlane * RSecond_Plane);
}

void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotWindow = 90.0;
    float rotSecondWindow = 180.0;
    
    /* Configuración del suelo */
    glm::mat4 RWindow = glm::rotate(I, glm::radians(rotWindow), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 RSecond_Window = glm::rotate(I, glm::radians(rotSecondWindow), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 SWindow = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TWindow = glm::translate(I, glm::vec3(0.0, 2.0, 0.0));

    /* Dibuja la ventana */
    glDepthMask(GL_FALSE);
    drawObjectMat(modelPlano, matTransparentPolishedBronze, P, V, M * RWindow * SWindow * TWindow);
    drawObjectMat(modelPlano, matTransparentPolishedBronze, P, V, M * RWindow * SWindow * TWindow * RSecond_Window);
    glDepthMask(GL_TRUE);
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
    
    /* Dibuja la cúpula con transparencias */
    drawCupula(P, V, M);
}

void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    /* Configuración del cuerpo */
    glm::mat4 S_esfera = glm::scale(I, glm::vec3(0.25, 0.1, 0.25));

    /* Dibuja el cuerpo */
    drawObjectTex(modelEsfera, texEsfera, P, V, M * S_esfera);
}

void drawCupula(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    // TODO Revisar tamaño y posición
    glm::mat4 S_cupula = glm::scale(I, glm::vec3(0.15, 0.15, 0.15));
    glm::mat4 T_cupula = glm::translate(I, glm::vec3(0.0, 0.8, 0.0));

    glDepthMask(GL_FALSE);
    drawObjectMat(modelEsfera, matTransparentPolishedBronze, P, V, M * S_cupula * T_cupula);
    glDepthMask(GL_TRUE);
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
    drawObjectMat(modelCilindro, matGold, P, V,
            M * T_cilindro * R_cilindro * S_cilindro);

    drawObjectMat(modelCilindro, matGold, P, V,
            M * R72 * T_cilindro * R_cilindro * S_cilindro);

    drawObjectMat(modelCilindro, matGold, P, V,
            M * R72 * R72 * T_cilindro * R_cilindro * S_cilindro);

    drawObjectMat(modelCilindro, matGold, P, V,
            M * R72 * R72 * R72 * T_cilindro * R_cilindro * S_cilindro);

    drawObjectMat(modelCilindro, matGold, P, V,
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

    drawObjectMat(modelCilindro, matPolishedBronze, P, V,
            M * S_cilindro * T_cilindro);
}

void drawArticulacion(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S_esfera = glm::scale(I, glm::vec3(0.0375, 0.0375, 0.0375));

    /* Dibuja el cuerpo */
    drawObjectTex(modelEsfera, texEsfera, P, V, M * S_esfera);
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
    drawObjectMat(modelCono, matRuby, P, V, M * R_anim * T_cono * R_cono * S_cono);
    drawObjectMat(modelCono, matRuby, P, V, M * R_anim * R90 * T_cono * R_cono * S_cono);
    drawObjectMat(modelCono, matRuby, P, V, M * R_anim * R90 * R90 * T_cono * R_cono * S_cono);
    drawObjectMat(modelCono, matRuby, P, V, M * R_anim * R90 * R90 * R90 * T_cono * R_cono * S_cono);
}

void funTimer(int ignore) {

    if (aspasMoviendose)
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
            break;
        case 's':
            if (aspasMoviendose)
                aspasMoviendose = false;
            else
                aspasMoviendose = true;
            break;
            // TODO Ver manera de mejorar esto (incluido establecer intensidad)
            // TODO Ver qué pasa si cambio solo el ambient
        case 'd':
            if (intensidadLD > 0) {
                intensidadLD -= 0.1;
            }
            break;
        case 'D':
            if (intensidadLD < 1) {
                intensidadLD += 0.1;
            }
            break;
        case 'f':
            // TODO Ver si puedo mejorar esto
            if(lightF[0].diffuse != glm::vec3(0.0)){
                lightF[0].diffuse = glm::vec3(0.0);
                lightF[0].specular = glm::vec3(0.0);
            } else {
                lightF[0].diffuse = glm::vec3(0.9);
                lightF[0].specular = glm::vec3(0.9);
            }
    }

    // Establecer intensidad
    lightD[0].diffuse = glm::vec3(intensidadLD);
    lightD[0].specular = glm::vec3(intensidadLD);

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

void funReshape(int wnew, int hnew) {

    // Configuración del Viewport
    glViewport(0, 0, wnew, hnew);

    // Captura de w y h
    w = wnew;
    h = hnew;
}
