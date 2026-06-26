#include "objetos.h"
#include "iluminacao.h"
#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "VGlobais.h"

// ============================================================
//  Estado interno do módulo de iluminação
// ============================================================

// Tipo de luz atual
static TipoLuz tipoLuzAtual = luz_direcional;

// Tipo de material atual
static TipoMaterial tipoMaterialAtual = mat_plastico;

// Shading: 0 = flat, 1 = smooth (Gouraud)
static int shadingSmooth = 1;

// Intensidade da componente difusa 
static float intensidadeDifusa = 0.8f;

// Intensidade da componente especular
static float intensidadeEspecular = 1.0f;

// brilho do material
static float shininess = 60.0f;

// Posição/direção da luz
static float posLuzX = 3.0f;
static float posLuzY = 5.0f;
static float posLuzZ = 4.0f;

// Ângulo do spot
static float spotCutoff = 30.0f;

// ============================================================
//  Tabela de materiais predefinidos
//  Cada entrada: {ambient R,G,B, diffuse R,G,B, specular R,G,B, shininess}
// ============================================================
struct MaterialDef {
    const char* nome;
    GLfloat ambiente[4];
    GLfloat difusa[4];
    GLfloat especular[4];
    GLfloat cor[4];      // cor base para glColor (com GL_COLOR_MATERIAL)
    GLfloat brilho;
};

static MaterialDef materiais[] = {
    // Plástico: difusa intensa, especular moderada, brilho médio
    {
        "Plastico",
        {0.1f, 0.1f, 0.3f, 1.0f},
        {0.2f, 0.4f, 0.9f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.2f, 0.4f, 0.9f, 1.0f},
        32.0f
    },
    // Metal: ambiente baixa, especular alta, brilho alto
    {
        "Metal",
        {0.2f, 0.2f, 0.2f, 1.0f},
        {0.55f, 0.55f, 0.55f, 1.0f},
        {0.9f, 0.9f, 0.9f, 1.0f},
        {0.6f, 0.6f, 0.65f, 1.0f},
        96.0f
    },
    // Borracha: difusa alta, especular quase nula, brilho baixo
    {
        "Borracha",
        {0.05f, 0.05f, 0.05f, 1.0f},
        {0.5f, 0.1f, 0.1f, 1.0f},
        {0.05f, 0.05f, 0.05f, 1.0f},
        {0.6f, 0.1f, 0.1f, 1.0f},
        8.0f
    },
    // Ceramica: ambiente moderada, especular concentrada, brilho muito alto
    {
        "Ceramica",
        {0.3f, 0.2f, 0.1f, 1.0f},
        {0.8f, 0.6f, 0.3f, 1.0f},
        {0.9f, 0.9f, 0.8f, 1.0f},
        {0.8f, 0.6f, 0.3f, 1.0f},
        128.0f
    }
};
static const int NUM_MATERIAIS = 4;

// ============================================================
//  aplicarMaterial — configura glMaterialfv com o material atual
// ============================================================
static void aplicarMaterial() {
    MaterialDef& m = materiais[tipoMaterialAtual];

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // glColor define ambiente e difusa via COLOR_MATERIAL
    glColor4fv(m.cor);

    // especular e shininess ainda precisam de glMaterialfv
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m.especular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m.brilho);
}

// ============================================================
//  configurarLuz — define os parâmetros de GL_LIGHT0 de acordo
//  com o tipo de luz selecionado
// ============================================================
static void configurarLuz() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // Shading model
    if (shadingSmooth)
        glShadeModel(GL_SMOOTH);   // Gouraud
    else
        glShadeModel(GL_FLAT);

    // Componentes comuns a todos os tipos
    GLfloat ambiente[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat difusa[]    = { intensidadeDifusa, intensidadeDifusa, intensidadeDifusa, 1.0f };
    GLfloat especular[] = { intensidadeEspecular, intensidadeEspecular, intensidadeEspecular, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);

    // Desabilitar spot por padrão (cutoff 180 = luz em todas as direções)
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

    switch (tipoLuzAtual) {

        case luz_ambiental: {
            // Luz ambiental: sem posição relevante, só componente ambiente intensa
            // Desligamos a difusa e especular para simular luz puramente ambiente
            GLfloat ambForte[] = { 0.8f, 0.8f, 0.8f, 1.0f };
            GLfloat zero[]     = { 0.0f, 0.0f, 0.0f, 1.0f };
            GLfloat pos[]      = { 0.0f, 0.0f, 1.0f, 0.0f }; // direcional qualquer
            glLightfv(GL_LIGHT0, GL_AMBIENT,  ambForte);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  zero);
            glLightfv(GL_LIGHT0, GL_SPECULAR, zero);
            glLightfv(GL_LIGHT0, GL_POSITION, pos);
            break;
        }

        case luz_direcional: {
            // w = 0.0 → luz direcional (raios paralelos, como o sol)
            GLfloat pos[] = { posLuzX, posLuzY, posLuzZ, 0.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, pos);
            break;
        }

        case luz_pontual: {
            // w = 1.0 → luz pontual (emite em todas as direções a partir de um ponto)
            GLfloat pos[] = { posLuzX, posLuzY, posLuzZ, 1.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, pos);
            // Atenuação com a distância
            glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
            glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.05f);
            glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
            break;
        }

        case luz_spot: {
            // Spot: posição + direção + ângulo de corte
            GLfloat pos[]     = { posLuzX, posLuzY, posLuzZ, 1.0f };
            GLfloat direcao[] = { -posLuzX, -posLuzY, -posLuzZ }; // aponta para a origem
            glLightfv(GL_LIGHT0, GL_POSITION,       pos);
            glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direcao);
            glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,    spotCutoff);
            glLightf (GL_LIGHT0, GL_SPOT_EXPONENT,  12.0f);
            break;
        }
    }
}

