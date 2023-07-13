/*
    Alumno:
    Fecha:22/12/2022
    Hora: 02:55
    Tema: B-Spline cubico
*/

#include <GL/glut.h>
#include <math.h>

#define LISTA_CURVA_BEZIER 1
#define LISTA_PUNTOS_CONTROL 2
#define LISTA_EJES 3
#define LISTA_POLILINEA 4

void dibujarEjes(void);
void init(void);
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);

float ventanaDimension = 500;    // Tamaño de la ventana
const int maximoPuntosControl = 8;  // Maximo puntos de control en la matriz de puntos
float pcontrol[maximoPuntosControl][2];
int numeroPuntosControl = 0;  // Numero de puntos de control introducidos a la matriz
int posicionPuntoControl = -1; // Guarda la posicion de un punto en la matriz de puntos de control

/*
    Estados iniciales de las opciones del menu Jerarquico.
    OJO: Cuando se active la opcion Mover Puntos esta a su vez descativara la opcion
        ingresar puntos. Esto debido a que se desea evitar que se ingrese un punto
        nuevo en la posicion de un punto ya existente que se desea mover.
*/
bool isIngresoPuntosOn = false;
bool isMostrarPolilineaOn = false;
bool isMostrarBezierOn = false;
bool isMoverPuntosOn = false;
bool isMostrarEjerOn = false;
bool isMostrarPuntosOn =true;
bool colorPuntoControlOPC1=true;
bool colorPuntoControlOPC2=false;
bool colorPuntoControlOPC3=false;
bool coloCurvaBezierOPC1 = true;
bool coloCurvaBezierOPC2=false;
bool coloCurvaBezierOPC3=false;

// Color de punto de control por defecto (morado)
GLfloat redPC = 142.f/255;
GLfloat greenPC =68.f/255;
GLfloat bluePC =173.f/255;

// Color de curva de Bezier por defecto (naranja)
GLfloat redCB = 211.f/255;
GLfloat greenCB =84.f/255;
GLfloat blueCB =0.f/255;

/*
    Nivel de detalle de cada segmento de la curva BSpline
    Siempre debe ser positivo y diferente de 1
*/
unsigned int nivelDetalleCurva = 100;

// Dibuja sistema cartesiano 2D
void dibujarEjes(void)
{
    glBegin(GL_LINES);
    glVertex2f(5, ventanaDimension / 2);
    glVertex2f(ventanaDimension - 5, ventanaDimension / 2);
    glVertex2f(ventanaDimension / 2, ventanaDimension - 5);
    glVertex2f(ventanaDimension / 2, 5);
    glEnd();
}

// Grafica los puntos de control
void graficarPuntosBezier(void)
{
    float x, y;
    glBegin(GL_POINTS);
    for (int i = 0; i < numeroPuntosControl; i++)
    {
        x = pcontrol[i][0];
        y = pcontrol[i][1];
        glVertex2f(x, y);
    }
    glEnd();
}

// Grafica la union de los segmentos que formara la curva B-Spline.
// Grafica los puntos extremos de cada segmento que forme la curva.

float factorial(int n)
{
    float p = 1;
    for (int i = 1; i <= n; i++)
        p = p * (float)i;
    return p;
}

float coeficienteNewton(int n, int k)
{
    return factorial(n) / (factorial(k) * factorial(n - k));
}

float curvaBezier(float u, int coordenada)
{
    float suma = 0.0;
    for (int i = 0; i < numeroPuntosControl; i++)
    {
        suma += pcontrol[i][coordenada] * coeficienteNewton(numeroPuntosControl - 1, i) * pow(u, numeroPuntosControl - 1 - i) * pow(1.0 - u, i);
    }
    return suma;
}

void graficarCurvaBezier(void)
{
    float x, y;
    glBegin(GL_LINE_STRIP);
    for (float u = 0.0; u <= 1; u += 0.01)
    {
        x = curvaBezier(u, 0);
        y = curvaBezier(u, 1);
        glVertex2f(x, y);
    }
    glEnd();
}

// Dibuja la linea que une todos los puntos de control segun orden de ingreso
void dibujarPolilinea()
{
    float x, y;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < numeroPuntosControl; i++)
    {
        x = pcontrol[i][0];
        y = pcontrol[i][1];
        glVertex2f(x, y);
    }
    glEnd();
}

