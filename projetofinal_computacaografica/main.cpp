#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "objetos.h"

// ============================================================
//  Estado de interação com o mouse
// ============================================================
static int   mouseX = 0, mouseY = 0;
static int   botaoPressionado = -1;
static float anguloX = 20.0f;
static float anguloY = -30.0f;
static float zoom    =  6.0f;

// Largura do painel lateral (em pixels)
#define LARGURA_PAINEL_LATERAL 200
#define ALTURA_PAINEL_RODAPE    80

// ============================================================
//  renderizarTexto
// ============================================================
void renderizarTexto(float x, float y, const char* texto,
                     float r, float g, float b) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, larguraJanela, 0, alturaJanela);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = texto; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
//  desenharPainelLateral — menu fixo à esquerda
// ============================================================
struct ItemMenu {
    const char* nome;
    Modulo      modulo;
};

static ItemMenu itensMenu[] = {
    { "Objetos",        mod_objetos        },
    { "Transformacoes", mod_transformacoes },
    { "Projecoes",      mod_projecoes      },
    { "Iluminacao",     mod_iluminacao     },
    { "Texturas",       mod_texturas       },
    { "Curvas",         mod_curvas         }
};
static const int NUM_ITENS_MENU = 6;

void desenharPainelLateral() {
    // Fundo cinza-claro do painel
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, larguraJanela, 0, alturaJanela);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor3f(0.92f, 0.92f, 0.94f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(LARGURA_PAINEL_LATERAL, 0);
        glVertex2f(LARGURA_PAINEL_LATERAL, alturaJanela);
        glVertex2f(0, alturaJanela);
    glEnd();

    // Linha separadora
    glColor3f(0.7f, 0.7f, 0.75f);
    glBegin(GL_LINES);
        glVertex2f(LARGURA_PAINEL_LATERAL, 0);
        glVertex2f(LARGURA_PAINEL_LATERAL, alturaJanela);
    glEnd();

    // Itens do menu
    float yInicial = alturaJanela - 50.0f;
    float espaco   = 50.0f;

    for (int i = 0; i < NUM_ITENS_MENU; i++) {
        float y = yInicial - i * espaco;

        // Retângulo de destaque se selecionado
        if (itensMenu[i].modulo == moduloAtual) {
            glColor3f(0.25f, 0.5f, 0.95f);
            glBegin(GL_QUADS);
                glVertex2f(5,                         y - 14);
                glVertex2f(LARGURA_PAINEL_LATERAL - 5, y - 14);
                glVertex2f(LARGURA_PAINEL_LATERAL - 5, y + 18);
                glVertex2f(5,                          y + 18);
            glEnd();
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        // Texto (branco se selecionado, escuro se não)
        if (itensMenu[i].modulo == moduloAtual)
            renderizarTexto(20, y, itensMenu[i].nome, 1.0f, 1.0f, 1.0f);
        else
            renderizarTexto(20, y, itensMenu[i].nome, 0.2f, 0.2f, 0.25f);

        // Reabrir matrizes pro próximo item do loop
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, larguraJanela, 0, alturaJanela);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
//  desenharPainelRodape — código OpenGL no rodapé
// ============================================================
void desenharPainelRodape() {
    char codigo[256]    = "";
    char descricao[256] = "";

    if (moduloAtual == mod_objetos)
        obterCodigoObjeto(codigo, descricao);
    else {
        sprintf(codigo,    "// Modulo em construcao");
        sprintf(descricao, "Selecione 'Objetos' no menu lateral para ver o codigo.");
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, larguraJanela, 0, alturaJanela);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Fundo do painel inferior (cinza-claro, igual ao lateral)
    glColor3f(0.92f, 0.92f, 0.94f);
    glBegin(GL_QUADS);
        glVertex2f(LARGURA_PAINEL_LATERAL, 0);
        glVertex2f(larguraJanela,          0);
        glVertex2f(larguraJanela,          ALTURA_PAINEL_RODAPE);
        glVertex2f(LARGURA_PAINEL_LATERAL, ALTURA_PAINEL_RODAPE);
    glEnd();

    // Linha separadora
    glColor3f(0.7f, 0.7f, 0.75f);
    glBegin(GL_LINES);
        glVertex2f(LARGURA_PAINEL_LATERAL, ALTURA_PAINEL_RODAPE);
        glVertex2f(larguraJanela,          ALTURA_PAINEL_RODAPE);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Textos (cores escuras pro fundo claro)
    renderizarTexto(LARGURA_PAINEL_LATERAL + 15, 55,
        "[ Comando OpenGL ]", 0.2f, 0.4f, 0.8f);
    renderizarTexto(LARGURA_PAINEL_LATERAL + 15, 35,
        codigo, 0.1f, 0.5f, 0.2f);
    renderizarTexto(LARGURA_PAINEL_LATERAL + 15, 15,
        descricao, 0.35f, 0.35f, 0.4f);
}

// ============================================================
//  desenharHUDTopo — informações rápidas acima da viewport
// ============================================================
void desenharHUDTopo() {
    if (moduloAtual == mod_objetos) {
        char buf[64];
        sprintf(buf, "Objeto: %s  [setas para trocar]", nomeObjetoAtual());
        renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 20,
            buf, 0.15f, 0.15f, 0.2f);
    }

    renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 40,
        "Arrastar: girar | Scroll: zoom | W: wire/solido",
        0.45f, 0.45f, 0.5f);
}