// ============================================================
//  desenharObjetoAtual
//  reutilizando objetoAtual de VGlobais.h
// ============================================================
static void desenharObjetoAtual() {
    desenharObjeto();
}
// ============================================================
//  desenharIndicadorLuz — bolinha amarela na posição da luz
// ============================================================
static void desenharIndicadorLuz() {
    if (tipoLuzAtual == luz_ambiental) return; // ambiente não tem posição

    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(posLuzX, posLuzY, posLuzZ);
        glColor3f(1.0f, 1.0f, 0.0f); // amarelo
        glutSolidSphere(0.12, 10, 10);

        // Se for spot, desenha uma linha indicando a direção
        if (tipoLuzAtual == luz_spot) {
            glColor3f(1.0f, 0.9f, 0.0f);
            glBegin(GL_LINES);
                glVertex3f(0, 0, 0);
                glVertex3f(-posLuzX * 0.4f, -posLuzY * 0.4f, -posLuzZ * 0.4f);
            glEnd();
        }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

// ============================================================
//  desenharIluminacao — função principal chamada pela main
// ============================================================

void desenharIluminacao()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    configurarLuz();
    aplicarMaterial();

    desenharObjetoAtual();
    desenharIndicadorLuz();

    glPopAttrib();
}

// ============================================================
//  obterCodigoIluminacao — gera o feedback para o rodapé
// ============================================================
void obterCodigoIluminacao(char* codigo, char* descricao) {
    MaterialDef& m = materiais[tipoMaterialAtual];

    switch (tipoLuzAtual) {
        case luz_ambiental:
            sprintf(codigo,
                "glLightfv(GL_LIGHT0, GL_AMBIENT, {0.8,0.8,0.8,1});");
            sprintf(descricao,
                "Ilumina tudo igualmente, sem direcao. Material: %s", m.nome);
            break;

        case luz_direcional:
            sprintf(codigo,
                "glLightfv(GL_LIGHT0, GL_POSITION, {%.1f,%.1f,%.1f,0.0f});",
                posLuzX, posLuzY, posLuzZ);
            sprintf(descricao,
                "Componente  w=0: raios paralelos. Shading: %s. Material: %s",
                shadingSmooth ? "GL_SMOOTH" : "GL_FLAT", m.nome);
            break;

        case luz_pontual:
            sprintf(codigo,
                "glLightfv(GL_LIGHT0, GL_POSITION, {%.1f,%.1f,%.1f,1.0f});",
                posLuzX, posLuzY, posLuzZ);
            sprintf(descricao,
                "Componente w=1: emite em todas direcoes. Material: %s, Brilho: %.0f",
                m.nome, m.brilho);
            break;

        case luz_spot:
            sprintf(codigo,
                "glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, %.1f); // cone de luz",
                spotCutoff);
            sprintf(descricao,
                "Cone de %.0f graus apontando para o objeto. Material: %s",
                spotCutoff, m.nome);
            break;
    }
}

// ============================================================
//  nomeLuzAtual — para uso no HUD da main
// ============================================================
const char* nomeLuzAtual() {
    switch (tipoLuzAtual) {
        case luz_ambiental:  return "Ambiental";
        case luz_direcional: return "Direcional";
        case luz_pontual:    return "Pontual";
        case luz_spot:       return "Spot";
    }
    return "?";
}

// ============================================================
//  processarTecladoIluminacao — teclas normais
//  Chamar da função teclado() da main quando moduloAtual == mod_iluminacao
// ============================================================
void processarTecladoIluminacao(unsigned char key) {
    // S: alterna shading flat/smooth
    if (key == 's' || key == 'S') {
        shadingSmooth = !shadingSmooth;
    }
    // M: próximo material
    if (key == 'm' || key == 'M') {
        tipoMaterialAtual = (TipoMaterial)((tipoMaterialAtual + 1) % NUM_MATERIAIS);
    }
    // L: próximo tipo de luz
    if (key == 'l' || key == 'L') {
        tipoLuzAtual = (TipoLuz)((tipoLuzAtual + 1) % 4);
    }
    // + / - : ajusta cutoff do spot
    if (key == '+') { spotCutoff += 5.0f; if (spotCutoff > 90.0f) spotCutoff = 90.0f; }
    if (key == '-') { spotCutoff -= 5.0f; if (spotCutoff < 5.0f)  spotCutoff = 5.0f;  }
    // D / d : ajusta intensidade difusa
    if (key == 'D') { intensidadeDifusa += 0.1f; if (intensidadeDifusa > 1.0f) intensidadeDifusa = 1.0f; }
    if (key == 'd') { intensidadeDifusa -= 0.1f; if (intensidadeDifusa < 0.0f) intensidadeDifusa = 0.0f; }
}

// ============================================================
//  processarTeclaEspecialIluminacao — setas movem a luz
//  Chamar da função teclaEspecial() da main quando mod_iluminacao
// ============================================================
void processarTeclaEspecialIluminacao(int key) {
    if (tipoLuzAtual == luz_ambiental) return; // ambiente não tem posição

    if (key == GLUT_KEY_RIGHT) posLuzX += 0.3f;
    if (key == GLUT_KEY_LEFT)  posLuzX -= 0.3f;
    if (key == GLUT_KEY_UP)    posLuzY += 0.3f;
    if (key == GLUT_KEY_DOWN)  posLuzY -= 0.3f;
}