void construyeListas()
{
    glNewList(LISTA_CURVA_BEZIER,GL_COMPILE);
    graficarCurvaBezier();
    glEndList();

    glNewList(LISTA_PUNTOS_CONTROL,GL_COMPILE);
    glColor3f(redPC,greenPC,bluePC);
    graficarPuntosBezier();
    glEndList();

    glNewList(LISTA_EJES,GL_COMPILE);
    dibujarEjes();
    glEndList();

    glNewList(LISTA_POLILINEA,GL_COMPILE);
    dibujarPolilinea();
    glEndList();
}

void actualizarContenidoLista()
{
    glDeleteLists(LISTA_PUNTOS_CONTROL, 1);
    glDeleteLists(LISTA_CURVA_BEZIER,1);

    glNewList(LISTA_CURVA_BEZIER,GL_COMPILE);
    glColor3f(redCB,greenCB,blueCB);
    graficarCurvaBezier();
    glEndList();

    glNewList(LISTA_PUNTOS_CONTROL,GL_COMPILE);
    glColor3f(redPC,greenPC,bluePC);
    graficarPuntosBezier();
    glEndList();

    glNewList(LISTA_EJES,GL_COMPILE);
    dibujarEjes();
    glEndList();

    glNewList(LISTA_POLILINEA,GL_COMPILE);
    dibujarPolilinea();
    glEndList();
}

// Metodo que devolvera la posicion en el arreglo de un punto de control en caso de que el valor de un pixel
// Seleccionado por mouse se considere parecido al suyo segun los criterios establecidos en el metodo
// en este caso dentro de un cuadrado 10*10 alrededor de un punto de control
void existe(int x, int y)
{
    for (int i = 0; i < numeroPuntosControl; i++)
    {
        if (pcontrol[i][0] - 5 <= x && x <= pcontrol[i][0] + 5 && pcontrol[i][1] - 5 <= y && y <= pcontrol[i][1] + 5)
        {
            posicionPuntoControl = i;
        }
    }
}

void myMouse(int button, int state, int x, int y)
{
    // Ingreso de puntos de control mediante mouse
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && isIngresoPuntosOn)
    {
        if (numeroPuntosControl < maximoPuntosControl)
        {
            pcontrol[numeroPuntosControl][0] = x;
            pcontrol[numeroPuntosControl][1] = ventanaDimension - y;
            numeroPuntosControl++;
        }
    }

    // Se determina si el pixel seleccionado con el puntero pertenece a la matriz
    // de puntos de control
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && isMoverPuntosOn)
    {
        // verifica si el pixel seleccionado puede considerarse un punto de control
        existe(x, ventanaDimension - y);
    }

    // Si el pixel seleccionado pertenece a la matriz de puntos se ubica su posicion
    // dentro de esta y se cambia su valor
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && isMoverPuntosOn)
    {
        if (posicionPuntoControl != -1)
        {
            if(x>0 && x<ventanaDimension && y>0 && y<ventanaDimension)
            {
                pcontrol[posicionPuntoControl][0] = x;
                pcontrol[posicionPuntoControl][1] = ventanaDimension - y;
                posicionPuntoControl = -1;
            }
        }
    }
}

// Permitira capturar la posicion del mouse mientras un boton de este se mantenga presionado
// y se arrastre, lo que ayudara a que la curva B-Spline se modifique en tiempo real mientras
// se mueve el mouse
void motion(int x, int y)
{
    if (posicionPuntoControl != -1)
    {
        if(x>=0 && x<=(ventanaDimension) && y>=0 && y<=(ventanaDimension))
        {
            pcontrol[posicionPuntoControl][0] = x;
            pcontrol[posicionPuntoControl][1] = ventanaDimension - y;
        }
    }
}

void init()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, ventanaDimension, 0, ventanaDimension);
}

void actualizar()
{
    glutPostRedisplay();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(6);
    if(isMostrarPuntosOn)
    {
        glCallList(LISTA_PUNTOS_CONTROL);
    }

    if(isMostrarBezierOn)
    {
        glLineWidth(2);
        glCallList(LISTA_CURVA_BEZIER);
    }

    if (isMostrarPolilineaOn)
    {
        glColor3f(0.f, 1.f, 0.f);
        glLineWidth(1);
        glCallList(LISTA_POLILINEA);

    }

    if (isMostrarEjerOn)
    {
        glLineWidth(2);
        glColor3f(1.0, 0.0, 0.0);
        glCallList(LISTA_EJES);
    }
    actualizarContenidoLista();
    glFlush();
    glutSwapBuffers();
}

