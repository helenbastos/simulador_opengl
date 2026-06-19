#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "objetos.h"

// ------------------------------------------------------------
//  desenharObjeto
// ------------------------------------------------------------
void desenharObjeto() {
    switch (objetoAtual) {
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
    }
}

// ------------------------------------------------------------
//  nomeObjetoAtual
// ------------------------------------------------------------
const char* nomeObjetoAtual() {
    const char* nomes[] = {
        "Ponto", "Linha", "Triangulo", "Quad",
        "Cubo", "Esfera", "Teapot", "Cone"
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
    }
}