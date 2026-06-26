#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "VGlobais.h"
#include "objetos.h"

#define PI 3.14159265358979323846

// ------------------------------------------------------------
//  Funções auxiliares para formas 2D que o glBegin não tem pronto
// ------------------------------------------------------------

// Desenha um polígono regular de N lados no plano XY
static void desenharPoligonoRegular(int lados, float raio) {
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.85f, 0.55f, 0.15f);
        glVertex3f(0.0f, 0.0f, 0.0f); // centro

        for (int i = 0; i <= lados; i++) {
            float angulo = 2.0f * PI * i / lados;
            float x = raio * cosf(angulo);
            float y = raio * sinf(angulo);
            glColor3f(0.95f, 0.7f, 0.2f);
            glVertex3f(x, y, 0.0f);
        }
    glEnd();

    // Contorno
    glColor3f(0.5f, 0.3f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < lados; i++) {
            float angulo = 2.0f * PI * i / lados;
            glVertex3f(raio * cosf(angulo), raio * sinf(angulo), 0.0f);
        }
    glEnd();
    glLineWidth(1.0f);
}

// Desenha um círculo (polígono com muitos lados)
static void desenharCirculo(float raio) {
    int segmentos = 48;
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.2f, 0.6f, 0.9f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        for (int i = 0; i <= segmentos; i++) {
            float angulo = 2.0f * PI * i / segmentos;
            glColor3f(0.3f, 0.75f, 1.0f);
            glVertex3f(raio * cosf(angulo), raio * sinf(angulo), 0.0f);
        }
    glEnd();
}

