#include <stdlib.h>
#include<conio.h>
#include <gl/glut.h>
#include <math.h>

#define ListaCilindro 1

void dibujarEjes(void);
void dibujarTapaCilindro(int numPuntos, float radio);
void dibujarCilindro(int numPuntos, int numDivisiones, float radio, float altura);
void dibujarCilindroAlambrico(int numPuntos, int numDivisiones, float radio, float altura);
void dibujarPolea(float radio);
void construye_listas(void);
void girar_objeto_geometrico ();

float angulo = 0.0f, deltaAngulo = 0.0f;

// theta[] me indica los ángulos iniciales en los 3 ejes
static GLfloat theta[] = {0.0,0.0,0.0};
// eje es el ángulo a rotar
static GLint eje = 2;

void dibujarEjes()
{
    //glLoadIdentity();
    glPushMatrix();

    glRotatef(10, 1, 0, 0); //Rotar 10 grados sobre el eje x
    glRotatef(-30, 0, 1.0f, 0); //Rotar -30 grados sobre el eje y

    glLineWidth(2.0f); //Grosor de linea

    glBegin(GL_LINES);

    // Eje X (Rojo)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-50.0f, 0.0f, 0.0f);
    glVertex3f(50.0f, 0.0f, 0.0f);

    // Eje Y (Verde)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -50.0f, 0.0f);
    glVertex3f(0.0f, 50.0f, 0.0f);

    // Eje Z (Azul)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -50.0f);
    glVertex3f(0.0f, 0.0f, 50.0f);

    glPopMatrix();

    glEnd();

    //pintado del plano horizontal
    glColor3f(0.5f, 0.5f, 0.5f);  // Establece el color plomo (RGB: 0.5, 0.5, 0.5)

    glBegin(GL_QUADS);

    glVertex3f(-50.0f, 0.0f, -50.0f);  // Esquina inferior izquierda
    glVertex3f(-50.0f, 0.0f, 50.0f);   // Esquina inferior derecha
    glVertex3f(50.0f, 0.0f, 50.0f);    // Esquina superior derecha
    glVertex3f(50.0f, 0.0f, -50.0f);   // Esquina superior izquierda

    glEnd();

}

void dibujarTapaCilindro(int numPuntos, float radio)
{
    float x, z;
    deltaAngulo = 2 * M_PI / numPuntos;

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0, 0, 0);

    for (int i = 0; i <= numPuntos; i++)
    {
        angulo = i * deltaAngulo;
        x = radio * sin(angulo);
        z = radio * cos(angulo);
        glVertex3f(x, 0, z);
    }

    glEnd();
}

void dibujarCilindro(int numPuntos, int numDivisiones, float radio, float altura)
{
    float deltaAltura, x, y, z;
    deltaAngulo = 2 * M_PI / numPuntos;
    deltaAltura = altura / (float)(numDivisiones - 1);

    // Dibujar laterales del cilindro
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= numPuntos; i++)
    {
        angulo = i * deltaAngulo;
        x = radio * sin(angulo);
        z = radio * cos(angulo);

        glVertex3f(x, -altura / 2, z);
        glVertex3f(x, altura / 2, z);
    }

    glEnd();

    // Dibujar tapa inferior
    glPushMatrix();
    glTranslatef(0, -altura / 2, 0);
    dibujarTapaCilindro(numPuntos, radio);
    glPopMatrix();

    // Dibujar tapa superior
    glPushMatrix();
    glTranslatef(0, altura / 2, 0);
    dibujarTapaCilindro(numPuntos, radio);
    glPopMatrix();
}

void dibujarCilindroAlambrico(int numPuntos, int numDivisiones, float radio, float altura)
{
    float deltaAltura;
    deltaAngulo = 2 * M_PI / numPuntos;
    deltaAltura = altura / (float)(numDivisiones - 1);

    glBegin(GL_LINES);
    for (int i = 0; i < numPuntos; i++)
    {
        angulo = i * deltaAngulo;
        float cosAngulo = cos(angulo);
        float sinAngulo = sin(angulo);

        for (int j = 0; j < numDivisiones; j++)
        {
            float y = -altura / 2 + (float)j * deltaAltura;

            float x1 = radio * sinAngulo;
            float z1 = radio * cosAngulo;
            float x2 = radio * sin((i + 1) * deltaAngulo);
            float z2 = radio * cos((i + 1) * deltaAngulo);

            glVertex3f(x1, y, z1);
            glVertex3f(x2, y, z2);

            // Dibujar líneas verticales para las divisiones
            if (j < numDivisiones - 1)
            {
                glVertex3f(x1, y, z1);
                glVertex3f(x1, y + deltaAltura, z1);
            }
        }
    }
    glEnd();
}

void dibujarPolea(float radio)
{
    glColor3f(1,0,0);
    dibujarCilindro(20,20,radio,1);

    glColor3f(0,0,1);
    dibujarCilindroAlambrico(20,20,radio,1);
}

void construye_listas(void)
{
    glNewList(ListaCilindro,GL_COMPILE);

    //Polea grande
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    glRotatef(90, 1, 0, 0);
    dibujarPolea(1.0f);
    glPopMatrix();

    //Polea pequeña
    glPushMatrix();
    glTranslatef(sqrt(2.0f), 1.5f, 0.0f);
    glRotatef(90, 1, 0, 0);
    dibujarPolea(0.5);
    glPopMatrix();

    glEndList();
}

// dibujamos nuestra escena
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();

    dibujarEjes();

    glRotatef(angulo, 0, 0, 1); // Rotar en el eje Z
    glCallList(ListaCilindro);

    glFlush();
    glutSwapBuffers();
}
// esta función controla el angulo de rotación según el eje de giro
void girar_objeto_geometrico ()
{
    deltaAngulo = 1.0f; // Incremento del ángulo de rotación, puedes ajustar el valor según la velocidad de giro deseada

    angulo += deltaAngulo;

    if (angulo >= 360.0f)
        angulo -= 360.0f; // Restablecer el ángulo a 0 cuando alcanza 360 grados

    glutPostRedisplay(); // Indicar que la escena debe redibujarse
}

void teclado(unsigned char tecla,int x,int y)
{
    switch(tecla)
    {
    case 'A' :

        break;
    case 'S' :

        break;
    case 'D' :

        break;
    case 'F' :

        break;
    case 'Q' :

        break;
    case 'W' :

        break;
    }
}
// control de ventana (recuerde el volumen de visualización)
// modifique dicho volumen según su conveniencia
void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-4.0, 4.0, -4.0 * (GLfloat)h / (GLfloat)w, 4.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-4.0 * (GLfloat)w / (GLfloat)h, 4.0 * (GLfloat)w / (GLfloat)h, -4.0, 4.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,500);
    glutInitWindowPosition(200,100);
    glutCreateWindow("Poleas en movimiento");
    glClearColor(1,1,1,1);
    glutReshapeFunc(myReshape);
    //construye_listas();
    glutDisplayFunc(display);
    //glutIdleFunc(girar_objeto_geometrico);
    //glutKeyboardFunc(teclado);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
