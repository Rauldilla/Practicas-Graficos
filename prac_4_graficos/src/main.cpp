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
#define X_PLANE 3.0
#define Z_PLANE 3.0
#define BASE_SIZE 0.75

void funInit();
void funReshape(int w, int h);
void funDisplay();
void funTimer(int ignore);
void funSpecial(int key, int x, int y);
void funKeyboard(unsigned char key, int x, int y);
void funMouse(int button, int state, int x, int y);
void funMotion(int x, int y);

void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void setLights(glm::mat4 P, glm::mat4 V);

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawMaquina(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBase(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawGancho(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawArticulacion(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSoporte(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawGarras(glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
Shaders shaders;

// Modelos
Model modelPlano;
Model modelEsfera;
Model modelCubo;
Model modelCilindro;
Model modelCono;

// Texturas
Texture textureEmissive;
Texture textureNoEmissive;
Texture textureLights;
Texture textureRedLights;
Texture textureBlueLights;
Texture textureGlass;
Texture textureIron;
Texture textureToyStory;

// Viewport
int w = 600;
int h = 600;

// Animaciones
GLint speed = 20;
GLint speedLights = 0;
float fovy = 60.0;
float rotX = 0.0;
float rotY = 0.0;
float extensionSoporteY = 0.0;
float rotGarra = 70;
float rotGancho = 0;
float movX = 0;
float movZ = 0;
float alphaX = 0.5;
float alphaY = 0.2;
bool rotActivada = true;

// Luces y materiales
#define   NLD 1
#define   NLP 4
#define   NLF 2
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];
Material matCyanPlastic;
Material matBrass;
Material matRedRubber;
Textures texLuz;
Textures texLuzRoja;
Textures texLuzAzul;
Textures texCristal;
Textures texIron;
Textures texToyStory;

int main(int argc, char** argv) {

    // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Practica 4");

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

    // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutTimerFunc(speed, funTimer, 0);
    glutDisplayFunc(funDisplay);
    glutSpecialFunc(funSpecial);
    glutKeyboardFunc(funKeyboard);
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

    // Modelos
    modelPlano.initModel("resources/models/plane.obj");
    modelEsfera.initModel("resources/models/sphere.obj");
    modelCubo.initModel("resources/models/cube.obj");
    modelCilindro.initModel("resources/models/cylinder.obj");
    modelCono.initModel("resources/models/cone.obj");

    // Texturas
    textureEmissive.initTexture("resources/textures/imgEmissive.png");
    textureNoEmissive.initTexture("resources/textures/imgNoEmissive.png");
    textureLights.initTexture("resources/textures/imgLuces.png");
    textureRedLights.initTexture("resources/textures/imgLucesRojas.png");
    textureBlueLights.initTexture("resources/textures/imgLucesAzules.png");
    textureGlass.initTexture("resources/textures/imgGlass.png");
    textureIron.initTexture("resources/textures/imgIron.png");
    textureToyStory.initTexture("resources/textures/imgToyStory.png");

    // Luz ambiental global
    lightG.ambient = glm::vec3(0.9, 0.9, 0.9);

    // Luces direccionales
    lightD[0].direction = glm::vec3(-1.0, 0.0, 0.0);
    lightD[0].ambient = glm::vec3(0.1, 0.1, 0.1);
    lightD[0].diffuse = glm::vec3(0.7, 0.7, 0.7);
    lightD[0].specular = glm::vec3(0.7, 0.7, 0.7);

    // Luces posicionales
    lightP[0].position = glm::vec3(2.9, 0.1, 2.9);
    lightP[0].ambient = glm::vec3(0.0, 0.0, 1.0);
    lightP[0].diffuse = glm::vec3(0.0, 0.0, 1.0);
    lightP[0].specular = glm::vec3(0.0, 0.0, 1.0);
    lightP[0].c0 = 1.00;
    lightP[0].c1 = 0.22;
    lightP[0].c2 = 0.20;
    
    lightP[1].position = glm::vec3(-2.9, 0.1, 2.9);
    lightP[1].ambient = glm::vec3(1.0, 0.0, 0.0);
    lightP[1].diffuse = glm::vec3(1.0, 0.0, 0.0);
    lightP[1].specular = glm::vec3(1.0, 0.0, 0.0);
    lightP[1].c0 = 1.00;
    lightP[1].c1 = 0.22;
    lightP[1].c2 = 0.20;
    
    lightP[2].position = glm::vec3(-2.9, 0.1, -2.9);
    lightP[2].ambient = glm::vec3(0.0, 0.0, 1.0);
    lightP[2].diffuse = glm::vec3(0.0, 0.0, 1.0);
    lightP[2].specular = glm::vec3(0.0, 0.0, 1.0);
    lightP[2].c0 = 1.00;
    lightP[2].c1 = 0.22;
    lightP[2].c2 = 0.20;
    
    lightP[3].position = glm::vec3(2.9, 0.1, -2.9);
    lightP[3].ambient = glm::vec3(1.0, 0.0, 0.0);
    lightP[3].diffuse = glm::vec3(1.0, 0.0, 0.0);
    lightP[3].specular = glm::vec3(1.0, 0.0, 0.0);
    lightP[3].c0 = 1.00;
    lightP[3].c1 = 0.22;
    lightP[3].c2 = 0.20;

    // Luces focales
    lightF[0].position = glm::vec3(3.5, 2.0, 0.0);
    lightF[0].direction = glm::vec3(-3, -2.0, 0.0);
    lightF[0].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightF[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].innerCutOff = 13.0;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 2.0;
    lightF[0].c0 = 1.000;
    lightF[0].c1 = 0.090;
    lightF[0].c2 = 0.032;

    lightF[1].position = glm::vec3(-3.5, 2.0, 0.0);
    lightF[1].direction = glm::vec3(3, -2.0, 0.0);
    lightF[1].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightF[1].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightF[1].specular = glm::vec3(0.9, 0.9, 0.9);
    lightF[1].innerCutOff = 13.0;
    lightF[1].outerCutOff = lightF[1].innerCutOff + 2.0;
    lightF[1].c0 = 1.000;
    lightF[1].c1 = 0.090;
    lightF[1].c2 = 0.032;

    // Materiales
    matCyanPlastic.ambient = glm::vec4(0.0f, 0.1f, 0.06f, 1.0f);
    matCyanPlastic.diffuse = glm::vec4(0.0f, 0.50980392f, 0.50980392f, 1.0f);
    matCyanPlastic.specular = glm::vec4(0.50196078f, 0.50196078f, 0.50196078f, 1.0f);
    matCyanPlastic.emissive = glm::vec4(0.0, 0.0, 0.0, 0.0);
    matCyanPlastic.shininess = 32.0f;

    matBrass.ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f,1.0f);
    matBrass.diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
    matBrass.specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
    matBrass.emissive = glm::vec4(0.0, 0.0, 0.0, 1.0);
    matBrass.shininess = 27.8974f;
    
    matRedRubber.ambient = glm::vec4(0.05f,0.0f,0.0f,1.0f);
    matRedRubber.diffuse = glm::vec4(0.5f,0.4f,0.4f,1.0f);
    matRedRubber.specular = glm::vec4(0.7f,0.04f,0.04f,1.0f);
    matRedRubber.emissive = glm::vec4(0.0, 0.0, 0.0, 1.0);
    matRedRubber.shininess = 10.0f;

    texLuz.diffuse = textureLights.getTexture();
    texLuz.specular = textureLights.getTexture();
    texLuz.emissive = textureNoEmissive.getTexture();
    texLuz.normal = 0;
    texLuz.shininess = 10.0;
    
    texLuzRoja.diffuse = textureRedLights.getTexture();
    texLuzRoja.specular = textureRedLights.getTexture();
    texLuzRoja.emissive = textureNoEmissive.getTexture();
    texLuzRoja.normal = 0;
    texLuzRoja.shininess = 10.0;
    
    texLuzAzul.diffuse = textureBlueLights.getTexture();
    texLuzAzul.specular = textureBlueLights.getTexture();
    texLuzAzul.emissive = textureNoEmissive.getTexture();
    texLuzAzul.normal = 0;
    texLuzAzul.shininess = 10.0;

    texCristal.diffuse = textureGlass.getTexture();
    texCristal.specular = textureGlass.getTexture();
    texCristal.emissive = textureNoEmissive.getTexture();
    texCristal.normal = 0;
    texCristal.shininess = 10.0;
    
    texIron.diffuse = textureIron.getTexture();
    texIron.specular = textureIron.getTexture();
    texIron.emissive = textureNoEmissive.getTexture();
    texIron.normal = 0;
    texIron.shininess = 10.0;
    
    texToyStory.diffuse = textureToyStory.getTexture();
    texToyStory.specular = textureToyStory.getTexture();
    texToyStory.emissive = textureNoEmissive.getTexture();
    texToyStory.normal = 0;
    texToyStory.shininess = 10.0;
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
    float nplane = 0.1;
    float fplane = 25.0;
    float aspect = (float) w / (float) h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

    // Matriz de Vista V (Cámara)
    float x = 10.0 * glm::cos(alphaY) * glm::sin(alphaX);
    float y = 10.0 * glm::sin(alphaY);
    float z = 10.0 * glm::cos(alphaY) * glm::cos(alphaX);
    glm::vec3 pos(x, y, z);
    glm::vec3 lookat(0.0, 0.0, 0.0);
    glm::vec3 up(0.0, 1.0, 0.0);
    glm::mat4 V = glm::lookAt(pos, lookat, up);
    shaders.setVec3("ucpos", pos);

    // Fijamos las luces
    setLights(P, V);

    // Dibujar escena
    glm::mat4 TGancho = glm::translate(I, glm::vec3(movX, 0.0, movZ));  // Para el movimiento en x e y del gancho
    drawSuelo(P, V, I);
    
    glm::mat4 TWindow = glm::translate(I, glm::vec3(0.0, 0.72, 0.0));   // Colocación de gancho y ventana para que no atraviese el suelo
    
    drawMaquina(P, V, I * TGancho * TWindow);
    drawVentana(P, V, I * TWindow);

    // Intercambiamos los buffers
    glutSwapBuffers();

}

void setLights(glm::mat4 P, glm::mat4 V) {

    shaders.setLight("ulightG", lightG);
    for (int i = 0; i < NLD; i++) shaders.setLight("ulightD[" + toString(i) + "]", lightD[i]);
    for (int i = 0; i < NLP; i++) shaders.setLight("ulightP[" + toString(i) + "]", lightP[i]);
    for (int i = 0; i < NLF; i++) shaders.setLight("ulightF[" + toString(i) + "]", lightF[i]);
    
    for (int i = 0; i < NLP; i++) {
        glm::mat4 M = glm::scale(glm::translate(I, lightP[i].position), glm::vec3(0.05));
        
        if(i % 2 == 0) {
            drawObjectTex(modelEsfera, texLuzAzul, P, V, M);
        } else {
            drawObjectTex(modelEsfera, texLuzRoja, P, V, M);
        }
    }
}

void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM", M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uWithMaterials", true);
    shaders.setMaterial("umaterial", material);
    model.renderModel(GL_FILL);

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
    glm::mat4 SSuelo = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TSuelo = glm::translate(I, glm::vec3(0.0, 0.0, 0.0));
    glm::mat4 RSecondSuelo = glm::rotate(I, glm::radians(rotPlane), glm::vec3(0.0, 0.0, 1.0));

    /* Dibuja el suelo */
    drawObjectTex(modelPlano, texIron, P, V, M * SSuelo * TSuelo);
    drawObjectTex(modelPlano, texIron, P, V, M * SSuelo * TSuelo * RSecondSuelo);
}

void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotPlane = 180;

    /* Configuración del ventana */
    glm::mat4 SWindow = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TWindow = glm::translate(I, glm::vec3(0.0, 2.5, 0.0));
    glm::mat4 RSecond_Window = glm::rotate(I, glm::radians(rotPlane), glm::vec3(0.0, 0.0, 1.0));

    /* Dibuja el ventana */
    glDepthMask(GL_FALSE);
    drawObjectTex(modelPlano, texCristal, P, V, M * SWindow * TWindow);
    drawObjectTex(modelPlano, texCristal, P, V, M * SWindow * TWindow * RSecond_Window);
    glDepthMask(GL_TRUE);
}

void drawMaquina(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    // Para extender el gancho
    glm::mat4 TExtension = glm::translate(I, glm::vec3(0.0, extensionSoporteY, 0.0));
    // Para la rotación automática del gancho
    glm::mat4 RGancho = glm::rotate(I, glm::radians(rotGancho), glm::vec3(0.0, 1.0, 0.0));

    // Dibujar gancho
    drawBase(P, V, M);
    drawSoporte(P, V, M);
    drawSoporte(P, V, M * TExtension);
    drawGancho(P, V, M * RGancho * TExtension);

}

void drawBase(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 SBase = glm::scale(I, glm::vec3(BASE_SIZE, 0.1, BASE_SIZE));
    glm::mat4 TBase = glm::translate(I, glm::vec3(0.0, 2.39, 0.0));

    drawObjectMat(modelCubo, matCyanPlastic, P, V, M * TBase * SBase);
}

void drawSoporte(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 SSoporte = glm::scale(I, glm::vec3(0.15, 0.5, 0.15));
    glm::mat4 TSoporte = glm::translate(I, glm::vec3(0.0, 1.8, 0.0));

    drawObjectMat(modelCilindro, matBrass, P, V, M * TSoporte * SSoporte);
}

// TODO refactor del duro
void drawGancho(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    // Articulación
    glm::mat4 TArticulacion = glm::translate(I, glm::vec3(0.0, 1.25, 0.0));
    drawArticulacion(P, V, M * TArticulacion);

    // Garras
    float colocacionGarra = 90;
    glm::mat4 R90 = glm::rotate(I, glm::radians(colocacionGarra), glm::vec3(0.0, 1.0, 0.0));
    
    drawGarras(P, V, M * TArticulacion);
    drawGarras(P, V, M * R90 * TArticulacion);
    drawGarras(P, V, M * R90 * R90 * TArticulacion);
    drawGarras(P, V, M * R90 * R90 * R90 * TArticulacion);
}

void drawArticulacion(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 SArticulacion = glm::scale(I, glm::vec3(0.15, 0.15, 0.15));
    drawObjectTex(modelEsfera, texToyStory, P, V, M * SArticulacion);
}

void drawGarras(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 TGarra = glm::translate(I, glm::vec3(0.0, -1.0, -0.5));
    glm::mat4 RGarra = glm::rotate(I, glm::radians(rotGarra), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 SGarra = glm::scale(I, glm::vec3(0.1, 0.4, 0.1));

    drawObjectMat(modelCubo, matBrass, P, V, M * RGarra * SGarra * TGarra);
    drawObjectMat(modelCubo, matBrass, P, V, M * RGarra * SGarra * TGarra);
    drawObjectMat(modelCubo, matBrass, P, V, M * RGarra * SGarra * TGarra);
    drawObjectMat(modelCubo, matBrass, P, V, M * RGarra * SGarra * TGarra);

    // Articulacion de garras
    glm::mat4 SArticulacionGarra = glm::scale(I, glm::vec3(0.1, 0.1, 0.1));
    glm::mat4 TArticulacionGarra = glm::translate(I, glm::vec3(0.0, -0.8, -0.0));

    drawObjectMat(modelEsfera, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * SArticulacionGarra * TGarra);
    
    drawObjectMat(modelEsfera, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * SArticulacionGarra * TGarra);
    
    drawObjectMat(modelEsfera, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * SArticulacionGarra * TGarra);
    
    drawObjectMat(modelEsfera, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * SArticulacionGarra * TGarra);
    
    // Pinchos
    float rotPincho = 90;
    
    glm::mat4 TPincho = glm::translate(I, glm::vec3(0.0, -0.1, 0.15));
    glm::mat4 RPincho = glm::rotate(I, glm::radians(rotPincho), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 SPincho = glm::scale(I, glm::vec3(0.0722, 0.0722, 0.0722));
    
    drawObjectMat(modelCono, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * TPincho * RPincho * SPincho);
    
    drawObjectMat(modelCono, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * TPincho * RPincho * SPincho);
    
    drawObjectMat(modelCono, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * TPincho * RPincho * SPincho);
    
    drawObjectMat(modelCono, matRedRubber, P, V, M * RGarra
            * TArticulacionGarra * TPincho * RPincho * SPincho);
}

void funTimer(int ignore) {
    
    if(rotActivada) rotGancho += 0.5;
    
    speedLights += speed;
    if(speedLights == 500) {
        speedLights = 0;
        
        if(lightP[0].diffuse != glm::vec3(0.0)) {
            lightP[1].ambient = glm::vec3(1.0, 0.0, 0.0);
            lightP[1].diffuse = glm::vec3(1.0, 0.0, 0.0);
            lightP[1].specular = glm::vec3(1.0, 0.0, 0.0);

            lightP[3].ambient = glm::vec3(1.0, 0.0, 0.0);
            lightP[3].diffuse = glm::vec3(1.0, 0.0, 0.0);
            lightP[3].specular = glm::vec3(1.0, 0.0, 0.0);

            lightP[0].ambient = glm::vec3(0.0);
            lightP[0].diffuse = glm::vec3(0.0);
            lightP[0].specular = glm::vec3(0.0);

            lightP[2].ambient = glm::vec3(0.0);
            lightP[2].diffuse = glm::vec3(0.0);
            lightP[2].specular = glm::vec3(0.0);
        } else {
            lightP[0].ambient = glm::vec3(0.0, 0.0, 1.0);
            lightP[0].diffuse = glm::vec3(0.0, 0.0, 1.0);
            lightP[0].specular = glm::vec3(0.0, 0.0, 1.0);

            lightP[2].ambient = glm::vec3(0.0, 0.0, 1.0);
            lightP[2].diffuse = glm::vec3(0.0, 0.0, 1.0);
            lightP[2].specular = glm::vec3(0.0, 0.0, 1.0);

            lightP[1].ambient = glm::vec3(0.0);
            lightP[1].diffuse = glm::vec3(0.0);
            lightP[1].specular = glm::vec3(0.0);

            lightP[3].ambient = glm::vec3(0.0);
            lightP[3].diffuse = glm::vec3(0.0);
            lightP[3].specular = glm::vec3(0.0);
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(speed, funTimer, 0);

}

void funSpecial(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_UP:
            if(movZ > -2) {
                movZ -= 0.1;
                lightF[0].direction -= glm::vec3(0.0, 0.0, 0.1);
                lightF[1].direction -= glm::vec3(0.0, 0.0, 0.1);
            }
            break;
        case GLUT_KEY_DOWN:
            if(movZ < 2) {
                movZ += 0.1;
                lightF[0].direction += glm::vec3(0.0, 0.0, 0.1);
                lightF[1].direction += glm::vec3(0.0, 0.0, 0.1);
            }
            break;
        case GLUT_KEY_LEFT:
            if(movX > -2) {
                movX -= 0.1;
                lightF[0].direction -= glm::vec3(0.1, 0.0, 0.0);
                lightF[1].direction -= glm::vec3(0.1, 0.0, 0.0);
            }
            break;
        case GLUT_KEY_RIGHT:
            if(movX < 2) {
                movX += 0.1;
                lightF[0].direction += glm::vec3(0.1, 0.0, 0.0);
                lightF[1].direction += glm::vec3(0.1, 0.0, 0.0);
            }
    }
    glutPostRedisplay();

}

void funKeyboard(unsigned char key, int x, int y) {

    switch (key) {
        case 's':
            if (extensionSoporteY >= -1.0) {
                extensionSoporteY -= 0.1f;
            }
            break;
        case 'S':
            if (extensionSoporteY < -0.1) {
                extensionSoporteY += 0.1f;
            }
            break;
        case 'r':
            if(rotGarra > 30) {
                rotGarra -= 0.5;
            }
            break;
        case 'R':
            if(rotGarra <= 70) {
                rotGarra += 0.5;
            }
            break;
        case 'g':
            rotActivada = !rotActivada;
    }
    glutPostRedisplay();

}

void funMouse(int button, int state, int x, int y) {

    switch (button) {
        case 3: fovy -= fovy > 10.0f ? 1.0f: 0.0f;
            break;
        case 4: fovy += fovy < 110.0f ? 1.0f: 0.0f;
            break;
    }
    glutPostRedisplay();

}

void funMotion(int x, int y) {

    float pi = glm::pi<float>();
    float limY = glm::radians(89.0f);

    alphaX = pi * ((float) x / (float) w - 0.5f);
    alphaY = pi * (0.5f - (float) y / (float) h);
    if (alphaY<-limY) alphaY = -limY;
    if (alphaY > limY) alphaY = limY;
    glutPostRedisplay();

}