// Desenha um cilindro sólido/wireframe usando quadric (gluCylinder)
// Necessário porque GLUT puro não tem glutSolidCylinder
static void desenharCilindro(float raioBase, float altura, int slices, int stacks, bool wire) {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, wire ? GLU_LINE : GLU_FILL);
    gluQuadricNormals(quad, GLU_SMOOTH);

    glPushMatrix();
        // Centraliza o cilindro na origem (gluCylinder cresce em +Z)
        glTranslatef(0.0f, 0.0f, -altura / 2.0f);
        gluCylinder(quad, raioBase, raioBase, altura, slices, stacks);

        // Tampa de baixo
        glPushMatrix();
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(quad, 0.0, raioBase, slices, 1);
        glPopMatrix();

        // Tampa de cima
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, altura);
            gluDisk(quad, 0.0, raioBase, slices, 1);
        glPopMatrix();
    glPopMatrix();

    gluDeleteQuadric(quad);
}
// ------------------------------------------------------------
//  desenharObjeto
// ------------------------------------------------------------
void desenharObjeto() {
    switch (objetoAtual) {

        //  OBJETOS 2D
        case objeto_ponto:
            glDisable(GL_LIGHTING);
            glPointSize(12.0f);
            glBegin(GL_POINTS);
                glColor3f(0.9f, 0.1f, 0.1f); glVertex3f( 0.0f,  0.0f, 0.0f);
                glColor3f(0.1f, 0.6f, 0.9f); glVertex3f( 0.5f,  0.5f, 0.0f);
                glColor3f(0.1f, 0.8f, 0.3f); glVertex3f(-0.5f,  0.3f, 0.0f);
                glColor3f(0.9f, 0.6f, 0.1f); glVertex3f( 0.3f, -0.5f, 0.0f);
            glEnd();
            glEnable(GL_LIGHTING);
            break;

        case objeto_linha:
            glDisable(GL_LIGHTING);
            glLineWidth(2.5f);
            glBegin(GL_LINE_STRIP);
                glColor3f(0.9f, 0.4f, 0.0f);
                glVertex3f(-1.5f, -0.5f, 0.0f);
                glVertex3f(-0.5f,  1.0f, 0.0f);
                glVertex3f( 0.5f, -1.0f, 0.0f);
                glVertex3f( 1.5f,  0.5f, 0.0f);
            glEnd();
            glLineWidth(1.0f);
            glEnable(GL_LIGHTING);
            break;

        case objeto_triangulo:
            glDisable(GL_LIGHTING);
            glBegin(GL_TRIANGLES);
                glColor3f(0.9f, 0.1f, 0.1f); glVertex3f(-1.0f, -0.8f, 0.0f);
                glColor3f(0.1f, 0.7f, 0.2f); glVertex3f( 1.0f, -0.8f, 0.0f);
                glColor3f(0.1f, 0.3f, 0.9f); glVertex3f( 0.0f,  1.0f, 0.0f);
            glEnd();
            glEnable(GL_LIGHTING);
            break;

        case objeto_quadrado:
            glDisable(GL_LIGHTING);
            glBegin(GL_QUADS);
                glColor3f(0.95f, 0.65f, 0.1f);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f,  1.0f, 0.0f);
                glVertex3f(-1.0f,  1.0f, 0.0f);
            glEnd();
            glEnable(GL_LIGHTING);
            break;

        case objeto_poligono:
            glDisable(GL_LIGHTING);
            desenharPoligonoRegular(6, 1.2f); // hexágono
            glEnable(GL_LIGHTING);
            break;

        case objeto_circulo:
            glDisable(GL_LIGHTING);
            desenharCirculo(1.2f);
            glEnable(GL_LIGHTING);
            break;

        //  OBJETOS 3D
         case objeto_cubo:
            glColor3f(0.3f, 0.5f, 0.9f);
            if (modoWire) glutWireCube(1.5);
            else          glutSolidCube(1.5);
            break;

        case objeto_esfera:
            glColor3f(0.2f, 0.7f, 0.5f);
            if (modoWire) glutWireSphere(1.0, 32, 32);
            else          glutSolidSphere(1.0, 32, 32);
            break;

        case objeto_teapot:
            glColor3f(0.8f, 0.3f, 0.3f);
            if (modoWire) glutWireTeapot(1.0);
            else          glutSolidTeapot(1.0);
            break;

        case objeto_cone:
            glColor3f(0.6f, 0.4f, 0.8f);
            if (modoWire) glutWireCone(0.8, 1.5, 20, 20);
            else          glutSolidCone(0.8, 1.5, 20, 20);
            break;

        case objeto_torus:
            glColor3f(0.9f, 0.6f, 0.1f);
            if (modoWire) glutWireTorus(0.35, 0.9, 16, 32);
            else          glutSolidTorus(0.35, 0.9, 16, 32);
            break;

        case objeto_cilindro:
            glColor3f(0.4f, 0.7f, 0.85f);
            desenharCilindro(0.8f, 1.8f, 28, 8, modoWire);
            break;

        case objeto_octaedro:
            glColor3f(0.75f, 0.3f, 0.6f);
            if (modoWire) glutWireOctahedron();
            else          glutSolidOctahedron();
            break;

        case objeto_icosaedro:
            glColor3f(0.35f, 0.55f, 0.3f);
            if (modoWire) glutWireIcosahedron();
            else          glutSolidIcosahedron();
            break;
    }
}

//  objetoAtualE2D
bool objetoAtualE2D() {
    return objetoAtual <= objeto_circulo;
}

// ------------------------------------------------------------
//  nomeObjetoAtual
// ------------------------------------------------------------
const char* nomeObjetoAtual() {
    const char* nomes[] = {
        "Ponto", "Linha", "Triangulo", "Quadrado", "Poligono (Hexagono)", "Circulo",
        "Cubo", "Esfera","Cone", "Teapot","Torus", "Cilindro", "Octaedro", "Icosaedro"
    };
    return nomes[objetoAtual];
}

