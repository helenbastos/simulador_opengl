#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "texturas.h"
#include "VGlobais.h"
#include "objetos.h"

#define PI 3.14159265358979323846f
#define TEX_1D_SIZE  16   // número de texels na textura 1D
#define TEX_2D_SIZE  64   // largura/altura da textura 2D (potência de 2)

// ============================================================
//  Estado interno do módulo
// ============================================================
static TipoTextura   tipoTexturaAtual  = tex_2d;
static PadraoTextura padraoAtual       = padrao_xadrez;
static ModoTextura   modoAtual         = modo_modulate;
static FiltroTextura filtroAtual       = filtro_linear;

// IDs das texturas geradas
static GLuint idTex1D = 0;
static GLuint idTex2D = 0;

// Flag para recriar textura quando parâmetros mudam
static int texturaSuja = 1;

// ============================================================
//  Geração de texturas procedurais 1D
// ============================================================
static void gerarTextura1D() {
    // Vetor RGB com TEX_1D_SIZE cores — gradiente espectral
    GLubyte dados[TEX_1D_SIZE][3];

    for (int i = 0; i < TEX_1D_SIZE; i++) {
        float t = (float)i / (TEX_1D_SIZE - 1); // 0.0 a 1.0

        switch (padraoAtual) {
            case padrao_gradiente:
                // Gradiente vermelho → azul
                dados[i][0] = (GLubyte)((1.0f - t) * 255);
                dados[i][1] = (GLubyte)(sinf(t * PI) * 200);
                dados[i][2] = (GLubyte)(t * 255);
                break;
            case padrao_listras:
                // Alterna entre amarelo e azul escuro
                if ((i % 4) < 2) {
                    dados[i][0] = 255; dados[i][1] = 220; dados[i][2] = 0;
                } else {
                    dados[i][0] = 20;  dados[i][1] = 60;  dados[i][2] = 180;
                }
                break;
            case padrao_xadrez:
            case padrao_circular:
                // Para 1D, xadrez e circular viram listras simples
                dados[i][0] = (GLubyte)((sinf(t * PI * 4) * 0.5f + 0.5f) * 255);
                dados[i][1] = (GLubyte)((cosf(t * PI * 4) * 0.5f + 0.5f) * 200);
                dados[i][2] = 180;
                break;
        }
    }

    if (idTex1D == 0) glGenTextures(1, &idTex1D);
    glBindTexture(GL_TEXTURE_1D, idTex1D);

    // Filtros
    GLenum filtroGL = (filtroAtual == filtro_linear) ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // Envia textura para a GPU
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB,
                 TEX_1D_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);
}

// ============================================================
//  Geração de texturas procedurais 2D
// ============================================================
static void gerarTextura2D() {
    GLubyte dados[TEX_2D_SIZE][TEX_2D_SIZE][3];

    for (int y = 0; y < TEX_2D_SIZE; y++) {
        for (int x = 0; x < TEX_2D_SIZE; x++) {
            float s = (float)x / TEX_2D_SIZE; // 0..1
            float t = (float)y / TEX_2D_SIZE; // 0..1

            GLubyte r, g, b;

            switch (padraoAtual) {
                case padrao_xadrez: {
                    // Tabuleiro de xadrez clássico
                    int qx = (int)(s * 8);
                    int qy = (int)(t * 8);
                    if ((qx + qy) % 2 == 0) {
                        r = 240; g = 60; b = 60;   // vermelho
                    } else {
                        r = 60; g = 60; b = 240;   // azul
                    }
                    break;
                }
                case padrao_listras: {
                    // Listras horizontais coloridas
                    int faixa = (int)(t * 8) % 4;
                    if      (faixa == 0) { r=255; g=80;  b=80;  }
                    else if (faixa == 1) { r=80;  g=255; b=80;  }
                    else if (faixa == 2) { r=80;  g=80;  b=255; }
                    else                 { r=255; g=255; b=80;  }
                    break;
                }
                case padrao_gradiente: {
                    // Gradiente diagonal
                    r = (GLubyte)(s * 255);
                    g = (GLubyte)(t * 255);
                    b = (GLubyte)((1.0f - s * 0.5f - t * 0.5f) * 255);
                    break;
                }
                case padrao_circular: {
                    // Padrão de anéis concêntricos
                    float dx = s - 0.5f;
                    float dy = t - 0.5f;
                    float dist = sqrtf(dx*dx + dy*dy) * 2.0f;
                    float val = sinf(dist * PI * 6) * 0.5f + 0.5f;
                    r = (GLubyte)(val * 200 + 55);
                    g = (GLubyte)((1.0f - val) * 180 + 40);
                    b = (GLubyte)(sinf(dist * PI * 3) * 100 + 155);
                    break;
                }
                default:
                    r = g = b = 200;
            }

            dados[y][x][0] = r;
            dados[y][x][1] = g;
            dados[y][x][2] = b;
        }
    }

    if (idTex2D == 0) glGenTextures(1, &idTex2D);
    glBindTexture(GL_TEXTURE_2D, idTex2D);

    // Filtros
    GLenum filtroGL = (filtroAtual == filtro_linear) ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Envia textura para a GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 TEX_2D_SIZE, TEX_2D_SIZE, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, dados);
}

