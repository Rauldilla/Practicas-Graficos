#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

void funInit();
void funDestroy();
void funReshape(int w, int h);
void funDisplay();
void funKeyboard(unsigned char key, int x, int y);
void funTimer(int value);
void drawTeapot();

GLfloat color[] = {0.0, 0.0, 1.0};

GLint speed   = 500;
bool  dibujar = true;

// size gestiona el tamaño de la tetera. Puede cambiar con el tamaño de la pantalla
GLfloat size = 0.5;

// Las variables x e y alamcenan la dimension original de la pantalla
// y si esta se modifica, la dimension anterior a la modificacion.
GLfloat x = 500.0;
GLfloat y = 500.0;

int main(int argc, char** argv) {

 // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Sesion 1");
    
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
    glutKeyboardFunc(funKeyboard);
    //glutTimerFunc(speed,funTimer,0);
            
 // Bucle principal
    glutMainLoop();
    
 // Liberamos memoria
    funDestroy();
    
    return 0;
}

void funInit() {
    
}

void funDestroy() {
       
}

void funReshape(int w, int h) {
    
    std::cout << "Dimensiones pantalla (pixeles): " << w << "x" << h << "\n";
    
    if ((w < x) || (h < y)) { // La pantalla es mas PEQUEÑA
        size -= 0.1;
    } else { // La pantalla es mas GRANDE
        size += 0.1;
    }
    
    if (w < x) {
        color[2] -= 0.1;
    } else {
        color[2] += 0.1;
    }
    
    // Ahora x e y almacenaran la nueva dimension, porque si esta se modifica
    // asi tenemos la referencia de la dimension anterior. 
    x = w;
    y = h;
}

void funDisplay() {
    
 // Borramos el buffer de color
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawTeapot();
    
 // Intercambiamos los buffers
    glutSwapBuffers();
    
}

void funKeyboard(unsigned char key, int x, int y) {
   
    switch(key) {
        case 'r':
            color[0] = 1.0;
            color[1] = 0.0;
            color[2] = 0.0;  
            break;
        case 'g':
            color[0] = 0.0;
            color[1] = 1.0;
            color[2] = 0.0;  
            break;
        case 'b':
            color[0] = 0.0;
            color[1] = 0.0;
            color[2] = 1.0;  
            break;
        default:
            color[0] = 1.0;
            color[1] = 1.0;
            color[2] = 1.0;  
    }
    glutPostRedisplay();
        
}

void funTimer(int ignore) {
    
    if(dibujar) {
        glColor3f(color[0],color[1],color[2]);
        dibujar = false;
    }
    else {
        glColor3f(0.0, 0.0, 0.0);
        dibujar = true;
    }
    glutPostRedisplay();
    glutTimerFunc(speed,funTimer,0);
    
}

void drawTeapot() {
    glColor3f(color[0],color[1],color[2]);
    
    glutWireTeapot(size);
}