// ------------------------------------------------------------
//  obterCodigoObjeto — monta string de código + descrição
// ------------------------------------------------------------
void obterCodigoObjeto(char* codigo, char* descricao) {
    switch (objetoAtual) {
        case objeto_ponto:
            sprintf(codigo,   "glPointSize(12.0f); glBegin(GL_POINTS); glVertex3f(x,y,z); glEnd();");
            sprintf(descricao,"GL_POINTS: cada vertice e desenhado como um ponto isolado.");
            break;
        case objeto_linha:
            sprintf(codigo,   "glBegin(GL_LINE_STRIP); glVertex3f(...); glEnd();");
            sprintf(descricao,"GL_LINE_STRIP: conecta os vertices em sequencia com segmentos.");
            break;
        case objeto_triangulo:
            sprintf(codigo,   "glBegin(GL_TRIANGLES); glVertex3f(...) x3; glEnd();");
            sprintf(descricao,"GL_TRIANGLES: primitiva base de toda geometria 3D.");
            break;
        case objeto_quadrado:
            sprintf(codigo,   "glBegin(GL_QUADS); glVertex3f(...) x4; glEnd();");
            sprintf(descricao,"GL_QUADS: define quadrilateros com 4 vertices cada.");
            break;
        case objeto_poligono:
            sprintf(codigo,   "glBegin(GL_TRIANGLE_FAN); glVertex3f(centro); glVertex3f(borda) x6; glEnd();");
            sprintf(descricao,"GL_TRIANGLE_FAN: forma um leque de triangulos a partir do centro - cria poligonos regulares.");
            break;
        case objeto_circulo:
            sprintf(codigo,   "glBegin(GL_TRIANGLE_FAN); glVertex3f(centro); /* 48 pontos na borda */ glEnd();");
            sprintf(descricao,"Circulo = poligono com muitos lados (48 segmentos simulam uma curva).");
            break;

        // ---- 3D ----
        case objeto_cubo:
            sprintf(codigo,   modoWire ? "glutWireCube(1.5);" : "glutSolidCube(1.5);");
            sprintf(descricao,"Cubo GLUT centrado na origem. Lado = 1.5 unidades.");
            break;
        case objeto_esfera:
            sprintf(codigo,   modoWire ? "glutWireSphere(1.0, 32, 32);" : "glutSolidSphere(1.0, 32, 32);");
            sprintf(descricao,"Esfera: raio=1.0, slices=32 (verticais), stacks=32 (horizontais).");
            break;
        case objeto_teapot:
            sprintf(codigo,   modoWire ? "glutWireTeapot(1.0);" : "glutSolidTeapot(1.0);");
            sprintf(descricao,"Bule de cha de Utah - icone historico da computacao grafica (1975).");
            break;
        case objeto_cone:
            sprintf(codigo,   modoWire ? "glutWireCone(0.8,1.5,20,20);" : "glutSolidCone(0.8,1.5,20,20);");
            sprintf(descricao,"Cone: base=0.8, altura=1.5, slices=20, stacks=20.");
            break;
        case objeto_torus:
            sprintf(codigo,   modoWire ? "glutWireTorus(0.35,0.9,16,32);" : "glutSolidTorus(0.35,0.9,16,32);");
            sprintf(descricao,"Torus (rosquinha): raio interno=0.35, raio externo=0.9.");
            break;
        case objeto_cilindro:
            sprintf(codigo,   "gluCylinder(quad, 0.8, 0.8, 1.8, 28, 8); // + 2 gluDisk para tampas");
            sprintf(descricao,"GLU quadric: cilindro nao existe pronto no GLUT, usa-se a biblioteca GLU.");
            break;
        case objeto_octaedro:
            sprintf(codigo,   modoWire ? "glutWireOctahedron();" : "glutSolidOctahedron();");
            sprintf(descricao,"Octaedro: solido com 8 faces triangulares (poliedro de Platao).");
            break;
        case objeto_icosaedro:
            sprintf(codigo,   modoWire ? "glutWireIcosahedron();" : "glutSolidIcosahedron();");
            sprintf(descricao,"Icosaedro: solido com 20 faces triangulares (poliedro de Platao).");
            break;
    }
}