// ============================================================
//  inicializarTexturas — chamada uma vez na init() da main
// ============================================================
void inicializarTexturas() {
    gerarTextura1D();
    gerarTextura2D();
    texturaSuja = 0;
}

// ============================================================
//  aplicarModoTextura — define como a textura interage com a cor
// ============================================================
static void aplicarModoTextura(GLenum target) {
    GLenum modoGL;
    switch (modoAtual) {
        case modo_modulate: modoGL = GL_MODULATE; break;
        case modo_decal:    modoGL = GL_DECAL;    break;
        case modo_replace:  modoGL = GL_REPLACE;  break;
        default:            modoGL = GL_MODULATE;
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modoGL);
}

// ============================================================
//  configurarTexGen — geração automática de coordenadas de textura
//  Permite que QUALQUER objeto receba textura sem glTexCoord manual
// ============================================================
static void configurarTexGen() {
    if (tipoTexturaAtual == tex_1d) {
        glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        GLfloat planoS[] = { 1.0f, 0.0f, 0.0f, 0.0f };
        glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
    } else {
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        GLfloat planoS[] = { 1.0f, 0.0f, 1.0f, 0.0f };
        GLfloat planoT[] = { 0.0f, 1.0f, 1.0f, 0.0f };  // <- muda aqui
        glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, planoT);
    }
}

// ============================================================
//  desenharTexturas — função principal chamada pela main
// ============================================================
void desenharTexturas() {
    if (moduloAtual != mod_texturas) return;

    if (texturaSuja) {
        gerarTextura1D();
        gerarTextura2D();
        texturaSuja = 0;
    }

    // Iluminação só no modo modulate
    if (modoAtual == modo_modulate) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    } else {
        glDisable(GL_LIGHTING);
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    if (tipoTexturaAtual == tex_1d) {
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, idTex1D);
        aplicarModoTextura(GL_TEXTURE_1D);
    } else {
        glDisable(GL_TEXTURE_1D);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTex2D);
        aplicarModoTextura(GL_TEXTURE_2D);
    }

    configurarTexGen();
    desenharObjeto();

    // Restaurar iluminação
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
}

// ============================================================
//  nomeTipoTexturaAtual
// ============================================================
const char* nomeTipoTexturaAtual() {
    if (tipoTexturaAtual == tex_1d) return "1D";
    return "2D";
}

// ============================================================
//  obterCodigoTextura — feedback no rodapé
// ============================================================
void obterCodigoTextura(char* codigo, char* descricao) {
    const char* filtroStr = (filtroAtual == filtro_linear) ? "GL_LINEAR" : "GL_NEAREST";
    const char* modoStr;
    switch (modoAtual) {
        case modo_modulate: modoStr = "GL_MODULATE"; break;
        case modo_decal:    modoStr = "GL_DECAL";    break;
        case modo_replace:  modoStr = "GL_REPLACE";  break;
        default:            modoStr = "GL_MODULATE";
    }

    const char* padraoStr;
    switch (padraoAtual) {
        case padrao_xadrez:    padraoStr = "Xadrez";    break;
        case padrao_listras:   padraoStr = "Listras";   break;
        case padrao_gradiente: padraoStr = "Gradiente"; break;
        case padrao_circular:  padraoStr = "Circular";  break;
        default:               padraoStr = "?";
    }

    if (tipoTexturaAtual == tex_1d) {
        sprintf(codigo,
            "glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, %d, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);",
            TEX_1D_SIZE);
        sprintf(descricao,
            "Tex1D | Padrao: %s | Filtro: %s | Modo: %s | TexGen: GL_OBJECT_LINEAR",
            padraoStr, filtroStr, modoStr);
    } else {
        sprintf(codigo,
            "glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, %d, %d, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);",
            TEX_2D_SIZE, TEX_2D_SIZE);
        sprintf(descricao,
            "Tex2D | Padrao: %s | Filtro: %s | Modo: %s | TexGen: GL_OBJECT_LINEAR",
            padraoStr, filtroStr, modoStr);
    }
}

// ============================================================
//  processarTecladoTexturas
//  T: alterna 1D/2D
//  P: próximo padrão
//  F: alterna filtro nearest/linear
//  M: próximo modo de aplicação
// ============================================================
void processarTecladoTexturas(unsigned char key) {
    if (key == 't' || key == 'T') {
        tipoTexturaAtual = (TipoTextura)((tipoTexturaAtual + 1) % 2);
        texturaSuja = 1;
    }
    if (key == 'p' || key == 'P') {
        if (tipoTexturaAtual == tex_1d) {
            // 1D só alterna entre gradiente e listras
            if (padraoAtual == padrao_gradiente) padraoAtual = padrao_listras;
            else padraoAtual = padrao_gradiente;
        } else {
            // 2D alterna entre todos os 4
            padraoAtual = (PadraoTextura)((padraoAtual + 1) % 4);
        }
        texturaSuja = 1;
    }
    if (key == 'f' || key == 'F') {
        filtroAtual = (FiltroTextura)((filtroAtual + 1) % 2);
        texturaSuja = 1;
    }
    if (key == 'm' || key == 'M') {
        modoAtual = (ModoTextura)((modoAtual + 1) % 3);
        texturaSuja = 1;
    }
}