// ============================================================
//  configurarIluminacao
// ============================================================
void configurarIluminacao() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat posLuz[]   = { 3.0f, 5.0f, 4.0f, 1.0f };
    GLfloat difusa[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambiente[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat especular[]= { 0.6f, 0.6f, 0.6f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  difusa);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambiente);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);

    GLfloat matEspec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR,  matEspec);
    glMaterialf (GL_FRONT, GL_SHININESS, 60.0f);
}

// ============================================================
//  display
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---- Viewport 3D: só a área que não é painel ----
    glViewport(LARGURA_PAINEL_LATERAL, ALTURA_PAINEL_RODAPE,
               larguraJanela - LARGURA_PAINEL_LATERAL,
               alturaJanela  - ALTURA_PAINEL_RODAPE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspecto = (float)(larguraJanela - LARGURA_PAINEL_LATERAL) /
                    (float)(alturaJanela  - ALTURA_PAINEL_RODAPE);
    gluPerspective(45.0, aspecto, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, zoom,  0, 0, 0,  0, 1, 0);

    configurarIluminacao();

    glRotatef(anguloX, 1.0f, 0.0f, 0.0f);
    glRotatef(anguloY, 0.0f, 1.0f, 0.0f);

    if (moduloAtual == mod_objetos) {
        desenharObjeto();
    }

    // ---- Painéis 2D: ocupam a janela inteira ----
    glViewport(0, 0, larguraJanela, alturaJanela);

    desenharPainelLateral();
    desenharPainelRodape();
    desenharHUDTopo();

    glutSwapBuffers();
}

// ============================================================
//  reshape
// ============================================================
void reshape(int w, int h) {
    if (h == 0) h = 1;
    larguraJanela = w;
    alturaJanela  = h;
}

// ============================================================
//  Mouse
// ============================================================
void mouseClick(int botao, int estado, int x, int y) {
    // Clique no painel lateral seleciona módulo
    if (estado == GLUT_DOWN && botao == GLUT_LEFT_BUTTON &&
        x < LARGURA_PAINEL_LATERAL) {

        int yInvertido = alturaJanela - y;
        float yInicial = alturaJanela - 50.0f;
        float espaco   = 50.0f;

        for (int i = 0; i < NUM_ITENS_MENU; i++) {
            float yTopo  = yInicial - i * espaco + 18;
            float yBase  = yInicial - i * espaco - 14;
            if (yInvertido <= yTopo && yInvertido >= yBase) {
                moduloAtual = itensMenu[i].modulo;
                break;
            }
        }
        glutPostRedisplay();
        return;
    }

    botaoPressionado = (estado == GLUT_DOWN) ? botao : -1;
    mouseX = x;
    mouseY = y;

    if (botao == 3) { zoom -= 0.3f; if (zoom < 1.5f) zoom = 1.5f; }
    if (botao == 4) { zoom += 0.3f; if (zoom > 20.0f) zoom = 20.0f; }

    glutPostRedisplay();
}

void mouseMove(int x, int y) {
    if (botaoPressionado == GLUT_LEFT_BUTTON && x > LARGURA_PAINEL_LATERAL) {
        anguloY += (x - mouseX) * 0.5f;
        anguloX += (y - mouseY) * 0.5f;
    }
    mouseX = x;
    mouseY = y;
    glutPostRedisplay();
}

// ============================================================
//  Teclado
// ============================================================
void teclaEspecial(int key, int x, int y) {
    if (moduloAtual == mod_objetos) {
        int obj = (int)objetoAtual;
        if (key == GLUT_KEY_RIGHT) obj = (obj + 1) % 8;
        if (key == GLUT_KEY_LEFT)  obj = (obj + 7) % 8;
        objetoAtual = (TipoObjeto)obj;
    }
    glutPostRedisplay();
}

void teclado(unsigned char key, int x, int y) {
    if (key == 'w' || key == 'W') modoWire = !modoWire;
    if (key == 27) exit(0);
    glutPostRedisplay();
}

// ============================================================
//  init — fundo BRANCO
// ============================================================
void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // branco
    glEnable(GL_DEPTH_TEST);
}

// ============================================================
//  main
// ============================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(larguraJanela, alturaJanela);
    glutCreateWindow("Simulador OpenGL");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclaEspecial);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);

    glutMainLoop();
    return 0;
}