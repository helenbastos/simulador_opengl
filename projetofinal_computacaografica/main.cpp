#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "objetos.h"
#include "transformacoes.h"
#include "projecoes.h"
#include "iluminacao.h"

// ============================================================
//  Estado de interação com o mouse
// ============================================================
static int   mouseX = 0, mouseY = 0;
static int   botaoPressionado = -1;
static float anguloX = 20.0f;
static float anguloY = -30.0f;
static float zoom    =  6.0f;
static bool look_at = false;


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

void desenharCodigoTransformacao()
{
    char valorX[32];
    char valorY[32];

    sprintf(valorX,"%.2f",eixosTransformacoes.xtranslate);
    sprintf(valorY,"%.2f",eixosTransformacoes.ytranslate);

    float x = LARGURA_PAINEL_LATERAL + 15;
    float y = 35;

    renderizarTexto(x,y,
        "glTranslatef(",
        0.1f,0.1f,0.1f);

    renderizarTexto(
        x+100,y,
        valorX,
        parametroAtual == param_x ?
            1.0f : 0.1f,
        parametroAtual == param_x ?
            0.8f : 0.5f,
        0.2f);

    renderizarTexto(x+150,y,", ",
        0.1f,0.1f,0.1f);

    renderizarTexto(
        x+170,y,
        valorY,
        parametroAtual == param_y ?
            1.0f : 0.1f,
        parametroAtual == param_y ?
            0.8f : 0.5f,
        0.2f);

    renderizarTexto(
        x+220,y,
        ", 0.00);",
        0.1f,0.1f,0.1f);
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
    else if (moduloAtual == mod_transformacoes)
        obterCodigoTransformacao(codigo, descricao);
    else if(moduloAtual==mod_projecoes)
        obterCodigoProjecao(codigo, descricao);
    else if (moduloAtual == mod_iluminacao)
        obterCodigoIluminacao(codigo, descricao);
    else {
        sprintf(codigo,    "// Modulo em construcao");
        //sprintf(descricao, "Selecione 'Objetos' no menu lateral para ver o codigo.");
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
    if(moduloAtual == mod_transformacoes)
    {
        desenharCodigoTransformacao();
    }
    else
    {
        renderizarTexto(
            LARGURA_PAINEL_LATERAL + 15,35,codigo,
            0.1f,0.5f,0.2f);
    }

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
         renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 40,
        "Arrastar: girar | Scroll: zoom | W: wire/solido",
        0.45f, 0.45f, 0.5f);
    }else if(moduloAtual==mod_transformacoes){
        renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 20,
        "Transformacoes Geometricas ", 0.15f, 0.15f, 0.2f);
        renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 40,
        "Arrastar: Rotacao | Scroll: zoom | T: Translacao/Escala [setas para modificar valores dos eixos x e y]",
        0.45f, 0.45f, 0.5f);
    }else if(moduloAtual=mod_projecoes){
        char buf[100];
        sprintf(buf, "Projecao: %s  [setas left/right para trocar]", nomeProjecaoAtual());
        renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 20,
            buf, 0.15f, 0.15f, 0.2f);
        obterComandosTeclas(buf);
        renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 40,
            buf, 0.45f, 0.45f, 0.5f); 
        renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 60,
            "Arrastar: Rotacao | Scroll: zoom | [setas up/down para aumentar/diminuir]", 0.45f, 0.45f, 0.5f); 
    }
    else if (moduloAtual == mod_iluminacao) {
    char buf[128];
    sprintf(buf, "Luz: %s | L: trocar luz | M: material | S: flat/smooth | Setas: mover luz", nomeLuzAtual());
    renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 20, buf, 0.15f, 0.15f, 0.2f);
    renderizarTexto(LARGURA_PAINEL_LATERAL + 15, alturaJanela - 40,
        "Objetos: setas (no menu Objetos) | +/-: angulo spot | D/d: intensidade difusa",
        0.45f, 0.45f, 0.5f);
    }
   
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

    if(moduloAtual!=mod_projecoes){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspecto = (float)(larguraJanela - LARGURA_PAINEL_LATERAL) /   
                    (float)(alturaJanela  - ALTURA_PAINEL_RODAPE);  
        gluPerspective(45.0, aspecto, 0.1, 100.0);                              

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 2, zoom,  0, 0, 0,  0, 1, 0);
    }else{
        eixosProjecoes.aspectPerspective = (float)(larguraJanela - LARGURA_PAINEL_LATERAL) /   
                    (float)(alturaJanela  - ALTURA_PAINEL_RODAPE);  
        desenharProjecao();
    }
    
    configurarIluminacao();

    glRotatef(anguloX, 1.0f, 0.0f, 0.0f);
    glRotatef(anguloY, 0.0f, 1.0f, 0.0f);

    if (moduloAtual == mod_objetos) {
        desenharObjeto();
    }else if(moduloAtual==mod_transformacoes){
        desenharTransformacao();
    }else if(moduloAtual==mod_projecoes){
        desenhaImagemProjecao();
    }

    //mod_iluminacao
    if (moduloAtual == mod_iluminacao) {
        desenharIluminacao();
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
    eixosProjecoes.zoom=zoom;
    glutPostRedisplay();
}

