#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

void funInit();
void funDestroy();
void funReshape(int w, int h);
void funDisplay();
void funKeyboard(unsigned char key, int x, int y);
void funTimer(int value);

GLfloat color[] = {0.0, 0.0, 1.0};

GLint speed   = 500;
bool  dibujar = true;

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
    
}

void funDisplay() {
    
 // Borramos el buffer de color
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(color[0],color[1],color[2]);
    
 // Dibujamos una tetera con modelo de alambre
    glutSolidTeapot(1.0);
    
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