void menuPrincipal(int opcion)
{
    switch (opcion)
    {
    case 1:
        isIngresoPuntosOn = true;
        break;
    case 2:
        isIngresoPuntosOn = false;
        break;
    case 3:
        isMostrarPolilineaOn = true;
        break;
    case 4:
        isMostrarPolilineaOn = false;
        break;
    case 5:
        isMostrarBezierOn = true;
        break;
    case 6:
        isMostrarBezierOn = false;
        break;
    case 7:
        // Activar mover puntos desactiva el ingreso de puntos para que no se ingrese un nuevo
        // punto el la posicion de un punto ya existente que en realidad se desea mover.
        isMoverPuntosOn = true;
        isIngresoPuntosOn = false;
        break;
    case 8:
        isMoverPuntosOn = false;
        break;
    case 9:
        isMostrarEjerOn = true;
        break;
    case 10:
        isMostrarEjerOn = false;
        break;
    case 11:
        // Morado
        redPC = 142.f/255;
        greenPC =68.f/255;
        bluePC =173.f/255;
        break;
    case 12:
        // Rojo
        redPC = 231.f/255;
        greenPC =76.f/255;
        bluePC =60.f/255;
        break;
    case 13:
        // Celeste
        redPC = 52.f/255;
        greenPC =152.f/255;
        bluePC =219.f/255;
        break;
    case 14:
        // Naranja
        redCB = 211.f/255;
        greenCB =84.f/255;
        blueCB =0.f/255;
        break;
    case 15:
        // Plomo
        redCB = 52.f/255;
        greenCB =73.f/255;
        blueCB =94.f/255;
        break;
    case 16:
        // Rosa
        redCB = 241.f/255;
        greenCB =148.f/255;
        blueCB =138.f/255;
        break;
    case 17:
        isMostrarPuntosOn = true;
        break;
    case 18:
        isMostrarPuntosOn=false;
        break;
    case 19:
        exit(-1);
        break;
    }
}

// Evita que las dimensiones de la ventana se modifiquen
// ya que esto afectaria las formulas usadas para la captura de puntos por mouse
void resize(int width, int height)
{
    //ignoramos los parametros
    glutReshapeWindow(ventanaDimension, ventanaDimension);
    glutPositionWindow(400, 100);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    // Usamos el buffer de profundidad para la determinacion de superficies visibles
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(ventanaDimension, ventanaDimension);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("PC 03");

    int subIngPuntos = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);

    int subMosPolilinea = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Activar", 3);
    glutAddMenuEntry("Desactivar", 4);

    int subMosBezier = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Activar", 5);
    glutAddMenuEntry("Desactivar", 6);

    int subMoverPuntos = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Activar", 7);
    glutAddMenuEntry("Desactivar", 8);

    int subMostrarEjes = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Activar", 9);
    glutAddMenuEntry("Desactivar", 10);

    int subColorPuntoControl = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Opcion 1", 11);
    glutAddMenuEntry("Opcion 2", 12);
    glutAddMenuEntry("Opcion 3", 13);

    int subColorCurvaBezier = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Opcion 1", 14);
    glutAddMenuEntry("Opcion 2", 15);
    glutAddMenuEntry("Opcion 3", 16);

    int subMostrarPuntos= glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Activar", 17);
    glutAddMenuEntry("Desactivar", 18);

    int menu = glutCreateMenu(menuPrincipal);
    glutAddSubMenu("Ingresar Puntos de Control (on/off)", subIngPuntos);
    glutAddSubMenu("Mostrar Polilinea (on/off)", subMosPolilinea);
    glutAddSubMenu("Mostrar curva Bezier (on/off)", subMosBezier);
    glutAddSubMenu("Mover puntos de control (on/off)", subMoverPuntos);
    glutAddSubMenu("Mostrar ejes coordenados (on/off)", subMostrarEjes);
    glutAddSubMenu("Color de punto de control", subColorPuntoControl);
    glutAddSubMenu("Color curva de bezier", subColorCurvaBezier);
    glutAddSubMenu("Mostrar puntos de control", subMostrarPuntos);
    glutAddMenuEntry("Esc", 19);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    construyeListas();
    glutMotionFunc(motion);
    glutIdleFunc(actualizar);
    glutMouseFunc(myMouse);
    glutMainLoop();
    return 0;
}