void mouseMove(int x, int y) {
    if (botaoPressionado == GLUT_LEFT_BUTTON && x > LARGURA_PAINEL_LATERAL) {
        anguloY += (x - mouseX) * 0.5f;
        anguloX += (y - mouseY) * 0.5f;

        parametroAtual = param_x;
        eixosTransformacoes.xrotate=anguloX;
        parametroAtual = param_y;
        eixosTransformacoes.yrotate=anguloY;
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
        if (key == GLUT_KEY_RIGHT) obj = (obj + 1) % NUM_OBJETOS;
        if (key == GLUT_KEY_LEFT)  obj = (obj + 7) % NUM_OBJETOS;
        objetoAtual = (TipoObjeto)obj;
    }else if(moduloAtual==mod_transformacoes){
        if (transformacaoAtual==transformacao_translacao){
            if (key == GLUT_KEY_RIGHT) eixosTransformacoes.xtranslate +=0.5f; if(eixosTransformacoes.xtranslate>6.0f) eixosTransformacoes.xtranslate=6.0f;
            if (key == GLUT_KEY_LEFT)  eixosTransformacoes.xtranslate -=0.5f; if(eixosTransformacoes.xtranslate<-6.0f) eixosTransformacoes.xtranslate=6.0f;
            //printf("eixo x: %0.2f\n",eixosTransformacoes.xtranslate);
            if (key == GLUT_KEY_UP) eixosTransformacoes.ytranslate += 0.5f; if(eixosTransformacoes.ytranslate>4.0f) eixosTransformacoes.ytranslate=4.0f;
            if (key == GLUT_KEY_DOWN) eixosTransformacoes.ytranslate -=0.5f; if (eixosTransformacoes.ytranslate<-4.0f) eixosTransformacoes.ytranslate=-4.0f;
            //printf("eixo y: %0.2f\n",eixosTransformacoes.ytranslate);
        }else if (transformacaoAtual==transformacao_escala){
            if (key == GLUT_KEY_RIGHT) eixosTransformacoes.xscale += 0.1f; if(eixosTransformacoes.xscale>1.8f) eixosTransformacoes.xscale = 1.8f;
            if (key == GLUT_KEY_LEFT)  eixosTransformacoes.xscale -= 0.1f; if(eixosTransformacoes.xscale<0.2f) eixosTransformacoes.xscale = 0.2f;
            if (key == GLUT_KEY_UP) eixosTransformacoes.yscale += 0.1f; if(eixosTransformacoes.yscale>1.8f) eixosTransformacoes.yscale = 1.8f;
            if (key == GLUT_KEY_DOWN) eixosTransformacoes.yscale -= 0.1f; if(eixosTransformacoes.yscale<0.2f) eixosTransformacoes.yscale = 0.2f;
        }
    }else if(moduloAtual==mod_projecoes){
        int proj = (int)projecaoAtual;
        if (key == GLUT_KEY_RIGHT) proj = (proj + 1) % 3;
        if (key == GLUT_KEY_LEFT)  proj = (proj + 2) % 3;
        projecaoAtual = (TipoProjecao)proj;
        //criar variáveis booleanas para modificar cada eixos com as setas up e down
        if(key==GLUT_KEY_UP) {
            switch(comandoAtual){
                case xMinOrtho: eixosProjecoes.xMinOrtho += 0.5; if(eixosProjecoes.xMinOrtho>8) eixosProjecoes.xMinOrtho = 8;
                    break;
                case yMinOrtho: eixosProjecoes.yMinOrtho += 0.5; if(eixosProjecoes.yMinOrtho >8) eixosProjecoes.yMinOrtho = 8;
                    break;
                case nearOrtho: eixosProjecoes.nearOrtho += 0.5; if(eixosProjecoes.nearOrtho>8) eixosProjecoes.nearOrtho = 8;
                    break;
                case xMinFrustum: eixosProjecoes.xMinFrustum +=0.5; if(eixosProjecoes.xMinFrustum>4) eixosProjecoes.xMinFrustum = 4;
                    break;
                case yMinFrustum: eixosProjecoes.yMinFrustum +=0.5; if(eixosProjecoes.yMinFrustum>4) eixosProjecoes.yMinFrustum = 4;
                    break;
                case nearFrustum: eixosProjecoes.nearFrustum +=0.5; if(eixosProjecoes.nearFrustum>10) eixosProjecoes.nearFrustum = 10;
                    break;
                case fovyPerspective: eixosProjecoes.fovyPerspective +=5; if(eixosProjecoes.fovyPerspective>90) eixosProjecoes.fovyPerspective = 90;
                    break;
                case nearPerspective: eixosProjecoes.nearPerspective +=5; if(eixosProjecoes.nearPerspective>(eixosProjecoes.zoom)) eixosProjecoes.nearPerspective = (eixosProjecoes.zoom);
                    break;
                case farPerspective: eixosProjecoes.farPerspective+=10; if(eixosProjecoes.farPerspective>100) eixosProjecoes.farPerspective = 100;
                    break;
                default:
                    break;
            }
        }else if (key==GLUT_KEY_DOWN){
            switch(comandoAtual){
                case xMinOrtho:
                    eixosProjecoes.xMinOrtho -= 0.5; if(eixosProjecoes.xMinOrtho<-8) eixosProjecoes.xMinOrtho = -8;
                    break;
                case yMinOrtho:
                    eixosProjecoes.yMinOrtho -=0.5; if(eixosProjecoes.yMinOrtho <-8) eixosProjecoes.yMinOrtho = -8;
                    break;
                case nearOrtho: eixosProjecoes.nearOrtho -= 0.5; if(eixosProjecoes.nearOrtho <-50) eixosProjecoes.nearOrtho = -50;
                    break;
                case xMinFrustum: eixosProjecoes.xMinFrustum -=0.5; if(eixosProjecoes.xMinFrustum<-4) eixosProjecoes.xMinFrustum = -4;
                    break;
                case yMinFrustum: eixosProjecoes.yMinFrustum -=0.5; if(eixosProjecoes.yMinFrustum<-4) eixosProjecoes.yMinFrustum = -4;
                    break;
                case nearFrustum: eixosProjecoes.nearFrustum -=0.5; if(eixosProjecoes.nearFrustum<0.5) eixosProjecoes.nearFrustum = 0.5;
                    break;
                case fovyPerspective: eixosProjecoes.fovyPerspective -=0.5; if(eixosProjecoes.fovyPerspective<10) eixosProjecoes.fovyPerspective = 10;
                    break;
                case nearPerspective: eixosProjecoes.nearPerspective -=0.5; if(eixosProjecoes.nearPerspective<-0.4) eixosProjecoes.nearPerspective = -0.4;
                    break;
                case farPerspective: eixosProjecoes.farPerspective -=0.5; if(eixosProjecoes.farPerspective<(eixosProjecoes.zoom-0.6)) eixosProjecoes.farPerspective = eixosProjecoes.zoom-0.6;
                    break;
                default:
                    break;
            }
        }

    }

    glutPostRedisplay();
}

