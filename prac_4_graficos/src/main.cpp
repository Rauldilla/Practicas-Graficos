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

void funInit();
void funReshape(int w, int h);
void funDisplay();
void funSpecial(int key, int x, int y);
void funKeyboard(unsigned char key, int x, int y);
void funMouse(int button, int state, int x, int y);
void funMotion(int x, int y);

void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void setLights(glm::mat4 P, glm::mat4 V);

void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
Shaders shaders;

// Modelos
Model modelPlano;
Model modelEsfera;
Model modelCubo;

// Texturas
Texture textureEmissive;
Texture textureNoEmissive;
Texture textureCubeDiffuse;
Texture textureCubeSpecular;
Texture textureWallDiffuse;
Texture textureWallSpecular;
Texture textureWallNormal;
Texture textureLights;

// Viewport
int w = 600;
int h = 600;

// Animaciones
float fovy = 60.0;
float rotX = 0.0;
float rotY = 0.0;
float desZ = 0.0;
float alphaX = 0.0;
float alphaY = -0.174533;

// Luces y materiales
#define   NLD 1
#define   NLP 1
#define   NLF 2
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];
Material matSilver;
Textures texCube;
Textures texWindow;
Textures texPlano;
Textures texLuz;

int main(int argc, char** argv) {

    // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Sesion 7");

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

    // Texturas
    textureEmissive.initTexture("resources/textures/imgEmissive.png");
    textureNoEmissive.initTexture("resources/textures/imgNoEmissive.png");
    textureCubeDiffuse.initTexture("resources/textures/imgCubeDiffuse.png");
    textureCubeSpecular.initTexture("resources/textures/imgCubeSpecular.png");
    textureWallDiffuse.initTexture("resources/textures/imgWallDiffuse.png");
    textureWallSpecular.initTexture("resources/textures/imgWallSpecular.png");
    textureWallNormal.initTexture("resources/textures/imgWallNormal.png");
    textureLights.initTexture("resources/textures/imgLuces.png");

    // Luz ambiental global
    lightG.ambient = glm::vec3(0.9, 0.9, 0.9);

    // Luces direccionales
    lightD[0].direction = glm::vec3(-1.0, 0.0, 0.0);
    lightD[0].ambient = glm::vec3(0.1, 0.1, 0.1);
    lightD[0].diffuse = glm::vec3(0.7, 0.7, 0.7);
    lightD[0].specular = glm::vec3(0.7, 0.7, 0.7);

    // Luces posicionales
    lightP[0].position = glm::vec3(0.0, 3.0, 3.0);
    lightP[0].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightP[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].c0 = 1.00;
    lightP[0].c1 = 0.22;
    lightP[0].c2 = 0.20;

    // Luces focales
    lightF[0].position = glm::vec3(-2.0, 2.0, 5.0);
    lightF[0].direction = glm::vec3(2.0, -2.0, -5.0);
    lightF[0].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightF[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].innerCutOff = 10.0;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 5.0;
    lightF[0].c0 = 1.000;
    lightF[0].c1 = 0.090;
    lightF[0].c2 = 0.032;

    lightF[1].position = glm::vec3(2.0, 2.0, 5.0);
    lightF[1].direction = glm::vec3(-2.0, -2.0, -5.0);
    lightF[1].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightF[1].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightF[1].specular = glm::vec3(0.9, 0.9, 0.9);
    lightF[1].innerCutOff = 5.0;
    lightF[1].outerCutOff = lightF[1].innerCutOff + 1.0;
    lightF[1].c0 = 1.000;
    lightF[1].c1 = 0.090;
    lightF[1].c2 = 0.032;

    // Materiales
    matSilver.ambient = glm::vec4(0.19225f, 0.19225f, 0.19225f, 0.5);
    matSilver.diffuse = glm::vec4(0.50754f, 0.50754f, 0.50754f, 0.5);
    matSilver.specular = glm::vec4(0.508273f, 0.508273f, 0.508273f, 0.5);
    matSilver.emissive = glm::vec4(0.000000, 0.000000, 0.000000, 0.5);
    matSilver.shininess = 51.2f;
    
    texCube.diffuse = textureCubeDiffuse.getTexture();
    texCube.specular = textureCubeSpecular.getTexture();
    texCube.emissive = textureEmissive.getTexture();
    texCube.normal = 0;
    texCube.shininess = 10.0;

    texPlano.diffuse = textureWallDiffuse.getTexture();
    texPlano.specular = textureWallSpecular.getTexture();
    texPlano.emissive = textureNoEmissive.getTexture();
    texPlano.normal = textureWallNormal.getTexture();
    texPlano.shininess = 51.2;
    
    texLuz.diffuse = textureLights.getTexture();
    texLuz.specular = textureLights.getTexture();
    texLuz.emissive = textureNoEmissive.getTexture();
    texLuz.normal = 0;
    texLuz.shininess = 10.0;

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

    // Dibujamos la escena
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
        glm::mat4 M = glm::scale(glm::translate(I, lightP[i].position), glm::vec3(0.1));
        drawObjectTex(modelEsfera, texLuz, P, V, M);
    }

    for (int i = 0; i < NLF; i++) {
        glm::mat4 M = glm::scale(glm::translate(I, lightF[i].position), glm::vec3(0.025));
        drawObjectTex(modelEsfera, texLuz, P, V, M);
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

void drawVentana(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    float rotPlane = 180;

    /* Configuración del suelo */
    glm::mat4 SWindow = glm::scale(I, glm::vec3(X_PLANE, 1.0, Z_PLANE));
    glm::mat4 TWindow = glm::translate(I, glm::vec3(0.0, 2.5, 0.0));
    glm::mat4 RSecond_Window = glm::rotate(I, glm::radians(rotPlane), glm::vec3(0.0, 0.0, 1.0));

    /* Dibuja el suelo */
    glDepthMask(GL_FALSE);
    drawObjectMat(modelPlano, matSilver, P, V, M * SWindow * TWindow);
    drawObjectMat(modelPlano, matSilver, P, V, M * SWindow * TWindow * RSecond_Window);
    glDepthMask(GL_TRUE);
}

void funSpecial(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_UP: rotX -= 5.0f;
            break;
        case GLUT_KEY_DOWN: rotX += 5.0f;
            break;
        case GLUT_KEY_LEFT: rotY -= 5.0f;
            break;
        case GLUT_KEY_RIGHT: rotY += 5.0f;
            break;
        default:
            rotX = 0.0f;
            rotY = 0.0f;
            break;
    }
    glutPostRedisplay();

}

void funKeyboard(unsigned char key, int x, int y) {

    switch (key) {
        case 'e': desZ -= 0.1f;
            break;
        case 'd': desZ += 0.1f;
            break;
        default: desZ = 0.0f;
            break;
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