void teclado(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'w':
        case 'W':
            modoWire = !modoWire;
            break;

        case 't':
        case 'T':
            if(transformacaoAtual == transformacao_translacao)
                transformacaoAtual = transformacao_escala;
            else if(transformacaoAtual == transformacao_escala)
                transformacaoAtual = transformacao_rotacao;
            else
                transformacaoAtual = transformacao_translacao;

            parametroAtual = param_nenhum;
            break;

        case 27:
            exit(0);
            break;
    }

    if(moduloAtual == mod_projecoes){
      if(projecaoAtual==projecao_ortho){
          if(key=='X' || key == 'x') comandoAtual = xMinOrtho;
          else if(key=='Y' || key == 'y') comandoAtual = yMinOrtho;
          else if (key=='N' || key == 'n') comandoAtual = nearOrtho;
          else if(key=='R' || key == 'r') eixosProjecoes.xMinOrtho = -2; eixosProjecoes.yMinOrtho = -2; eixosProjecoes.nearOrtho = - 300; eixosProjecoes.zoom = 5;
      }else if(projecaoAtual==projecao_frustum){
          if(key=='X' || key == 'x') comandoAtual = xMinFrustum;
          else if(key=='Y' || key == 'y') comandoAtual = yMinFrustum;
          else if (key=='N' || key == 'n') comandoAtual = nearFrustum;
          else if(key=='R' || key == 'r') eixosProjecoes.xMinFrustum = -1; eixosProjecoes.yMinFrustum = -1; eixosProjecoes.nearFrustum = 5; eixosProjecoes.zoom = 5;
      }else if(projecaoAtual==projecao_perspective){
          if(key=='F' || key == 'f') comandoAtual = farPerspective;
          else if(key=='Y' || key == 'y') comandoAtual = fovyPerspective;
          else if (key=='N' || key == 'n') comandoAtual = nearPerspective;
          else if(key=='R' || key == 'r') eixosProjecoes.fovyPerspective = 45; eixosProjecoes.nearPerspective = 0.1; eixosProjecoes.farPerspective = 100; eixosProjecoes.zoom = 5;
      }
    }

    if (moduloAtual == mod_iluminacao) processarTecladoIluminacao(key);
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